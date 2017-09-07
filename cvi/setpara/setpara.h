/*******************************************************************************
 *   Filename:       SetPara.h
 *   Revised:        All copyrights reserved to redmorningcn.
 *   Revision:       v1.0
 *   Writer:	     redmorningcn.
 *
 *   Description:    ����ͳ��ģ������� 

					 
					 ˫��ѡ�� otr �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� Otr �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� OTR �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   �� app_cfg.h ��ָ��������� ���ȼ�  �� APP_TASK_COMM_PRIO     ��
 *                                            �� �����ջ�� APP_TASK_COMM_STK_SIZE ����С
 *
 *   Notes:
 *     				E-mail: redmorningcn@qq.com
 *
 *******************************************************************************/

#ifndef	SET_PARA_H
#define	SET_PARA_H


/********************************************************************************************/
/* Include files																			*/
/********************************************************************************************/
#include "asynctmr.h"
#include <utility.h>
#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include <formatio.h> 
#include "includes.h"


/********************************************************************************************/
/* Constants																					*/
/********************************************************************************************/
//uir�ļ���
#define		DATA_CARD			0x0001			//���ݿ�
#define		DENSITY_CARD		0x0020			//�ܶȿ�
#define		MODEL_CARD			0x0040			//����ģ�Ϳ�
#define		TIME_CARD			0x0100			//ʱ�俨
#define		HIGHT_CARD			0x0200			//�߶ȵ�����
#define		COPY_CARD			0x0400			//FLASH���ƿ�
#define		MODEL_SELECT_CARD	0x1000			//ģ��ѡ��
#define		FIX_CARD			0x2000			//װ����
#define		COPY_FRAM_CARD		0x4000			//���翽����

#define		SET_DATA_CARD		0x0080			//������
#define		DEBUG_CARD			0x0800			//���Ը��ƿ�
#define		DATA_CARD_DIS		0x10000			//���ݿ�
#define		DATA_CARD_FIN		0x20000			//���ݿ�

#define		EXPAND_CARD			0x8000			//��չ��
#define		SYS_RUN_PARA		0x40000	
#define		CLEAR_ALL			0x80000			//�������в���

#define		RST_SYS				0x100000		//ϵͳ��λ
#define		CALC_PARA           0x200000		//
#define		RUN_MODEL_PARA      0x400000		//����ģ�Ϳ�



//��������ô��ڿռ�����


/********************************************************************************************/
//�ṹ��																				    */
/********************************************************************************************/
 
//������Ϣ   ����+����
//4 bytes
typedef struct {     
 	uint16	Type;				//��������	2	�μ��������ʹ����
 	uint16	Num;				//������		2	
} stcLocoId;

//��Ʒ��Ϣ:  �ͺ�+ ���
//12 bytes
typedef struct _StrProductInfo {
	uint32			Type;			//��Ʒ���0 :NDP02  ,1:NDP02-B
	uint32			ID;				//��Ʒ���	16110002
	stcLocoId		sLocoId;		//������Ϣ	104(DF4B) - 1000
}stcProductInfo;


//�����������: ģ�ͱ�ţ��߶ȣ��ܶȣ�б��
//12 bytes
typedef struct _StrOilPara {
    uint8			ModelNum;		//ģ�ͱ��	0~64
    uint8			Rec8;			//����
    int16			Hig;			//�߶�  -500~500
    int16			Density;		//�ܶ�  800~900,     0.xxxx��/�������ף�10000����
    int16			Rec16;			//����
    uint32			Slope;			//����ϵ�� :����ֵ�ٳ�(slope/10000)  
}stcOilPara;

//ʱ��----------------------------------------
typedef		struct	_stcTime_
{
	unsigned char	Sec;
	unsigned char	Min;
	unsigned char	Hour;
	unsigned char	Date;
	unsigned char	Month;
	unsigned char	Year;
	unsigned short	CrcCheck;
}stcTime;


/********************************************************************************************/
/* Globals																					*/
/********************************************************************************************/
//���ڽṹ�弰ͨѶ
extern	int				l_eqiupmentcode;		//װ�������룬����������
extern	stcProductInfo  gstrProductInfo;		//��Ʒ����
extern	stcOilPara		gsOilPara;				//�����������


/********************************************************************************************/
/* Prototypes																				*/
/********************************************************************************************/

/********************************************************************************************/
/* �������ò���      																		*/
//ͨ��l_eqiupmentcode�����������ù��̡�
//�˺�����Ҫѭ��ִ�У������ⲿ���� l_eqiupmentcode
/********************************************************************************************/
void Com_SetParaTask(void);		



/*******************************************************************************
 *              end of file                                                    *
 *******************************************************************************/

#endif
