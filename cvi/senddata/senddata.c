/*******************************************************************************
 *   Filename:       senddata.c
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


/********************************************************************************************/
/* Constants																				*/
/********************************************************************************************/
stcCsnrProtocolPara	gsCsnrProtocolPara;

unsigned char	senddatabuf[256];
unsigned char 	txdatabuf[256];

extern	int	gPanelHandle;
extern	int	gBinFilePanelHandle;
void printftext(int	panel,int id,const char *format, ...) ;

/*******************************************************************************************
���ݷ��ͣ� SendCsnrDataDeal
�����ݰ���Э���ʽ���͡�
��ʽ��ֵ  	ϵͳ����ʱ�Զ����ġ�
*******************************************************************************************/
void	SendCsnrDataDeal(char *buf,int len)
{
	 int i;
	 
	 gsCsnrProtocolPara.databuf =  senddatabuf;		//����ָ�븳ֵ	
	 gsCsnrProtocolPara.rxtxbuf =  txdatabuf;
	 
	 memcpy(gsCsnrProtocolPara.databuf,buf,len);	//���ݷ�ֵ
	 gsCsnrProtocolPara.datalen = (unsigned char)len;
	 
	 DataPackage_CSNC(&gsCsnrProtocolPara);			//��Э����
	 
	 SendCom1(gsCsnrProtocolPara.rxtxbuf,gsCsnrProtocolPara.rxtxlen);  //�Ӵ���������
	 
	for (i = 0 ;i < gsCsnrProtocolPara.rxtxlen; i++)
	{
		printftext(gBinFilePanelHandle,IAP_PANEL_INFOTEXTBOX,"%02X",gsCsnrProtocolPara.rxtxbuf[i]);	 
	}
	printftext(gBinFilePanelHandle,IAP_PANEL_INFOTEXTBOX,"\r\n");	 

} 	


/*******************************************************************************************
���ݷ��ͣ� ȡSENDTEXTBOX�����ݣ���stringת��Ϊhex
*******************************************************************************************/
int	GetSendTextBoxData(uint8 buf[])
{
	uint8	string[512];
	uint32	len,i,j;
	uint8	charbuf[4]="0";
	
	GetCtrlVal(gPanelHandle,PANEL_COM1SENDTEXTBOX,string);  	//ȡ�ؼ��е�����
	
	len = 0;
	
	for(i = 0,j = 0;i<strlen(string);i++)							//����ת��	
	{
		if(string[i] == ' ')								//ȥ�ո�
		{
			continue;
		}else{
		
			//�жϷǷ��ַ�
			charbuf[j%2] = string[i];
			if(j %2 == 1)
			{
			    charbuf[2] 	= 	'\0';
				buf[len++]  = 	(unsigned char)strtol(charbuf,NULL,16);	
			}
			j++;
		}
	}
	
	return	len;
}

/*******************************************************************************************
���ݷ��ͣ� SendTextBox()
����SENDTEXTBOX������
*******************************************************************************************/
void	SendTextBox(void)
{
	char	buf[256]; 
	int		len;
	
	len = GetSendTextBoxData(buf);		//ȡ������ֵ
	
	SendCsnrDataDeal(buf,len);
}


extern	stcFileInfo		gsBinFileInfo;
extern	char	SetFileDealPer(void);
extern	stcIapCtrl	gsIapCtrl;

void InitIPACtrl(void)
{
	gsIapCtrl.echoflg 	= 0;
	gsIapCtrl.endflg 	= 0;
	gsIapCtrl.filesize 	= gsBinFileInfo.size;
	gsIapCtrl.recvnum	= 0;
	gsIapCtrl.resendflg = 0;
	gsIapCtrl.sendnum	= 0;
	gsIapCtrl.sendsize  = 0;
	gsIapCtrl.startflg 	= 0;
}

typedef  struct   _stcIAPPara_
{
    short  	hardver;        //Ӳ���汾
    short  	softver;        //����汾
    int  	softsize;       //�����С
    int  	addr;           //��ǰ��ַ
    int  	framenum;       //֡���
    short  	code;           //ָ���� 01����ʾ����ȷ�Ŀ���
    short  	crc16;
}stcIAPPara;

stcIAPPara	gsIapPara =
{
	0x10,
	0x20,
	0,
	0,
	0,
	0
};
void printftext(int	panel,int id,const char *format, ...);
extern int   g_systime; 

