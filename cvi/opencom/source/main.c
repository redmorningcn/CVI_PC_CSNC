/*******************************************************************************
 *   Filename:       main.c
 *   Revised:        All copyrights reserved to redmorningcn.
 *   Revision:       v1.0
 *   Writer:	     redmorningcn.
 *
 *   Description:    
					���Ժ���

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
#include "main.h"
#include "cvi_com_operate.h"
#include <formatio.h> 

/********************************************************************************************/
/* Constants																					*/
/********************************************************************************************/
//uir�ļ���
#define		UIR_MAIN_NAME			"main.uir"



/********************************************************************************************/
/* Globals																					*/
/********************************************************************************************/
//������
static int 		gPanelHandle;										//װ�������

static int		gthreadFunctionId1;									//�̱߳��
static int 		gExiting;											//�߳��˳�

static	int		gSendDataFlg = 0;

//���ڽṹ�弰ͨѶ
extern	stcUartConfig   gsCom1Config;								//����1����
extern	stcUartContrl	gsCom1Contrl;								//����1ͨѶ


/********************************************************************************************/
/* local Prototypes																				*/
/********************************************************************************************/
int	GetMainPanelHandle(void);										//�����								
static int CVICALLBACK ThreadFunction1 (void *functionData);


void CloseCom1(void);
/********************************************************************************************/
/* Application entry point.																	*/
/********************************************************************************************/
int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)	/* Initialize CVI libraries */
		return -1;	/* out of memory */

    gPanelHandle 		= GetMainPanelHandle();
	
	/* Schedule two thread functions */				 //���̣߳��������̺߳���
	CmtScheduleThreadPoolFunction (DEFAULT_THREAD_POOL_HANDLE,
								   ThreadFunction1, NULL,
								   &gthreadFunctionId1);	
	
	/* 	Display the panel and run the UI */
	DisplayPanel (gPanelHandle);
	

	RunUserInterface ();
	
	CloseCom1();
	
	/* Wait for the thread functions to finish executing */
	CmtWaitForThreadPoolFunctionCompletion (DEFAULT_THREAD_POOL_HANDLE,
											gthreadFunctionId1,
											OPT_TP_PROCESS_EVENTS_WHILE_WAITING);	
	/* Release thread functions */
	CmtReleaseThreadPoolFunctionID (DEFAULT_THREAD_POOL_HANDLE, gthreadFunctionId1);
	
	/*	Discard the loaded panels from memory */
	DiscardPanel (gPanelHandle);
	
	return 0;
}

/* First thread function */		//���ڽ���
static int CVICALLBACK ThreadFunction1 (void *functionData)
{
	char recbuf[512];
	char sendbuf[512];
	int	 reclen;
	
	
	while (!gExiting)						//�߳��˳�
	{
		if(GetCom1RecEndFlg())				//�������ݣ�����ʾ
		{
			reclen = ReadCom1(recbuf,sizeof(recbuf));
			
			if(reclen < sizeof(recbuf))
				recbuf[reclen] = '\0';		//��ӽ�����ʾ	
				
			SetCtrlVal(gPanelHandle,PANEL_COM1RECVTEXTBOX,recbuf);
			SetCtrlVal(gPanelHandle,PANEL_COM1RECVTEXTBOX,"\r\n");
			//InsertTextBoxLine (gPanelHandle, PANEL_COM1RECVTEXTBOX,-1, recbuf);

		}
		
		GetCtrlVal(gPanelHandle,PANEL_COM1SENDTEXTBOX,sendbuf);
		
		if(		
					strlen(sendbuf)
				&& 	gsCom1Config.open 
				&&  gSendDataFlg == 1
		  )
		{
			gSendDataFlg = 0;
			SendCom1(sendbuf,strlen(sendbuf));
		}
	}

	return 0;
}

/********************************************************************************************/
/*�������.																	            */
/********************************************************************************************/
int GetMainPanelHandle(void)
{
    static int mainPanelHandle = 0;

    if(!mainPanelHandle)
    {
        mainPanelHandle = LoadPanel (0, UIR_MAIN_NAME, PANEL);
    }

    return mainPanelHandle;
}

/********************************************************************************************/
/* PanelCB ():  Quits this application.														*/
/********************************************************************************************/
int CVICALLBACK PanelCB (int panel, int event, void *callbackData,
							int eventData1, int eventData2)
{
	switch (event) 
		{
		case EVENT_CLOSE:
			QuitUserInterface (0);
			break;
		}
	return 0;
}

/********************************************************************************************/
/* PanelCB ():  Quits this application.														*/
/********************************************************************************************/
int CVICALLBACK Quit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			QuitUserInterface(0);
			break;
		}
	return 0;
}

int CVICALLBACK SendDataCallback (int panel, int control, int event,
								  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			
			gSendDataFlg = 1;
			break;
	}
	return 0;
}
