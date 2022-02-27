//****************************************Copyright (c)***********************************//
//ԭ�Ӹ����߽�ѧƽ̨��www.yuanzige.com
//����֧�֣�www.openedv.com
//�Ա����̣�http://openedv.taobao.com
//��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡZYNQ & FPGA & STM32 & LINUX���ϡ�
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2018-2028
//All rights reserved
//----------------------------------------------------------------------------------------
// File name:           osd
// Last modified Date:  2019/10/8 17:25:36
// Last Version:        V1.0
// Descriptions:        OSD IP������
//----------------------------------------------------------------------------------------
// Created by:          ����ԭ��
// Created date:        2019/10/8 17:25:36
// Version:             V1.0
// Descriptions:        The original version
//
//----------------------------------------------------------------------------------------
//****************************************************************************************//

#include "xbasic_types.h"
#include "xparameters.h"
#include "xstatus.h"
#include "osd.h"
#include "stdio.h"

//�궨��
#define OSD_DEVICE_ID     XPAR_OSD_0_DEVICE_ID

//ȫ�ֱ���
XOSD Osd;                
XOSD_Config *OsdCfgPtr;     
OSD_LayerCfg OSD_LCfg;

//Color table definition
u32 color_arr[16] = {
    0x00000000, 0xa0a25f58, 0xa08080ff, 0xa0808010,
    0xa0ef5a51, 0x00000000, 0xa0465289, 0x00000000,
    0xa065ba6b, 0x00000000, 0xa09017c5, 0xa0a9c860,
    0xa0bc3198, 0xa010a5a9, 0xa0808080, 0xa0ada1ab
};

//Text table definition
char __attribute__ ((aligned (4))) TextData[8][32] = {
    "OV5640 1", //"String #1",
    "OV5640 2", //"String #2",
    "Hello",
    "Xilinx",
    "String #5",
    "String #6",
    "String #7",
    "String #8"
};

