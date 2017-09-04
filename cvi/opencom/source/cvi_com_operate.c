/*******************************************************************************
 *   Filename:       cvi_com1con.c
 *   Revised:        All copyrights reserved to redmorningcn.
 *   Revision:       v1.0
 *   Writer:	     redmorningcn.
 *
 *   Description:    
					 1���򿪺͹رմ��ڡ�
					 2���������ݷ��ͣ��������ݶ�ȡ��
					 3��ͨ�����ڻص��������첽��ʱ�����ƴ����շ�����

�����������˴��ڻص��������첽ʱ�ӣ����̣߳��ļ������Ⱥ�����ʵ�ִ������ã����ڷ��ͽ��ա� ����ӿں�������
////////////////////////////////////////////////////////////////////////////
����ӿڣ��ⲿ���̵��ã���
1��comconfig.uir��������ҳ��ӿڣ�
2��OpenCom1Callback �򿪴��ڻص���������CVI�ص�������
3��ReadCom1			�Ӵ���1�ж�ȡ���ݣ�
4��SendCom1			�򴮿�1��д�����ݣ�
5��CloseCom1	    �رմ���1��
6��GetCom1RecEndFlg ȡ���ݽ�����ɱ�ʾ��


/////////////////////////////////////////////////////////////////////////////
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

/********************************************************************************************/
/* Include files																			*/
/********************************************************************************************/
#include "asynctmr.h"
#include <utility.h>
#include <rs232.h>
#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include "cvi_com_operate.h"
#include <formatio.h> 

/********************************************************************************************/
/* Constants																					*/
/********************************************************************************************/
//uir�ļ���
#define		UIR_COMCONFIG_NAME		"comconfig.uir"

//���������ļ���																																									                             
#define		MAX_PATHNAME_LEN		512
#define		COM_CONFIG_FILE_NAME	"commconfig.ini"

#define		Enabled					1

/********************************************************************************************/
/* Globals																					*/
/********************************************************************************************/
//������
static int 		gCom1PanelHandle;									//����1���

//���ڽṹ�弰ͨѶ
extern	stcUartConfig   gsCom1Config;								//����1����
extern 	stcUartContrl	gsCom1Contrl;								//����1ͨѶ

int				g_com1systictimes = 0;
//��ʱ������
int				g_com1timerId;										//�첽��ʱ�����

/********************************************************************************************/
/* local Prototypes																				*/
/********************************************************************************************/
int 	GetMainChildPanel(void);										//�����

void 	CVICALLBACK Com1RecvAndSend (int portNo,int eventMask,void *callbackData);  //�����շ��ص����� 

void 	WriteCom1ConfigToFile(stcUartConfig * sUartConfig);				//д�������ݵ��ļ�    
void 	InitCom1Contrl(void);											//��ʼ��COM1�Ŀ��ƽṹ��     
void	SendByteCom(int port,char temp);
void	SetCviCtrlCom1Config(stcUartConfig * sUartConfig); 


/********************************************************************************************/
/* CloseCom1 �ش���1																		*/
/********************************************************************************************/
void CloseCom1(void)
{
	if( gsCom1Config.open )
	{
		DiscardAsyncTimer (g_com1timerId);							//�ر��첽��ʱ��
			/*	Close the open COM port before exiting */
		CloseCom (gsCom1Config.port);
	}
}
	

/********************************************************************************************/
/* ��ȡ���ձ�ʾ																		    	*/
/********************************************************************************************/
char	GetCom1RecEndFlg(void)
{
	return	(char)gsCom1Contrl.rdend;
}

/********************************************************************************************/
/* ��ȡ���ձ�ʾ																		    	*/
/********************************************************************************************/
void	ClearCom1RecEndFlg(void)
{
	gsCom1Contrl.rdend = 0;
}

