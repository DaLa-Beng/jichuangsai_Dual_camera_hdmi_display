//****************************************Copyright (c)***********************************//
//原子哥在线教学平台：www.yuanzige.com
//技术支持：www.openedv.com
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xil_types.h"
#include "xil_cache.h"
#include "xparameters.h"
#include "xaxivdma.h"
#include "xaxivdma_i.h"
#include "display_ctrl_hdmi/display_ctrl.h"
#include "vdma_api/vdma_api.h"
#include "emio_sccb_cfg/emio_sccb_cfg.h"
#include "ov5640/ov5640_init.h"
#include "osd/osd.h"
#include "ff.h"
#include "sleep.h"

//宏定义
#define FRAME_BUFFER_NUM   3                           //帧缓存个数3
#define DYNCLK_BASEADDR    XPAR_AXI_DYNCLK_0_BASEADDR  //动态时钟基地址
#define VDMA_ID            XPAR_AXIVDMA_0_DEVICE_ID    //VDMA器件ID
#define DISP_VTC_ID        XPAR_VTC_0_DEVICE_ID        //VTC器件ID

static FATFS fatfs;
//BMP 图片文件头
u8 bmp_head[54] = { 0x42, 0x4d, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x36,
		0x0, 0x0, 0x0, 0x28, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
		0x0, 0x1, 0x0, 0x18, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc4,
		0xe, 0x0, 0x0, 0xc4, 0x0e, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
		0x0 };
//BMP 图片各参数偏移地址
UINT *bf_size = (UINT *) (bmp_head + 0x2);
UINT *bmp_width = (UINT *) (bmp_head + 0x12);
UINT *bmp_height = (UINT *) (bmp_head + 0x16);
UINT *bmp_size = (UINT *) (bmp_head + 0x22);
//BMP 图片编号
int pic_cnt = 0;

//抓拍的图片显存地址
unsigned int const bmp_addr = (XPAR_PS7_DDR_0_S_AXI_BASEADDR + 0x4000000);


//全局变量
//frame buffer的起始地址
unsigned int frame_buffer_addr[DISPLAY_VDMA_NUM];
unsigned int vdma_id[DISPLAY_VDMA_NUM] = {XPAR_AXIVDMA_0_DEVICE_ID,
		                                  XPAR_AXIVDMA_1_DEVICE_ID};

XAxiVdma     vdma[DISPLAY_VDMA_NUM];
DisplayCtrl  dispCtrl;
VideoMode    vd_mode;

//向 SD 卡中写 BMP 图片
void write_sd_bmp(u8 *frame) {
	FIL fil; //文件对象
	UINT bw; //写文件函数返回已写入的字节数
	char pic_name[20]; //字符串，用于存储 BMP 文件名

	//打印 BMP 图片信息(宽/高/图片大小),以及 BMP 文件大小
	xil_printf("width = %d, height = %d, size = %d, file size = %d bytes \n\r",
			*bmp_width, *bmp_height, *bmp_size, *bf_size);

	//给 BMP 图片的文件名编号
	sprintf(pic_name, "picture %04u.bmp", pic_cnt);
	//打开 BMP 文件,如果不存在则创建该文件
	f_open(&fil, pic_name, FA_CREATE_ALWAYS | FA_WRITE);

	//移动文件读写指针到文件开头
	f_lseek(&fil, 0);
	//写入 BMP 文件头
	f_write(&fil, bmp_head, 54, &bw);
	//写入抓拍的图片
	f_write(&fil, frame, *bmp_size, &bw);
	//关闭文件
	f_close(&fil);
	xil_printf("write %s done! \n\r", pic_name);
}
//初始化文件系统
int platform_init_fs()
{
	FRESULT status;
	TCHAR *Path = "0:/";
	BYTE work[FF_MAX_SS];

	//注册一个工作区(挂载分区文件系统)
	//在使用任何其它文件函数之前，必须使用 f_mount 函数为每个使用卷注册一个工作区
	status = f_mount(&fatfs, Path, 1); //挂载 SD 卡
	if (status != FR_OK) {
	xil_printf("Volume is not FAT formated; formating FAT\r\n");
	//格式化 SD 卡
	status = f_mkfs(Path, FM_FAT32, 0, work, sizeof work);
	if (status != FR_OK) {
	xil_printf("Unable to format FATfs\r\n");
	return -1;
}
//格式化之后，重新挂载 SD 卡
	status = f_mount(&fatfs, Path, 1);
	if (status != FR_OK) {
	xil_printf("Unable to mount FATfs\r\n");
	return -1;
	}
	}
	return 0;
}

