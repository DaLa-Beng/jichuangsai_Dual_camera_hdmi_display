//****************************************Copyright (c)***********************************//
//ԭ�Ӹ����߽�ѧƽ̨��www.yuanzige.com
//����֧�֣�www.openedv.com
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

//�궨��
#define FRAME_BUFFER_NUM   3                           //֡�������3
#define DYNCLK_BASEADDR    XPAR_AXI_DYNCLK_0_BASEADDR  //��̬ʱ�ӻ���ַ
#define VDMA_ID            XPAR_AXIVDMA_0_DEVICE_ID    //VDMA����ID
#define DISP_VTC_ID        XPAR_VTC_0_DEVICE_ID        //VTC����ID

static FATFS fatfs;
//BMP ͼƬ�ļ�ͷ
u8 bmp_head[54] = { 0x42, 0x4d, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x36,
		0x0, 0x0, 0x0, 0x28, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
		0x0, 0x1, 0x0, 0x18, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc4,
		0xe, 0x0, 0x0, 0xc4, 0x0e, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
		0x0 };
//BMP ͼƬ������ƫ�Ƶ�ַ
UINT *bf_size = (UINT *) (bmp_head + 0x2);
UINT *bmp_width = (UINT *) (bmp_head + 0x12);
UINT *bmp_height = (UINT *) (bmp_head + 0x16);
UINT *bmp_size = (UINT *) (bmp_head + 0x22);
//BMP ͼƬ���
int pic_cnt = 0;

//ץ�ĵ�ͼƬ�Դ��ַ
unsigned int const bmp_addr = (XPAR_PS7_DDR_0_S_AXI_BASEADDR + 0x4000000);


//ȫ�ֱ���
//frame buffer����ʼ��ַ
unsigned int frame_buffer_addr[DISPLAY_VDMA_NUM];
unsigned int vdma_id[DISPLAY_VDMA_NUM] = {XPAR_AXIVDMA_0_DEVICE_ID,
		                                  XPAR_AXIVDMA_1_DEVICE_ID};

XAxiVdma     vdma[DISPLAY_VDMA_NUM];
DisplayCtrl  dispCtrl;
VideoMode    vd_mode;

//�� SD ����д BMP ͼƬ
void write_sd_bmp(u8 *frame) {
	FIL fil; //�ļ�����
	UINT bw; //д�ļ�����������д����ֽ���
	char pic_name[20]; //�ַ��������ڴ洢 BMP �ļ���

	//��ӡ BMP ͼƬ��Ϣ(��/��/ͼƬ��С),�Լ� BMP �ļ���С
	xil_printf("width = %d, height = %d, size = %d, file size = %d bytes \n\r",
			*bmp_width, *bmp_height, *bmp_size, *bf_size);

	//�� BMP ͼƬ���ļ������
	sprintf(pic_name, "picture %04u.bmp", pic_cnt);
	//�� BMP �ļ�,����������򴴽����ļ�
	f_open(&fil, pic_name, FA_CREATE_ALWAYS | FA_WRITE);

	//�ƶ��ļ���дָ�뵽�ļ���ͷ
	f_lseek(&fil, 0);
	//д�� BMP �ļ�ͷ
	f_write(&fil, bmp_head, 54, &bw);
	//д��ץ�ĵ�ͼƬ
	f_write(&fil, frame, *bmp_size, &bw);
	//�ر��ļ�
	f_close(&fil);
	xil_printf("write %s done! \n\r", pic_name);
}
//��ʼ���ļ�ϵͳ
int platform_init_fs()
{
	FRESULT status;
	TCHAR *Path = "0:/";
	BYTE work[FF_MAX_SS];

	//ע��һ��������(���ط����ļ�ϵͳ)
	//��ʹ���κ������ļ�����֮ǰ������ʹ�� f_mount ����Ϊÿ��ʹ�þ�ע��һ��������
	status = f_mount(&fatfs, Path, 1); //���� SD ��
	if (status != FR_OK) {
	xil_printf("Volume is not FAT formated; formating FAT\r\n");
	//��ʽ�� SD ��
	status = f_mkfs(Path, FM_FAT32, 0, work, sizeof work);
	if (status != FR_OK) {
	xil_printf("Unable to format FATfs\r\n");
	return -1;
}
//��ʽ��֮�����¹��� SD ��
	status = f_mount(&fatfs, Path, 1);
	if (status != FR_OK) {
	xil_printf("Unable to mount FATfs\r\n");
	return -1;
	}
	}
	return 0;
}