/********************************************************************************************/
/* �Ӵ���1ȡ����  																	        */
/********************************************************************************************/
int	ReadCom1(char *buf,int len)
{
	int	temp;
	
	if(len < 1)
		return 0;
	
	temp = len < gsCom1Contrl.rdlen?len:gsCom1Contrl.rdlen; 

	memcpy(buf,gsCom1Contrl.rdbuf,temp);						//�������ݵ�ָ����������
	
	gsCom1Contrl.rdlen = 0;										//����ƿ������Ϣ
	gsCom1Contrl.p_rd  = gsCom1Contrl.rdbuf;
	gsCom1Contrl.rdend = 0;
	
	return	temp;												//���ض�ȡ����Ϣ
}

/********************************************************************************************/
/* �Ӵ���1��������																	        */
/********************************************************************************************/
int	SendCom1(char *buf,int len)
{

	if( len < 1 )
		return 0;

	//���ӽ���״̬�жϡ�������ڽ��գ���ȴ�10ms�ٷ��͡�
	int	times = g_com1systictimes;								//ȡ���ڶ�ʱ��ʱ�䡣
	while(gsCom1Contrl.timeout <= 1 )
	{
		 if((g_com1systictimes > times + 100) || g_com1systictimes < times)	//������ʱ��50������װ�û��ǽ���״̬���˳����ݷ���
			 return 0;
	}
	
	//���ӷ����жϣ����������գ���ȴ�10ms�ٷ��͡�
	times = g_com1systictimes;								//ȡ���ڶ�ʱ��ʱ�䡣
	while( gsCom1Contrl.wrlen  )							//���������գ��ȴ�
	{
		 if((g_com1systictimes > times + 100) || g_com1systictimes < times)	//������ʱ��50������װ�û��ǽ���״̬���˳����ݷ���
			 return 0;
	}
	
	//////////////////////׼�����ݷ���
	int	temp;
	temp = len;	
	
	if( len > sizeof(gsCom1Contrl.wrbuf))
		temp = sizeof(gsCom1Contrl.wrbuf);
	
	gsCom1Contrl.p_wr  = gsCom1Contrl.wrbuf;
	memcpy(gsCom1Contrl.wrbuf,buf,temp);	
	gsCom1Contrl.wrlen = temp;									//����ƿ������Ϣ

	Com1RecvAndSend(gsCom1Config.port,LWRS_TXEMPTY,0);

	return	temp;												//���ض�ȡ����Ϣ
}


/*---------------------------------------------------------------------------*/
/* This is the Async. timer callback routine.                                */
/*                                                                           */     
/*  !!! Remember that this callback may be run in a different thread !!!   	 */
/*                                                                           */
/* 1) Multithreading means that your global variables are not protected from */ 
/*    being written to by the main thread and this callback at the same time.*/
/*	  You will have to protect them by making them thread-safe.				 */
/*                                                                           */ 
/* 2) You should only call multithread-safe libraries in this routine.       */
/*    Generally, you will limit yourself to hardware-type functionality only.*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
int CVICALLBACK com1TimerCallback (	int reserved, int theTimerId, int event,
                                 	void *callbackData, int eventData1,
                                 	int eventData2)
{                                                   
	if(gsCom1Contrl.timeout < 0xff-1)
		gsCom1Contrl.timeout++;
	
	if(gsCom1Contrl.timeout >3 && gsCom1Contrl.rdlen)										//���ڽ��ս�����ʾ
	{
		gsCom1Contrl.rdend = 1;
	}
    
	g_com1systictimes++;
	return 0;
}

/********************************************************************************************/
/* Event_Char_Detect_Func ():  			*/
/* �շ��ص�����*/
/********************************************************************************************/
void CVICALLBACK Com1RecvAndSend (int portNo,int eventMask,void *callbackData)
{
	int	len;
	if(eventMask ==  LWRS_TXEMPTY )												//��������
	{
		if(gsCom1Contrl.wrlen)													//���ͻ�������������δ������ɣ��������͡�
		{
			ComWrt(gsCom1Config.port,gsCom1Contrl.p_wr,gsCom1Contrl.wrlen);
			gsCom1Contrl.wrlen = 0;
		}
	}
	
	if(eventMask ==  LWRS_RXCHAR )												//���յ�����   ����ôȷ�ϣ����ݽ�����ɣ���
	{
		while(GetInQLen(gsCom1Config.port))									//������δȡ
		{
			if(gsCom1Contrl.rdlen < sizeof(gsCom1Contrl.rdbuf))
			{
				*gsCom1Contrl.p_rd = (char)ComRdByte (gsCom1Config.port);
			
				gsCom1Contrl.p_rd++;
				gsCom1Contrl.rdlen++;
				
				//len = gsCom1Contrl.rdbuf[gsCom1Contrl.rdlen];
				//gsCom1Contrl.rdlen += len; 
			}
			else
			{
				ComRdByte (gsCom1Config.port);
			}
			
			int i = 150000;					//��ʱ������debug>???(i donnt know why)
			while(i--)
				gsCom1Contrl.timeout = 0;
		}

	}
	
	return;
}	