//Font definition
unsigned char __attribute__ ((aligned (4))) Font[128][8] = {
    {0x00, 0x36, 0x7F, 0x7F, 0x3E, 0x1C, 0x08, 0x00}, // NULL
    {0x18, 0x18, 0x18, 0x1F, 0x1F, 0x18, 0x18, 0x18},
    {0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03},
    {0x18, 0x18, 0x18, 0xF8, 0xF8, 0x00, 0x00, 0x00},
    {0x18, 0x18, 0x18, 0xF8, 0xF8, 0x18, 0x18, 0x18},
    {0x00, 0x00, 0x00, 0xF8, 0xF8, 0x18, 0x18, 0x18},
    {0x03, 0x07, 0x0E, 0x1C, 0x38, 0x70, 0xE0, 0xC0},
    {0xC0, 0xE0, 0x70, 0x38, 0x1C, 0x0E, 0x07, 0x03},
    {0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF},
    {0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x0F, 0x0F},
    {0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF},
    {0x0F, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00},
    {0xF0, 0xF0, 0xF0, 0xF0, 0x00, 0x00, 0x00, 0x00},
    {0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF},
    {0x00, 0x00, 0x00, 0x00, 0xF0, 0xF0, 0xF0, 0xF0},
    {0x00, 0x1C, 0x1C, 0x77, 0x77, 0x08, 0x1C, 0x00},
    {0x00, 0x00, 0x00, 0x1F, 0x1F, 0x18, 0x18, 0x18},
    {0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00},
    {0x18, 0x18, 0x18, 0xFF, 0xFF, 0x18, 0x18, 0x18},
    {0x00, 0x00, 0x3C, 0x7E, 0x7E, 0x7E, 0x3C, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF},
    {0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0},
    {0x00, 0x00, 0x00, 0xFF, 0xFF, 0x18, 0x18, 0x18},
    {0x18, 0x18, 0x18, 0xFF, 0xFF, 0x00, 0x00, 0x00},
    {0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0},
    {0x18, 0x18, 0x18, 0x1F, 0x1F, 0x00, 0x00, 0x00},
    {0x78, 0x60, 0x78, 0x60, 0x7E, 0x18, 0x1E, 0x00},
    {0x00, 0x18, 0x3C, 0x7E, 0x18, 0x18, 0x18, 0x00},
    {0x00, 0x18, 0x18, 0x18, 0x7E, 0x3C, 0x18, 0x00},
    {0x00, 0x18, 0x30, 0x7E, 0x30, 0x18, 0x00, 0x00},
    {0x00, 0x18, 0x0C, 0x7E, 0x0C, 0x18, 0x00, 0x00},

    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // Space
    {0x00, 0x18, 0x18, 0x18, 0x18, 0x00, 0x18, 0x00},
    {0x00, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x66, 0xFF, 0x66, 0x66, 0xFF, 0x66, 0x00},
    {0x18, 0x3E, 0x60, 0x3C, 0x06, 0x7C, 0x18, 0x00},
    {0x00, 0x66, 0x6C, 0x18, 0x30, 0x66, 0x46, 0x00},
    {0x1C, 0x36, 0x1C, 0x38, 0x6F, 0x66, 0x3B, 0x00},
    {0x00, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x0E, 0x1C, 0x18, 0x18, 0x1C, 0x0E, 0x00},
    {0x00, 0x70, 0x38, 0x18, 0x18, 0x38, 0x70, 0x00},
    {0x00, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x00, 0x00},
    {0x00, 0x18, 0x18, 0x7E, 0x18, 0x18, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x30},
    {0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00},
    {0x00, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x40, 0x00},
    {0x00, 0x3C, 0x66, 0x6E, 0x76, 0x66, 0x3C, 0x00},
    {0x00, 0x18, 0x38, 0x18, 0x18, 0x18, 0x7E, 0x00},
    {0x00, 0x3C, 0x66, 0x0C, 0x18, 0x30, 0x7E, 0x00},
    {0x00, 0x7E, 0x0C, 0x18, 0x0C, 0x66, 0x3C, 0x00},
    {0x00, 0x0C, 0x1C, 0x3C, 0x6C, 0x7E, 0x0C, 0x00},
    {0x00, 0x7E, 0x60, 0x7C, 0x06, 0x66, 0x3C, 0x00},
    {0x00, 0x3C, 0x60, 0x7C, 0x66, 0x66, 0x3C, 0x00},
    {0x00, 0x7E, 0x06, 0x0C, 0x18, 0x30, 0x30, 0x00},
    {0x00, 0x3C, 0x66, 0x3C, 0x66, 0x66, 0x3C, 0x00},
    {0x00, 0x3C, 0x66, 0x3E, 0x06, 0x0C, 0x38, 0x00},
    {0x00, 0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x00},
    {0x00, 0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x30},
    {0x06, 0x0C, 0x18, 0x30, 0x18, 0x0C, 0x06, 0x00},
    {0x00, 0x00, 0x7E, 0x00, 0x00, 0x7E, 0x00, 0x00},
    {0x60, 0x30, 0x18, 0x0C, 0x18, 0x30, 0x60, 0x00},
    {0x00, 0x3C, 0x66, 0x0C, 0x18, 0x00, 0x18, 0x00},

    {0x00, 0x3C, 0x66, 0x6E, 0x6E, 0x60, 0x3E, 0x00}, // @
    {0x00, 0x18, 0x3C, 0x66, 0x66, 0x7E, 0x66, 0x00}, // A
    {0x00, 0x7C, 0x66, 0x7C, 0x66, 0x66, 0x7C, 0x00},
    {0x00, 0x3C, 0x66, 0x60, 0x60, 0x66, 0x3C, 0x00},
    {0x00, 0x78, 0x6C, 0x66, 0x66, 0x6C, 0x78, 0x00},
    {0x00, 0x7E, 0x60, 0x7C, 0x60, 0x60, 0x7E, 0x00},
    {0x00, 0x7E, 0x60, 0x7C, 0x60, 0x60, 0x60, 0x00},
    {0x00, 0x3E, 0x60, 0x60, 0x6E, 0x66, 0x3E, 0x00},
    {0x00, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x00},
    {0x00, 0x7E, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00},
    {0x00, 0x06, 0x06, 0x06, 0x06, 0x66, 0x3C, 0x00},
    {0x00, 0x66, 0x6C, 0x78, 0x78, 0x6C, 0x66, 0x00},
    {0x00, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7E, 0x00},
    {0x00, 0x63, 0x77, 0x7F, 0x6B, 0x63, 0x63, 0x00},
    {0x00, 0x66, 0x76, 0x7E, 0x7E, 0x6E, 0x66, 0x00},
    {0x00, 0x3C, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00},
    {0x00, 0x7C, 0x66, 0x66, 0x7C, 0x60, 0x60, 0x00},
    {0x00, 0x3C, 0x66, 0x66, 0x66, 0x6C, 0x36, 0x00},
    {0x00, 0x7C, 0x66, 0x66, 0x7C, 0x6C, 0x66, 0x00},
    {0x00, 0x3C, 0x60, 0x3C, 0x06, 0x06, 0x3C, 0x00},
    {0x00, 0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00},
    {0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7E, 0x00},
    {0x00, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x00},
    {0x00, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63, 0x00},
    {0x00, 0x66, 0x66, 0x3C, 0x3C, 0x66, 0x66, 0x00},
    {0x00, 0x66, 0x66, 0x3C, 0x18, 0x18, 0x18, 0x00},
    {0x00, 0x7E, 0x0C, 0x18, 0x30, 0x60, 0x7E, 0x00},
    {0x00, 0x1E, 0x18, 0x18, 0x18, 0x18, 0x1E, 0x00},
    {0x00, 0x40, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x00},
    {0x00, 0x78, 0x18, 0x18, 0x18, 0x18, 0x78, 0x00},
    {0x00, 0x08, 0x1C, 0x36, 0x63, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00},

    {0x40, 0x40, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00}, // `
    {0x00, 0x00, 0x3C, 0x06, 0x3E, 0x66, 0x3E, 0x00}, // a
    {0x00, 0x60, 0x60, 0x7C, 0x66, 0x66, 0x7C, 0x00},
    {0x00, 0x00, 0x3C, 0x60, 0x60, 0x60, 0x3C, 0x00},
    {0x00, 0x06, 0x06, 0x3E, 0x66, 0x66, 0x3E, 0x00},
    {0x00, 0x00, 0x3C, 0x66, 0x7E, 0x60, 0x3C, 0x00},
    {0x00, 0x0E, 0x18, 0x3E, 0x18, 0x18, 0x18, 0x00},
    {0x00, 0x00, 0x3E, 0x66, 0x66, 0x3E, 0x06, 0x7C},
    {0x00, 0x60, 0x60, 0x7C, 0x66, 0x66, 0x66, 0x00},
    {0x00, 0x18, 0x00, 0x38, 0x18, 0x18, 0x3C, 0x00},
    {0x00, 0x06, 0x00, 0x06, 0x06, 0x06, 0x06, 0x3C},
    {0x00, 0x60, 0x60, 0x6C, 0x78, 0x6C, 0x66, 0x00},
    {0x00, 0x38, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00},
    {0x00, 0x00, 0x66, 0x7F, 0x7F, 0x6B, 0x63, 0x00},
    {0x00, 0x00, 0x7C, 0x66, 0x66, 0x66, 0x66, 0x00},
    {0x00, 0x00, 0x3C, 0x66, 0x66, 0x66, 0x3C, 0x00},
    {0x00, 0x00, 0x7C, 0x66, 0x66, 0x7C, 0x60, 0x60},
    {0x00, 0x00, 0x3E, 0x66, 0x66, 0x3E, 0x06, 0x06},
    {0x00, 0x00, 0x7C, 0x66, 0x60, 0x60, 0x60, 0x00},
    {0x00, 0x00, 0x3E, 0x60, 0x3C, 0x06, 0x7C, 0x00},
    {0x00, 0x18, 0x7E, 0x18, 0x18, 0x18, 0x0E, 0x00},
    {0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x3E, 0x00},
    {0x00, 0x00, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x00},
    {0x00, 0x00, 0x63, 0x6B, 0x7F, 0x3E, 0x36, 0x00},
    {0x00, 0x00, 0x66, 0x3C, 0x18, 0x3C, 0x66, 0x00},
    {0x00, 0x00, 0x66, 0x66, 0x66, 0x3E, 0x0C, 0x78},
    {0x00, 0x00, 0x7E, 0x0C, 0x18, 0x30, 0x7E, 0x00},

    {0x14, 0x10, 0x10, 0x40, 0x10, 0x10, 0x14, 0x00}, // {
    {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18},
    {0x50, 0x10, 0x10, 0x04, 0x10, 0x10, 0x50, 0x00}, // }
    {0x11, 0x11, 0x44, 0x44, 0x00, 0x00, 0x00, 0x00}, // ~
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 127 DEL
};

