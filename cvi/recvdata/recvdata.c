/*******************************************************************************
 *   Filename:       recvdata.c
 *   Revised:        All copyrights reserved to redmorningcn.
 *   Revision:       v1.0
 *   Writer:	     redmorningcn.
 *
 *   Description:    
					 1、打开和关闭文件。
					 2、设置从文件开始重新处理。
					 3、读取文件已处理长度。

本函数采用了串口回调函数，异步时钟，多线程，文件操作等函数。实现串口配置，串口发送接收。 对外接口函数如下
////////////////////////////////////////////////////////////////////////////
对外接口（外部工程调用）：
1、comconfig.uir参数设置页面接口；
2、OpenCom1Callback 打开串口回调函数；（CVI回调函数）
3、ReadCom1			从串口1中读取数据；
4、SendCom1			向串口1中写入数据；
5、CloseCom1	    关闭串口1；
6、GetCom1RecEndFlg 取数据接收完成标示；
 *   Notes:
 *     				E-mail: redmorningcn@qq.com
 *
 *******************************************************************************/


/********************************************************************************************/
/* Include files																			*/
/********************************************************************************************/
#include <rs232.h>
#include <utility.h>
#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include <formatio.h>
#include "cvi_file.h"
#include "IAP.h"
#include "panel1.h"
#include "csnr_package_deal.h"
#include "cvi_com_operate.h"
#include "includes.h"
#include "recvdata.h"
#include "oilrecord.h"
#include "senddata.h"



#define		MY_ADDR				0xca
//IAP 下载帧控制字
#define     IAP_FRAME_CODE      10 

extern	stcFileInfo		gsBinFileInfo;

/********************************************************************************************/
/* Constants																				*/
/********************************************************************************************/
stcCsnrProtocolPara	gsRecCsnrProtocolPara;

unsigned char	Recvdatabuf[256];
extern	int		gPanelHandle;
extern	int		gBinFilePanelHandle;


stcIapCtrl		gsIapCtrl;
extern	int		gSendDataFlg;

typedef struct  _stcIAPStart_
{
    uint16  HardVer;        //硬件版本
    uint16  SoftVer;        //软件版本
    uint32  SoftSize;       //软件大小
    uint32  Addr;           //当前地址
    uint16  CurNum;         //当前序号
}stcIapStart;

 
stcIapStart gsIapStart;		//IAP下载


/*******************************************************************************************

*******************************************************************************************/
void	RecvOilRecord(char* buf,int len)
{
	static	int	times = 0;
	if(	gsRecCsnrProtocolPara.sourceaddr == 0x80   ||
		gsRecCsnrProtocolPara.datalen 	 == 128    ||
		gsRecvOilRecordCtrl.enableflg
	  )										//
	{
		memcpy((uint8 *)&gsFlshRec,gsRecCsnrProtocolPara.databuf,sizeof(gsFlshRec));	//get-data
		
		if(gsRecCsnrProtocolPara.framnum != gsRecvOilRecordCtrl.recnum)
		{
			gsRecvOilRecordCtrl.recnum = gsRecCsnrProtocolPara.framnum;
			gsRecvOilRecordCtrl.storeflg =1;
		
			SendRecordRecEcho();			//echo
			
			times = 0;
		}
		
		if(times  > 10)					//次数超过10次，退出。
		{
			SendRecordRecEcho();			//echo
		}
		times++;
	}
}

/*******************************************************************************************
数据接收数据处理： RecvDeal
*******************************************************************************************/
void	RecvDeal(char* buf,int len)
{
	uint16	tmp16;
	
	gsRecCsnrProtocolPara.rxtxbuf 	=  buf;
	gsRecCsnrProtocolPara.rxtxlen 	=  (char)len;
	gsRecCsnrProtocolPara.databuf	=  Recvdatabuf;
	
	DataUnpack_CSNC(&gsRecCsnrProtocolPara);	 //配置
	
	if(gsRecCsnrProtocolPara.destaddr == MY_ADDR)
	{
		switch(gsRecCsnrProtocolPara.framcode)
		{
			case 0:						//V 1.0    c
				RecvOilRecord(gsRecCsnrProtocolPara.databuf,gsRecCsnrProtocolPara.datalen);
				break;
			case 1:
			case 2:
				break;
			case IAP_FRAME_CODE:		 //IAP 应答
				memcpy((uint8 *)&tmp16,gsRecCsnrProtocolPara.databuf,sizeof(tmp16));	

				gsIapCtrl.endflg  	= 0;
				gsIapCtrl.resendflg	= 0;
				gsIapCtrl.echoflg   = 0;
				
				if(tmp16 == 1 || tmp16 == 2) 					//继续下载
				{
					if(tmp16 == 1)								//开始发送，启动发送
					{
						gsIapCtrl.echoflg = 1;	
						
						memcpy((uint8 *)&gsIapStart,&gsRecCsnrProtocolPara.databuf[2],sizeof(gsIapStart));	//接收应答	
						
						printf("\r\n启动命令，收到的序号: %d",gsIapStart.CurNum);
																					//断点续传
						if(
									gsIapStart.CurNum   							//应答帧序号不为零，开始断点续传
								&&	gsIapStart.SoftSize == gsBinFileInfo.size		//文件大小相同
						  )				
						{
							gsIapCtrl.sendnum  		= gsIapStart.CurNum;			//发送序号			
							gsIapCtrl.sendsize 		= gsIapStart.CurNum * 128;		//已发长度
							gsBinFileInfo.deallen  	= gsIapStart.CurNum * 128;  	//文件已处理长度
						}
					}
					else
					{
						memcpy((uint8 *)&gsIapCtrl.recvnum,&gsRecCsnrProtocolPara.databuf[2],sizeof(gsIapCtrl.recvnum));	   //取接收序号
						if(gsIapCtrl.recvnum == gsIapCtrl.sendnum)  
						{
							gsIapCtrl.sendnum++;
							gsIapCtrl.echoflg 	= 1;			  //数据下载正确，应答标示置1
						}else{
							gsIapCtrl.resendflg	= 1;			  //数据下载错误，重发标示置1
						}
					}
				} else if(tmp16 == 3 || tmp16 == 4)
				{
					 gsIapCtrl.endflg = 1;
					 gSendDataFlg = 0;						 	//退出发送
				}
				
				if(gsRecCsnrProtocolPara.databuf[1])
				printf("\r\n通讯异常，代码：%0x ",gsRecCsnrProtocolPara.databuf[1]);
				
				break;
			default:;
		}
	}
	return;
}

/*******************************************************************************************
数据发送： 取SENDTEXTBOX的内容，将string转换为hex
*******************************************************************************************/


/*******************************************************************************************
数据发送： SendTextBox()
发送SENDTEXTBOX的内容
*******************************************************************************************/

/*******************************************************************************************
数据发送： SendFileData()
发送SendFileData的内容
*******************************************************************************************/

