/*******************************************************************************
 *   Filename:       SetPara.c
 *   Revised:        All copyrights reserved to redmorningcn.
 *   Revision:       v1.0
 *   Writer:	     redmorningcn.
 *
 *   Description:    
					 1、设置统计模块运算参数。

/////////////////////////////////////////////////////////////////////////////
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

/********************************************************************************************/
/* Include files																			*/
/********************************************************************************************/
#include <utility.h>
#include <rs232.h>
#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include "setpara.h"
#include "senddata.h" 
#include "csnr_package_deal.h" 
#include "cvi_com_operate.h" 
#include "databasesample.h" 
#include "setmodel.h"


/********************************************************************************************/
/* Constants																				*/
/********************************************************************************************/


/********************************************************************************************/
/* Globals																					*/
/********************************************************************************************/
int				l_eqiupmentcode = 0;		//装置命令码，操作面板控制
stcProductInfo  gstrProductInfo;		//产品参数
stcOilPara		gsOilPara;				//油量参数

/********************************************************************************************/
/* local Prototypes																			*/
/********************************************************************************************/


/********************************************************************************************/
//读系统时间，转换为stcTime结构
/********************************************************************************************/
void     ReadTime(stcTime * sTime)
{
	struct tm *ptr;
    time_t lt; 
	
    lt=time(NULL);
    ptr=localtime((time_t*)&lt);				//读系统时间
	
	sTime->Sec 		= (unsigned char)ptr->tm_sec;
	sTime->Min 		= (unsigned char)ptr->tm_min;
	sTime->Hour 	= (unsigned char)ptr->tm_hour;	
	sTime->Date 	= (unsigned char)ptr->tm_mday;	  	
	sTime->Month 	= (unsigned char)ptr->tm_mon+1;
	sTime->Year 	= (unsigned char)ptr->tm_year-100;	
//	printf("%d-%d-%d %d:%d:%d\r\n",sTime->Year ,sTime->Month,sTime->Date,sTime->Hour,sTime->Min,sTime->Sec);
}