/********************************************************************************************/
/*�򿪴���1.																	            */
/********************************************************************************************/
int CVICALLBACK OpenCom1Callback (int panel, int control, int event,
								  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(gsCom1Config.open  == 0)
			{
				gCom1PanelHandle	= GetMainChildPanel();
				
				DisplayPanel (gCom1PanelHandle);										//��ʾ�������
				SetCtrlAttribute (panel, OPEN_COM1_PANEL, ATTR_LABEL_TEXT, "�رմ���");
			
				gsCom1Config.open = 1;
				
				if(ReadCom1ConfigFromFile(&gsCom1Config))								//���ļ��ж�ȡ�����ļ�
				{
					SetCviCtrlCom1Config(&gsCom1Config);								//���ô�����ʾֵ���ָ�Ϊ�ϴ�����ֵ
				}

			}else{
				SetCtrlAttribute (panel, OPEN_COM1_PANEL, ATTR_LABEL_TEXT, "�򿪴���");			
				
				gsCom1Config.open = 0;
				
				CloseCom (gsCom1Config.port);											//�رմ���
				
				if(g_com1timerId)	
					DiscardAsyncTimer (g_com1timerId);									//�ر��첽��ʱ��
			}
			break;
	}
	return 0;
}

/********************************************************************************************/
/*�������.																	            */
/********************************************************************************************/
int GetMainChildPanel(void)
{
    static int mainChildPanel = 0;

    if(!mainChildPanel)
    {
         mainChildPanel  = LoadPanel (0,UIR_COMCONFIG_NAME, COM1_PANEL);
    }

    return mainChildPanel;
}

/********************************************************************************************/
/* SendByteCom ():  		��ָ�����ڷ�������	*/
/********************************************************************************************/
void	SendByteCom(int port,char temp)
{
	
	int	i = 10;
	while(i--);
	 //printf("%02x ",(unsigned char )temp);
    
	ComWrt(port,&temp,sizeof(temp));
}

/********************************************************************************************/
/* InitCom1Contrl ():  		��ʼ��COM1�Ŀ��ƽṹ��	*/
/********************************************************************************************/
void	InitCom1Contrl(void)
{
	gsCom1Contrl.p_rd 	= gsCom1Contrl.rdbuf;		//���ջ�����
	gsCom1Contrl.p_wr 	= gsCom1Contrl.wrbuf;		//���ͻ�����
	gsCom1Contrl.rdlen	= 0;						//�����ֳ���
	gsCom1Contrl.wrlen 	= 0;						//�����ֳ���
	gsCom1Contrl.timeout= 0;						//��ʱ
	gsCom1Contrl.rdend	= 0;						//���ս�����ʾ
}


