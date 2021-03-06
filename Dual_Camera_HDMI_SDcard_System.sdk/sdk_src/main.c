//****************************************Copyright (c)***********************************//
//原子哥在线教学平台：www.yuanzige.com
//技术支持：www.openedv.com
//淘宝店铺：http://openedv.taobao.com
//关注微信公众平台微信号："正点原子"，免费获取ZYNQ & FPGA & STM32 & LINUX资料。
//版权所有，盗版必究。
//Copyright(C) 正点原子 2018-2028
//All rights reserved
//----------------------------------------------------------------------------------------
// File name:           main
// Last modified Date:  2019/10/8 17:25:36
// Last Version:        V1.0
// Descriptions:        双目OV5640 HDMI显示
//----------------------------------------------------------------------------------------
// Created by:          正点原子
// Created date:        2019/10/8 17:25:36
// Version:             V1.0
// Descriptions:        The original version
//
//----------------------------------------------------------------------------------------
//****************************************************************************************//

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

//宏定义
#define FRAME_BUFFER_NUM   3                           //帧缓存个数3
#define DYNCLK_BASEADDR    XPAR_AXI_DYNCLK_0_BASEADDR  //动态时钟基地址
#define VDMA_ID            XPAR_AXIVDMA_0_DEVICE_ID    //VDMA器件ID
#define DISP_VTC_ID        XPAR_VTC_0_DEVICE_ID        //VTC器件ID

//全局变量
//frame buffer的起始地址
unsigned int frame_buffer_addr[DISPLAY_VDMA_NUM];
unsigned int vdma_id[DISPLAY_VDMA_NUM] = {XPAR_AXIVDMA_0_DEVICE_ID,
		                                  XPAR_AXIVDMA_1_DEVICE_ID};

XAxiVdma     vdma[DISPLAY_VDMA_NUM];
DisplayCtrl  dispCtrl;
VideoMode    vd_mode;

int main(void)
{
	u32 status0,status1;
	u16 cmos_h_pixel;   //ov5640 DVP 输出水平像素点数
	u16 cmos_v_pixel;   //ov5640 DVP 输出垂直像素点数
	u16 total_h_pixel;  //ov5640 水平总像素大小
	u16 total_v_pixel;  //ov5640 垂直总像素大小

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

    return 0;
}