//OSD��ʼ��
void osd_init(u16 lcd_id,int screen_width,int screen_height)
{
	u8 text_size = 1;                          //�ַ��ı��Ŵ����,��Χ1��2��4��8
	OsdCfgPtr = XOSD_LookupConfig(OSD_DEVICE_ID);
	XOSD_CfgInitialize(&Osd, OsdCfgPtr, OsdCfgPtr->BaseAddress); //��ʼ��OSD IP��
	XOSD_RegUpdateEnable(&Osd);                //����OSD�Ĵ�������ʹ��
	XOSD_Enable(&Osd);                         //ʹ��OSD IP��
	osd_layer_cfg(screen_width,screen_height); //OSD����

    if(lcd_id == 0x4342)
    	text_size = 2;                         //4.3'��,�Ŵ�2��
    else
    	text_size = 4;                         //������,�Ŵ�4��
    //������XOSDʵ��,ͼ������,X������ʼλ��,Y������ʼλ��,��ɫ����,�ı�����,�ı���С
    //ͼ��2��ʾ�ı�����
	osd_draw_text(Osd,2,screen_width/10,10,11,0,text_size);
	//ͼ��3��ʾ�ı�����
	osd_draw_text(Osd,3,screen_width/2+screen_width/10,10,11,1,text_size);
}