/*******************************************************************************************
���ݷ��ͣ� SendFileData()
����SendFileData������
*******************************************************************************************/
void	SendFileData(void)
{
	static	char	buf[256]; 
	static	int		len = 0;
	static	int 	resendtimes = 0;
	int		addr;
	int		per;
	short	tmp16;
	
	static	int	sendtime;
	//addr = 0;
	//
	if(			gsIapCtrl.startflg
			|| 	gsIapCtrl.echoflg
			|| 	gsIapCtrl.resendflg  
	  )	
	{//
		if(gsIapCtrl.startflg)						//��ʼ���أ����Ϳ�ʼ����ָ��
		{
			InitIPACtrl();							//��ʼ�����Ʊ���
			
			gsIapPara.softsize 		= gsBinFileInfo.size;
			gsBinFileInfo.deallen 	= 0;
			
			len = 0;

			tmp16 = 0x01;							//��ʼָ��
			memcpy(&buf[len],(char *)&tmp16,sizeof(tmp16));
			len += sizeof(tmp16);
			
			memcpy(&buf[len],(char *)&gsIapPara,sizeof(gsIapPara));
			len +=sizeof(gsIapPara);
			
			printf("\r\n��������������͵����: %d",gsIapPara.framenum);

			SendCsnrDataDeal(buf,len);											//��������
			
		}else if(gsIapCtrl.echoflg)												//�յ���ȷ��Ӧ�𣬼������ݷ���
		{
			gsIapCtrl.echoflg = 0;
			len = 0;

			resendtimes = 10;		//�ط�����
			sendtime =  g_systime;
				
			printftext(gBinFilePanelHandle,IAP_PANEL_INFOTEXTBOX,"�������%d\r\n",gsIapCtrl.sendnum);	

			if(gsBinFileInfo.deallen <gsBinFileInfo.size)						//�ѳ����ļ�����С���ļ�����
			{
				tmp16 = 0x02;
				memcpy(&buf[len],(char *)&tmp16,sizeof(tmp16));   				//ָ��
				len += sizeof(tmp16);
				
				memcpy(&buf[len],(char *)&gsIapCtrl.sendnum,sizeof(tmp16));   	//��ţ��ڽ������ۼ�
				len += sizeof(gsIapCtrl.sendnum);
					
				int readlen = ReadFileData(gsBinFileInfo.deallen,&buf[len],128);//���ļ��ж�ȡ����
		 		len +=  readlen;
				
				per = SetFileDealPer();
				SetCtrlVal (gBinFilePanelHandle, IAP_PANEL_PRE, per);			//���ðٷֱ�(���ؽ���) 
				
				int i =10000;
				while(i--);
				SendCsnrDataDeal(buf,len);										//��������
				
				gsBinFileInfo.deallen += readlen;								//�Ѵ������� �����ۼ�len
				
				if(gsBinFileInfo.deallen >= gsBinFileInfo.size)					//���ݷ������
				{
					i = 0;
				}
				
				gsIapPara.framenum 		= gsIapCtrl.sendnum;					//��¼�ѷ���� 

			}
			else	//���ݷ�����ɣ����ͽ�����ʾ
			{
				tmp16 = 0x03;													//���ͽ���ָ��
				memcpy(&buf[len],(char *)&tmp16,sizeof(tmp16));   				//ָ��
				len += sizeof(tmp16); 
				resendtimes = 0;		//�ط�����

				memcpy(&buf[len],(char *)&gsIapPara,sizeof(gsIapPara));
				len +=sizeof(gsIapPara);				
				gsIapPara.framenum  = 0;									
				
				SendCsnrDataDeal(buf,len);										//��������
				
				printftext(gBinFilePanelHandle,IAP_PANEL_INFOTEXTBOX,"�������%d�����ݷ������\r\n",gsIapCtrl.sendnum);	
			}
		}
		else if(gsIapCtrl.resendflg)											//δ�յ���ȷָ��ط�
		{
			 gsIapCtrl.resendflg = 0;
			 SendCsnrDataDeal(buf,len);											//���������ϴε�����
		}
	}
	
	//�ط�����
	if(  	g_systime - sendtime > 100	//1s
		&&  resendtimes					//�ط���������1
		)
	{
		sendtime =  g_systime;
		resendtimes--;
		
		printftext(gBinFilePanelHandle,IAP_PANEL_INFOTEXTBOX,"�������%d\r\n",gsIapCtrl.sendnum);	

		SendCsnrDataDeal(buf,len);											//���������ϴε�����       
	}     
																																											 
	return;
}
