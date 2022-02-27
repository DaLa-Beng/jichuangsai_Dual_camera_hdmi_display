//****************************************Copyright (c)***********************************//
//ԭ�Ӹ����߽�ѧƽ̨��www.yuanzige.com
//����֧�֣�www.openedv.com
//�Ա����̣�http://openedv.taobao.com
//��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡZYNQ & FPGA & STM32 & LINUX���ϡ�
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2018-2028
//All rights reserved
//----------------------------------------------------------------------------------------
// File name:           main
// Last modified Date:  2019/10/8 17:25:36
// Last Version:        V1.0
// Descriptions:        ˫ĿOV5640 HDMI��ʾ
//----------------------------------------------------------------------------------------
// Created by:          ����ԭ��
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

//�궨��
#define FRAME_BUFFER_NUM   3                           //֡�������3
#define DYNCLK_BASEADDR    XPAR_AXI_DYNCLK_0_BASEADDR  //��̬ʱ�ӻ���ַ
#define VDMA_ID            XPAR_AXIVDMA_0_DEVICE_ID    //VDMA����ID
#define DISP_VTC_ID        XPAR_VTC_0_DEVICE_ID        //VTC����ID

//ȫ�ֱ���
//frame buffer����ʼ��ַ
unsigned int frame_buffer_addr[DISPLAY_VDMA_NUM];
unsigned int vdma_id[DISPLAY_VDMA_NUM] = {XPAR_AXIVDMA_0_DEVICE_ID,
		                                  XPAR_AXIVDMA_1_DEVICE_ID};

XAxiVdma     vdma[DISPLAY_VDMA_NUM];
DisplayCtrl  dispCtrl;
VideoMode    vd_mode;

int main(void)
{
	u32 status0,status1;
	u16 cmos_h_pixel;   //ov5640 DVP ���ˮƽ���ص���
	u16 cmos_v_pixel;   //ov5640 DVP �����ֱ���ص���
	u16 total_h_pixel;  //ov5640 ˮƽ�����ش�С
	u16 total_v_pixel;  //ov5640 ��ֱ�����ش�С

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

    return 0;
}