extern	int   g_com1systictimes;
/********************************************************************************************/
/* 串口设置参数      																		*/
//通过l_eqiupmentcode变量控制设置过程。
//此函数需要循环执行，且能外部设置 l_eqiupmentcode
/********************************************************************************************/
void Com_SetParaTask(void)		
{
	unsigned char  buf[256];
	unsigned int   datalen = 0;
	static   int   modelsendnum = 0;			//发送序号
	unsigned char  clearcode = 1;
	int  	i;
	uint8	*p;
	if(l_eqiupmentcode != 0)					//有设置指令
	{
		datalen = 0;
		
		memcpy(&buf[datalen],(unsigned char *)&l_eqiupmentcode,sizeof(l_eqiupmentcode));
		datalen += sizeof(l_eqiupmentcode);
		
		switch (l_eqiupmentcode)				//根据指令操作，祥见统计模块通讯协议
		{
			unsigned int 	startnum;
			unsigned int 	endnum;
			short  		density; 
			stcTime		sTime;   
			short   	hig; 
			char		modelnum;
			unsigned short 	locotype,loconum;
			unsigned char	printfflg,storetime,dishigflg,noavgflg;
			unsigned short	crc;
			unsigned int	calcpara;
			unsigned int 	recnum;  
			
			case DATA_CARD_DIS:  
							  break; 
			case DATA_CARD_FIN:  
							  break; 
			case DATA_CARD:  
							  break;    
			case DENSITY_CARD: 
							  density = gsOilPara.Density;			//取密度值

							  memcpy(&buf[datalen],(unsigned char *)&density,sizeof(density));
							  datalen += sizeof(density);
						
							  break;   
			case CALC_PARA: 
							  calcpara = gsOilPara.Slope;			//斜率  （/10000）

							  memcpy(&buf[datalen],(unsigned char *)&calcpara,sizeof(calcpara));
							  datalen += sizeof(calcpara);
							  
							  hig = gsOilPara.Hig;					//高度
							  memcpy(&buf[datalen],(unsigned char *)&hig,sizeof(hig));
							  datalen += sizeof(hig);
						
							  break;    							  
			case MODEL_CARD:  
							 
							  break;   
			case TIME_CARD:   
	 						  ReadTime((stcTime *)&sTime);											// ê±??   
							  memcpy(&buf[datalen],(unsigned char *)&sTime,sizeof(sTime));
							  datalen += sizeof(sTime);

							  break; 
			case HIGHT_CARD:  
							  hig = gsOilPara.Hig;				//高度
							  memcpy(&buf[datalen],(unsigned char *)&hig,sizeof(hig));
							  datalen += sizeof(hig);

							  break; 
			case COPY_CARD:  	
							  
						      break; 
			case MODEL_SELECT_CARD:
							  modelnum = gsOilPara.ModelNum;	//选模
							  
							  memcpy(&buf[datalen],(unsigned char *)&modelnum,sizeof(modelnum));
							  datalen += sizeof(modelnum);

							  break; 
			case FIX_CARD:  
							  locotype = gstrProductInfo.sLocoId.Type;	//车型
							  loconum  = gstrProductInfo.sLocoId.Num;	//编号

							  memcpy(&buf[datalen],(unsigned char *)&locotype,sizeof(locotype));
							  datalen += sizeof(locotype);
							 
							  memcpy(&buf[datalen],(unsigned char *)&loconum,sizeof(loconum));
							  datalen += sizeof(loconum);							  
							  
				     		  break; 
			case COPY_FRAM_CARD:  
							  break; 
							  
			case EXPAND_CARD:  	

							break; 
			case SYS_RUN_PARA:  	
							  break; 	
							  
			case CLEAR_ALL:  	

							  break;		
			case RUN_MODEL_PARA:						//设置单个运算用的油箱模型
					
					clearcode = 0;						//不清零装置标示，一般直接清零

					if(modelsendnum < 1 + (sizeof(gsCalcModel) / 128))
					{
						p = (uint8 *)&gsCalcModel;
						memcpy(&buf[datalen],(unsigned char *)&modelsendnum,sizeof(modelsendnum));		//复制序号
						datalen += sizeof(modelsendnum);

						if(sizeof(gsCalcModel) - 128*modelsendnum > 128 )
						{
							memcpy(&buf[datalen],(unsigned char *)(p + 128*modelsendnum),128);
							datalen += 128; 
						}else
						{
							memcpy(&buf[datalen],(unsigned char *)(p + 128*modelsendnum),
								   				 sizeof(gsCalcModel) - 128*modelsendnum );
							
							datalen += sizeof(gsCalcModel) - 128*modelsendnum ; 
						}
						modelsendnum++;
						
						i = g_com1systictimes;
						while(g_com1systictimes - i  < 1000);
					} 

					if(modelsendnum >= 1 + (sizeof(gsCalcModel) / 128)) //数据发送完成
					{
						modelsendnum = 0;
						clearcode = 1;				//清零标示，设置结束
					}
					break; 							  
							  
			default:
				break;
		}

		stcCsnrProtocolPara	sCsnrtmp;			//协议端口
		
		sCsnrtmp.databuf =  buf;				//要发送数据buf	
		sCsnrtmp.rxtxbuf =  txdatabuf;			//串口专用buf
		
		sCsnrtmp.datalen = (uint8)datalen;		//要发送数据长度
		sCsnrtmp.destaddr= 0x80;				//目标地址
		sCsnrtmp.sourceaddr	= 0xc2;				//原地址
		sCsnrtmp.framcode	= 0x00;				//原地址

		
		DataPackage_CSNC(&sCsnrtmp);			//按协议打包
	 
		SendCom1(sCsnrtmp.rxtxbuf,sCsnrtmp.rxtxlen);  //从串发送数据
		//DataSend_CSNR(0xc2,0x80,buf,datalen);          
		if( clearcode )
			l_eqiupmentcode = 0;					//操作完后，命令清零
	}
}