/********************************************************************************************/
/* ��com1_panel�������ʾ������Ϣ*/
/********************************************************************************************/
void	SetCviCtrlCom1Config(stcUartConfig * sUartConfig) 
{
	 SetCtrlVal (gCom1PanelHandle, COM1_PANEL_PORT, sUartConfig->port);			//ȡ�˿�
	 SetCtrlVal (gCom1PanelHandle, COM1_PANEL_BAUT, sUartConfig->baut);			//ȡ������
	 SetCtrlVal (gCom1PanelHandle, COM1_PANEL_BITS, sUartConfig->bits);			//ȡ���ݳ���
	 SetCtrlVal (gCom1PanelHandle, COM1_PANEL_PARITY, sUartConfig->parity);		//ȡУ��
	 SetCtrlVal (gCom1PanelHandle, COM1_PANEL_STOPS, sUartConfig->stops);		//ȡֹͣλ			   
}

/********************************************************************************************/
/* ReadCom1ConfigFromFile  		�������ļ��ж������ڵ������ļ���������������ļ����ã�
������ʾĬ��ֵ	*/
/********************************************************************************************/
int	ReadCom1ConfigFromFile(stcUartConfig * sUartConfig)
{
	char 	*fileName;									//�ļ�ָ��
	char 	projectDir[MAX_PATHNAME_LEN];				//����·��
	char 	fullPath[MAX_PATHNAME_LEN];					//�ļ�ȫ·��
	ssize_t size;										//�ļ���С
	int		filecom1hand;								//�ļ����
	stcUartConfig   comconfig;
	int		readsize;
	int		flg = 0;
	
	fileName = COM_CONFIG_FILE_NAME;
	
	GetProjectDir (projectDir) ;					//ȡ��Ŀ·��
//
	MakePathname (projectDir, fileName, fullPath);	//�ļ�·�� where fullPath has the following format: c:\myproject\myfile.dat
	
	if(GetFileInfo (fullPath, &size) == 0)			//�ļ�������
	{
		filecom1hand = OpenFile (fullPath, VAL_READ_WRITE, VAL_OPEN_AS_IS, VAL_BINARY);   	//���ļ�
		comconfig.baut = 57600;
		comconfig.port = 1;
		memcpy((char *)sUartConfig,(char *)&comconfig,sizeof(stcUartConfig));   
	}else{
		filecom1hand = OpenFile (fullPath, VAL_READ_WRITE, VAL_OPEN_AS_IS, VAL_BINARY);		//���ļ�
		
		SetFilePtr (filecom1hand, 0, 0);													//�ļ���ͷ��ʼ

		readsize = ReadFile (filecom1hand, (char *)&comconfig, sizeof(stcUartConfig));		//�����ļ���ȡ
		
		if(readsize)																		//���ݴ�����
		{
			if(0 < comconfig.port && comconfig.port < 11)									//�˿ں�����
				if(comconfig.stops == 1  || comconfig.stops == 2)							//ֹͣλ1��2
					 if(5<comconfig.bits && comconfig.bits < 9)								//����λ5-8
					 {
						memcpy((char *)sUartConfig,(char *)&comconfig,sizeof(stcUartConfig));
						flg = 1;
					 }
		}
		else
		{
			comconfig.baut = 57600;
			comconfig.port = 1;	
			memcpy((char *)sUartConfig,(char *)&comconfig,sizeof(stcUartConfig));   
		}
	}
	
	CloseFile(filecom1hand);							//�ر��ļ�
	
	return	flg;
}


