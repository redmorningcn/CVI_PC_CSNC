/*******************************************************************************
 *   Filename:       SetPara.h
 *   Revised:        All copyrights reserved to redmorningcn.
 *   Revision:       v1.0
 *   Writer:	     redmorningcn.
 *
 *   Description:    设置统计模块参数。 

					 
					 双击选中 otr 按 Ctrl + H, 钩选 Match the case, Replace with
 *                   输入您要的名字，点击 Replace All
 *                   双击选中 Otr 按 Ctrl + H, 钩选 Match the case, Replace with
 *                   输入您要的名字，点击 Replace All
 *                   双击选中 OTR 按 Ctrl + H, 钩选 Match the case, Replace with
 *                   输入您要的名字，点击 Replace All
 *                   在 app_cfg.h 中指定本任务的 优先级  （ APP_TASK_COMM_PRIO     ）
 *                                            和 任务堆栈（ APP_TASK_COMM_STK_SIZE ）大小
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
//uir文件名
#define		DATA_CARD			0x0001			//数据卡
#define		DENSITY_CARD		0x0020			//密度卡
#define		MODEL_CARD			0x0040			//参数模型卡
#define		TIME_CARD			0x0100			//时间卡
#define		HIGHT_CARD			0x0200			//高度调整卡
#define		COPY_CARD			0x0400			//FLASH复制卡
#define		MODEL_SELECT_CARD	0x1000			//模型选择卡
#define		FIX_CARD			0x2000			//装车卡
#define		COPY_FRAM_CARD		0x4000			//铁电拷贝卡

#define		SET_DATA_CARD		0x0080			//置数卡
#define		DEBUG_CARD			0x0800			//调试复制卡
#define		DATA_CARD_DIS		0x10000			//数据卡
#define		DATA_CARD_FIN		0x20000			//数据卡

#define		EXPAND_CARD			0x8000			//扩展卡
#define		SYS_RUN_PARA		0x40000	
#define		CLEAR_ALL			0x80000			//设置运行参数

#define		RST_SYS				0x100000		//系统复位
#define		CALC_PARA           0x200000		//
#define		RUN_MODEL_PARA      0x400000		//运算模型卡



//定义打开配置串口空间名称


/********************************************************************************************/
//结构体																				    */
/********************************************************************************************/
 
//机车信息   车型+车号
//4 bytes
typedef struct {     
 	uint16	Type;				//机车类型	2	参见机车类型代码表
 	uint16	Num;				//机车号		2	
} stcLocoId;

//产品信息:  型号+ 编号
//12 bytes
typedef struct _StrProductInfo {
	uint32			Type;			//产品类别，0 :NDP02  ,1:NDP02-B
	uint32			ID;				//产品编号	16110002
	stcLocoId		sLocoId;		//机车信息	104(DF4B) - 1000
}stcProductInfo;


//油量计算参数: 模型编号，高度，密度，斜率
//12 bytes
typedef struct _StrOilPara {
    uint8			ModelNum;		//模型编号	0~64
    uint8			Rec8;			//备用
    int16			Hig;			//高度  -500~500
    int16			Density;		//密度  800~900,     0.xxxx克/立方厘米（10000倍）
    int16			Rec16;			//备用
    uint32			Slope;			//修正系数 :计算值再乘(slope/10000)  
}stcOilPara;

//时间----------------------------------------
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
//串口结构体及通讯
extern	int				l_eqiupmentcode;		//装置命令码，操作面板控制
extern	stcProductInfo  gstrProductInfo;		//产品参数
extern	stcOilPara		gsOilPara;				//油量计算参数


/********************************************************************************************/
/* Prototypes																				*/
/********************************************************************************************/

/********************************************************************************************/
/* 串口设置参数      																		*/
//通过l_eqiupmentcode变量控制设置过程。
//此函数需要循环执行，且能外部设置 l_eqiupmentcode
/********************************************************************************************/
void Com_SetParaTask(void);		



/*******************************************************************************
 *              end of file                                                    *
 *******************************************************************************/

#endif