//OSD����
void osd_layer_cfg(int screen_width,int screen_height)
{
	//����OSD����ֱ���
	XOSD_SetScreenSize(&Osd, screen_width, screen_height);
	xil_printf("OSD output resolution is %d,%d\r\n",screen_width,screen_height);
	//����OSD����ɫ     ��ɫ����:RED BLUE GREEN
	XOSD_SetBackgroundColor(&Osd,0x00,0xff,0x00);
	//����ͼ��0���ⲿ��Ƶ
	OSD_LCfg = OSD_Layer0;
	XOSD_SetLayerAlpha(&Osd,OSD_LCfg.index,OSD_LCfg.alpha_enable,OSD_LCfg.alpha_value);
	XOSD_SetLayerPriority(&Osd,OSD_LCfg.index,OSD_LCfg.priority);
	XOSD_SetLayerDimension(&Osd,OSD_LCfg.index,0,0,screen_width/2,screen_height);
	XOSD_EnableLayer(&Osd,OSD_LCfg.index);
	//����ͼ��1���ⲿ��Ƶ
	OSD_LCfg = OSD_Layer1;
	XOSD_SetLayerAlpha(&Osd,OSD_LCfg.index,OSD_LCfg.alpha_enable,OSD_LCfg.alpha_value);
	XOSD_SetLayerPriority(&Osd,OSD_LCfg.index,OSD_LCfg.priority);
	XOSD_SetLayerDimension(&Osd,OSD_LCfg.index,screen_width/2,
			               0,screen_width/2,screen_height);
	XOSD_EnableLayer(&Osd,OSD_LCfg.index);
	//����ͼ��2���ڲ��ַ���ʾ
	OSD_LCfg = OSD_Layer2;
	XOSD_SetLayerAlpha(&Osd,OSD_LCfg.index,OSD_LCfg.alpha_enable,OSD_LCfg.alpha_value);
	XOSD_SetLayerPriority(&Osd,OSD_LCfg.index,OSD_LCfg.priority);
	XOSD_SetLayerDimension(&Osd,OSD_LCfg.index,0,0,screen_width,screen_height);
	XOSD_EnableLayer(&Osd,OSD_LCfg.index);
	//��OSDͼ�������BANK 0�м�����ɫ��
	XOSD_LoadColorLUTBank(&Osd,OSD_LCfg.index, 0, color_arr);
	//��OSDͼ�������BANK 0�м����ַ�(����)
	XOSD_LoadCharacterSetBank(&Osd,OSD_LCfg.index, 0, (u32 *)Font);
	//��OSDͼ�������BANK 0�м����ı�
	XOSD_LoadTextBank(&Osd,OSD_LCfg.index, 0, (u32 *)TextData);
	//ѡ��OSDͼ�����������ЧBANK(BANK 0)
	XOSD_SetActiveBank(&Osd,OSD_LCfg.index, 0, 0, 0, 0);

	//����ͼ��3���ڲ��ַ���ʾ
	OSD_LCfg = OSD_Layer3;
	XOSD_SetLayerAlpha(&Osd,OSD_LCfg.index,OSD_LCfg.alpha_enable,OSD_LCfg.alpha_value);
	XOSD_SetLayerPriority(&Osd,OSD_LCfg.index,OSD_LCfg.priority);
	XOSD_SetLayerDimension(&Osd,OSD_LCfg.index,0,0,screen_width,screen_height);
	XOSD_EnableLayer(&Osd,OSD_LCfg.index);
	//��OSDͼ�������BANK 0�м�����ɫ��
	XOSD_LoadColorLUTBank(&Osd,OSD_LCfg.index, 0, color_arr);
	//��OSDͼ�������BANK 0�м����ַ�(����)
	XOSD_LoadCharacterSetBank(&Osd,OSD_LCfg.index, 0, (u32 *)Font);
	//��OSDͼ�������BANK 0�м����ı�
	XOSD_LoadTextBank(&Osd,OSD_LCfg.index, 0, (u32 *)TextData);
	//ѡ��OSDͼ�����������ЧBANK(BANK 0)
	XOSD_SetActiveBank(&Osd,OSD_LCfg.index, 0, 0, 0, 0);

	xil_printf("OSD config done!\r\n" );
}

//OSD��ʾ�ı�,������XOSDʵ��,ͼ������,X������ʼλ��,Y������ʼλ��,��ɫ����,�ı�����,�ı���С
void osd_draw_text(XOSD Osd,int layer_index,int x_start, int y_start,
		int color_index, int text_index, int text_size)
{
	u32 instruction[XOSD_INS_SIZE];
	u16 ins_opcode_type = XOSD_INS_OPCODE_TXT;
	u8  txt_size = (text_size<<4);            //��4λ��ʾ�ı��Ŵ�ı���

	//����OSDָ��
	XOSD_CreateInstruction(&Osd, instruction, layer_index,ins_opcode_type,txt_size,
			x_start, y_start, x_start, y_start,text_index, color_index);
	//����ָ��
	XOSD_LoadInstructionList(&Osd, layer_index, 0, instruction, 1);
	xil_printf("OSD Layer %d draw text done!\r\n",layer_index);
}