//���� SD(TF)��
int sd_mount()
{
	FRESULT status;
	//��ʼ���ļ�ϵͳ������ SD ����������ز��ɹ������ʽ�� SD ����
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
	u16 cmos_h_pixel;   //ov5640 DVP ���ˮƽ���ص���
	u16 cmos_v_pixel;   //ov5640 DVP �����ֱ���ص���
	u16 total_h_pixel;  //ov5640 ˮƽ�����ش�С
	u16 total_v_pixel;  //ov5640 ��ֱ�����ش�С

	int rd_index; //VDMA ��ͨ��������֡������
	unsigned int rd_fram_addr; //VDMA ��ͨ��������֡�����ַ

	cmos_h_pixel = 1024;//����OV5640����ֱ���Ϊ1024*600

	cmos_v_pixel = 600;
	total_h_pixel = 2200;
	total_v_pixel = 1000;

	emio_init();                                //��ʼ��EMIO
	status0 = ov5640_init(CMOS_CH0,cmos_h_pixel,//��ʼ��ov5640 0
						  cmos_v_pixel,
						 total_h_pixel,
						 total_v_pixel);
	status1 = ov5640_init(CMOS_CH1,cmos_h_pixel,//��ʼ��ov5640 1
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
		//����VDMA
		run_vdma_frame_buffer(&vdma[i], vdma_id[i], vd_mode.width/2, vd_mode.height,
								frame_buffer_addr[i],0,0,BOTH);
	}
	//OSD��ʼ��
	osd_init(0x1016,vd_mode.width,vd_mode.height);
    //��ʼ��Display controller
	DisplayInitialize(&dispCtrl, DISP_VTC_ID, DYNCLK_BASEADDR);
    //����VideoMode
	DisplaySetMode(&dispCtrl, &vd_mode);
	DisplayStart(&dispCtrl);

	//���� VDMA �Դ��С�� BMP �ļ�ͷ��ֵ
	*bmp_width = vd_mode.width/2;
	*bmp_height = vd_mode.height;
	*bmp_size = vd_mode.width * vd_mode.height * 3/2;
	*bf_size = *bmp_size + 54;
//	//�����ļ�ϵͳ
//	f_mount(&fatfs, "", 1);
	sd_mount();

	while (1) {
		sleep(5);
		xil_printf("capture picture\n");

		//��ȡ��ǰ��ͨ��������֡������
		rd_index = XAxiVdma_CurrFrameStore(&vdma[0], XAXIVDMA_READ);
		xil_printf("current read frame is %d\n", rd_index);
		//��ͨ��פͣ�ڵ�ǰ֡
		XAxiVdma_StartParking(&vdma[0], rd_index, XAXIVDMA_READ);
		//����ȡ��ǰ֡����ʼ��ַ
		rd_fram_addr = (unsigned int) (frame_buffer_addr[0]
				+ vd_mode.height * vd_mode.width * 3 * rd_index);
		//����ǰ֡��ͼ�񿽱���ץ��ͼƬ��������
		memcpy((void *) bmp_addr, (void *) rd_fram_addr,
				vd_mode.height * vd_mode.width * 3);
		//������ͨ��פͣ���̣������ڶ�֮֡������л�
		XAxiVdma_StopParking(&vdma[0], XAXIVDMA_READ);
		//��ץ��ͼƬ���������е�ͼ���� BMP ��ʽд�� SD ��
		write_sd_bmp((u8 *) bmp_addr);
		//BMP ͼƬ����ۼ�
		pic_cnt++;
		xil_printf("take picture success and the pic_cnt is %d",pic_cnt);
		}
    return 0;
}