//挂载 SD(TF)卡
int sd_mount()
{
	FRESULT status;
	//初始化文件系统（挂载 SD 卡，如果挂载不成功，则格式化 SD 卡）
	status = platform_init_fs();
	if(status){
	xil_printf("ERROR: f_mount returned %d!\n",status);
	return XST_FAILURE;
	}
	return XST_SUCCESS;
}
int main(void)
{
	u32 status0,status1;
	u16 cmos_h_pixel;   //ov5640 DVP 输出水平像素点数
	u16 cmos_v_pixel;   //ov5640 DVP 输出垂直像素点数
	u16 total_h_pixel;  //ov5640 水平总像素大小
	u16 total_v_pixel;  //ov5640 垂直总像素大小

	int rd_index; //VDMA 读通道操作的帧缓存编号
	unsigned int rd_fram_addr; //VDMA 读通道操作的帧缓存地址

	cmos_h_pixel = 1024;//设置OV5640输出分辨率为1024*600

	cmos_v_pixel = 600;
	total_h_pixel = 2200;
	total_v_pixel = 1000;

	emio_init();                                //初始化EMIO
	status0 = ov5640_init(CMOS_CH0,cmos_h_pixel,//初始化ov5640 0
						  cmos_v_pixel,
						 total_h_pixel,
						 total_v_pixel);
	status1 = ov5640_init(CMOS_CH1,cmos_h_pixel,//初始化ov5640 1
						  cmos_v_pixel,
						 total_h_pixel,
						 total_v_pixel);
	if(status0 == 0 && status1 == 0)
		xil_printf("Dual OV5640 detected successful!\r\n");
	else
		xil_printf("Dual OV5640 detected failed!\r\n");

	vd_mode = VMODE_1024x600;

	for(u8 i=0;i<DISPLAY_VDMA_NUM;i++){
		frame_buffer_addr[i] = XPAR_PS7_DDR_0_S_AXI_BASEADDR+0x1000000*i;
		//配置VDMA
		run_vdma_frame_buffer(&vdma[i], vdma_id[i], vd_mode.width/2, vd_mode.height,
								frame_buffer_addr[i],0,0,BOTH);
	}
	//OSD初始化
	osd_init(0x1016,vd_mode.width,vd_mode.height);
    //初始化Display controller
	DisplayInitialize(&dispCtrl, DISP_VTC_ID, DYNCLK_BASEADDR);
    //设置VideoMode
	DisplaySetMode(&dispCtrl, &vd_mode);
	DisplayStart(&dispCtrl);

	//根据 VDMA 显存大小给 BMP 文件头赋值
	*bmp_width = vd_mode.width/2;
	*bmp_height = vd_mode.height;
	*bmp_size = vd_mode.width * vd_mode.height * 3/2;
	*bf_size = *bmp_size + 54;
//	//挂载文件系统
//	f_mount(&fatfs, "", 1);
	sd_mount();

	while (1) {
		sleep(5);
		xil_printf("capture picture\n");

		//获取当前读通道操作的帧缓存编号
		rd_index = XAxiVdma_CurrFrameStore(&vdma[0], XAXIVDMA_READ);
		xil_printf("current read frame is %d\n", rd_index);
		//读通道驻停在当前帧
		XAxiVdma_StartParking(&vdma[0], rd_index, XAXIVDMA_READ);
		//并获取当前帧的起始地址
		rd_fram_addr = (unsigned int) (frame_buffer_addr[0]
				+ vd_mode.height * vd_mode.width * 3 * rd_index);
		//将当前帧的图像拷贝到抓拍图片缓存区域
		memcpy((void *) bmp_addr, (void *) rd_fram_addr,
				vd_mode.height * vd_mode.width * 3);
		//结束读通道驻停过程，继续在多帧之间进行切换
		XAxiVdma_StopParking(&vdma[0], XAXIVDMA_READ);
		//将抓拍图片缓存区域中的图像以 BMP 格式写入 SD 卡
		write_sd_bmp((u8 *) bmp_addr);
		//BMP 图片编号累加
		pic_cnt++;
		xil_printf("take picture success and the pic_cnt is %d",pic_cnt);
		}
    return 0;
}