/********************************************************************************************/
/* WriteCom1ConfigToFile  		д�������ݵ��ļ�
/********************************************************************************************/
void	WriteCom1ConfigToFile(stcUartConfig * sUartConfig)
{
	char 	*fileName;									//�ļ�ָ��
	char 	projectDir[MAX_PATHNAME_LEN];				//����·��
	char 	fullPath[MAX_PATHNAME_LEN];					//�ļ�ȫ·��
	int		filecom1hand;								//�ļ����
	
	fileName = COM_CONFIG_FILE_NAME;
	
	GetProjectDir (projectDir) ;					//ȡ��Ŀ·��

	MakePathname (projectDir, fileName, fullPath);	//�ļ�·�� where fullPath has the following format: c:\myproject\myfile.dat
	
	filecom1hand = OpenFile (fullPath, VAL_READ_WRITE, VAL_OPEN_AS_IS, VAL_BINARY);		//���ļ�
		
	SetFilePtr (filecom1hand, 0, 0);													//�ļ���ͷ��ʼ

	WriteFile (filecom1hand, (char *)sUartConfig, sizeof(stcUartConfig));				
	
	CloseFile(filecom1hand);															//�ر��ļ�     				
}

/********************************************************************************************/
/* PanelCB ():  ����1���� gsCom1Config													   	*/
/********************************************************************************************/
void	Com1Config(void)
{
	int status;
	/* 	Open and Configure Com port */	
	status = OpenComConfig (	gsCom1Config.port, 
							   	"",
								gsCom1Config.baut,
								gsCom1Config.parity, 
								gsCom1Config.bits,
								gsCom1Config.stops,
								512, 
								512								  
							  );
	
	if(status < 0)														//�˿ڴ򿪴���
	{
		MessagePopup ("���ô�����Ϣ", "�ô��ڲ��ܴ򿪣���ȷ��");
		return ;
	}

	/* 	Make sure Serial buffers are empty */
	FlushInQ (gsCom1Config.port);
	FlushOutQ (gsCom1Config.port);	
	
	/* 	���ڻص�����,�������ջ�������ݣ�ִ�лص����� */
	InstallComCallback (gsCom1Config.port, LWRS_RXCHAR | LWRS_TXEMPTY, 1, 'a' , Com1RecvAndSend, 0);
	

	gsCom1Config.open = 1;
	WriteCom1ConfigToFile(&gsCom1Config);								//��������Ϣд���ļ�
	
	InitCom1Contrl();													//��ʼ������1ͨѶ���ƿ�
	
	g_com1timerId = NewAsyncTimer ( 3*10* (1 / gsCom1Config.baut) , -1,		//���첽��ʱ����5��������ʱ�䡣
           Enabled, com1TimerCallback, NULL);	
}


/********************************************************************************************/
/* PanelCB ():  ����1����													             	*/
/********************************************************************************************/
int CVICALLBACK Com1ConfigOkCallback (int panel, int control, int event,
									  void *callbackData, int eventData1, int eventData2)
{
	int status;
	switch (event)
	{
		case EVENT_COMMIT:
			
			GetCtrlVal (panel, COM1_PANEL_PORT, &gsCom1Config.port);			//ȡ�˿�
			GetCtrlVal (panel, COM1_PANEL_BAUT, &gsCom1Config.baut);			//ȡ������
			GetCtrlVal (panel, COM1_PANEL_BITS, &gsCom1Config.bits);			//ȡ���ݳ���
			GetCtrlVal (panel, COM1_PANEL_PARITY, &gsCom1Config.parity);		//ȡУ��
			GetCtrlVal (panel, COM1_PANEL_STOPS, &gsCom1Config.stops);			//ȡֹͣλ
		    
			Com1Config();														//���ô���1
			
			HidePanel (gCom1PanelHandle);										//�˳�����

			break;
	}
	return 0;
}

/********************************************************************************************/
/* PanelCB ():  Quits this application.														*/
/********************************************************************************************/
int CVICALLBACK Com1PanelCB (int panel, int event, void *callbackData,
							int eventData1, int eventData2)
{
	switch (event) 
		{
		case EVENT_CLOSE:
			HidePanel (gCom1PanelHandle);										//�˳�����
			break;
		}
	return 0;
}
