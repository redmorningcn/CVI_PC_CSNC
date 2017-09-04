/*******************************************************************************
 *   Filename:       recvdata.c
 *   Revised:        All copyrights reserved to redmorningcn.
 *   Revision:       v1.0
 *   Writer:	     redmorningcn.
 *
 *   Description:    
					 1���򿪺͹ر��ļ���
					 2�����ô��ļ���ʼ���´���
					 3����ȡ�ļ��Ѵ����ȡ�

�����������˴��ڻص��������첽ʱ�ӣ����̣߳��ļ������Ⱥ�����ʵ�ִ������ã����ڷ��ͽ��ա� ����ӿں�������
////////////////////////////////////////////////////////////////////////////
����ӿڣ��ⲿ���̵��ã���
1��comconfig.uir��������ҳ��ӿڣ�
2��OpenCom1Callback �򿪴��ڻص���������CVI�ص�������
3��ReadCom1			�Ӵ���1�ж�ȡ���ݣ�
4��SendCom1			�򴮿�1��д�����ݣ�
5��CloseCom1	    �رմ���1��
6��GetCom1RecEndFlg ȡ���ݽ�����ɱ�ʾ��
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


#define		MY_ADDR				0xca
//IAP ����֡������
#define     IAP_FRAME_CODE      10 

extern	stcFileInfo		gsBinFileInfo;


/********************************************************************************************/
/* Constants																				*/
/********************************************************************************************/
stcCsnrProtocolPara	gsRecCsnrProtocolPara;

unsigned char	Recvdatabuf[256];
extern	int	gPanelHandle;
extern	int	gBinFilePanelHandle;


stcIapCtrl	gsIapCtrl;
extern	int		gSendDataFlg;

typedef struct  _stcIAPStart_
{
    uint16  HardVer;        //Ӳ���汾
    uint16  SoftVer;        //����汾
    uint32  SoftSize;       //�����С
    uint32  Addr;           //��ǰ��ַ
    uint16  CurNum;         //��ǰ���
}stcIapStart;

 
stcIapStart gsIapStart;		//IAP����

/*******************************************************************************************
���ݽ������ݴ��� RecvDeal
*******************************************************************************************/
void	RecvDeal(char* buf,int len)
{
	uint16	tmp16;
	
	gsRecCsnrProtocolPara.rxtxbuf 	=  buf;
	gsRecCsnrProtocolPara.rxtxlen 	=  (char)len;
	gsRecCsnrProtocolPara.databuf	=  Recvdatabuf;
	
	DataUnpack_CSNC(&gsRecCsnrProtocolPara);	 //����
	
	if(gsRecCsnrProtocolPara.destaddr == MY_ADDR)
	{
		switch(gsRecCsnrProtocolPara.framcode)
		{
			case 0:
			case 1:
			case 2:
				break;
			case IAP_FRAME_CODE:		 //IAP Ӧ��
				memcpy((uint8 *)&tmp16,gsRecCsnrProtocolPara.databuf,sizeof(tmp16));	

				gsIapCtrl.endflg  	= 0;
				gsIapCtrl.resendflg	= 0;
				gsIapCtrl.echoflg   = 0;
				
				if(tmp16 == 1 || tmp16 == 2) 					//��������
				{
					if(tmp16 == 1)		//��ʼ���ͣ���������
					{
						gsIapCtrl.echoflg = 1;	
						
						memcpy((uint8 *)&gsIapStart,&gsRecCsnrProtocolPara.databuf[2],sizeof(gsIapStart));												//����Ӧ��	
						
						printf("\r\n��������յ������: %d",gsIapStart.CurNum);
						//�ϵ�����
						if(
									gsIapStart.CurNum   							//Ӧ��֡��Ų�Ϊ�㣬��ʼ�ϵ�����
								&&	gsIapStart.SoftSize == gsBinFileInfo.size		//�ļ���С��ͬ
						  )				
						{
							gsIapCtrl.sendnum  		= gsIapStart.CurNum;			//�������			
							gsIapCtrl.sendsize 		= gsIapStart.CurNum * 128;		//�ѷ�����
							gsBinFileInfo.deallen  	= gsIapStart.CurNum * 128;  	//�ļ��Ѵ�����
						}
					}
					else
					{
						memcpy((uint8 *)&gsIapCtrl.recvnum,&gsRecCsnrProtocolPara.databuf[2],sizeof(gsIapCtrl.recvnum));	   //ȡ�������
						if(gsIapCtrl.recvnum == gsIapCtrl.sendnum)  
						{
							gsIapCtrl.sendnum++;
							gsIapCtrl.echoflg 	= 1;			  //����������ȷ��Ӧ���ʾ��1
						}else{
							gsIapCtrl.resendflg	= 1;			  //�������ش����ط���ʾ��1
						}
					}
				} else if(tmp16 == 3 || tmp16 == 4)
				{
					 gsIapCtrl.endflg = 1;
					 gSendDataFlg = 0;						 //�˳�����
				}
				
				if(gsRecCsnrProtocolPara.databuf[1])
				printf("\r\nͨѶ�쳣�����룺%0x ",gsRecCsnrProtocolPara.databuf[1]);
				
				break;
			default:;
		}
	}
	return;
}

/*******************************************************************************************
���ݷ��ͣ� ȡSENDTEXTBOX�����ݣ���stringת��Ϊhex
*******************************************************************************************/


/*******************************************************************************************
���ݷ��ͣ� SendTextBox()
����SENDTEXTBOX������
*******************************************************************************************/

/*******************************************************************************************
���ݷ��ͣ� SendFileData()
����SendFileData������
*******************************************************************************************/

