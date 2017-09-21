#include <utility.h>
#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include "pc_csnr.h"
#include "cvi_com_operate.h"
#include "csnr_package_deal.h"
#include "cvi_file.h"
#include "senddata.h"
#include "setpara.h"
#include "oilrecord.h"
#include "databasesample.h"


/********************************************************************************************/
/* Constants										    */
/********************************************************************************************/
//uir�ļ���
#define		UIR_MAIN_NAME			"pc_csnr.uir"

//��ӡ������Ϣ
//#define		RECV_PRINTF	    1


/********************************************************************************************/
/* Globals										    */
/********************************************************************************************/
//������
int 		gPanelHandle;										//װ�������

static int		gthreadFunctionId1;									//�̱߳��
static int 		gExiting;											//�߳��˳�

int		gSendDataFlg = 0;

//���ڽṹ�弰ͨѶ
extern	stcUartConfig   	gsCom1Config;								//����1����
extern	stcUartContrl		gsCom1Contrl;								//����1ͨѶ
extern	stcCsnrProtocolPara	gsCsnrProtocolPara;							//Э��˿�

typedef 	struct	_stcmainpara_
{
	int		sendfmt;
	int		recvfmt;
}stcmainpara;

stcmainpara		gsmainpara;	

/********************************************************************************************/
/* local Prototypes																				*/
/********************************************************************************************/
int		GetMainPanelHandle(void);										//�����								
void 	printftext(int	panel,int id,const char *format, ...);
void 	CloseCom1(void);
char	WriteMainPanelData_File(void) ;
int		ReadMainPanelData_File(void) ;
static int 	CVICALLBACK ThreadFunction1 (void *functionData);
extern	void	SendTextBox(void);
/********************************************************************************************/
/* Application entry point.																	*/
/********************************************************************************************/
int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)					/* Initialize CVI libraries */
		return -1;	/* out of memory */

    gPanelHandle 		= GetMainPanelHandle();
	
	gExiting = 0;									//�����߳�
	/* Schedule two thread functions */				//���̣߳��������̺߳���
	CmtScheduleThreadPoolFunction (DEFAULT_THREAD_POOL_HANDLE,
								   ThreadFunction1, NULL,
								   &gthreadFunctionId1);	
	/* 	Display the panel and run the UI */
	DisplayPanel (gPanelHandle);
	
	RunUserInterface ();
	
	CloseCom1();
	WriteMainPanelData_File();
	
	gExiting = 1;									//�߳��˳�
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

void	RecvDeal(char* buf,int len);
char	ClearCom1RecEndFlg(void);
/* First thread function */		//�̴߳�����

static int CVICALLBACK ThreadFunction1 (void *functionData)
{
	char recbuf[512];
	char sendbuf[512];
	int	 reclen;
	
	while (!gExiting)							//�߳��˳�
	{
		if(GetCom1RecEndFlg())					//�������ݣ�����ʾ
		{
			ClearCom1RecEndFlg();
			reclen = ReadCom1(recbuf,sizeof(recbuf));
			
			if(reclen < sizeof(recbuf))
				recbuf[reclen] = '\0';			//��ӽ�����ʾ	
			
#ifdef	 RECV_PRINTF
			if(gsmainpara.recvfmt == 1)			//��ӡ����
			{
				for(int i = 0;i < reclen;i++)  
				{
					printftext(gPanelHandle,PANEL_INFOTEXTBOX,"%02x ",(unsigned char)recbuf[i]);	
				}
			}else
			{
				SetCtrlVal(gPanelHandle,PANEL_INFOTEXTBOX,recbuf);   	//��ȡ������������
				SetCtrlVal(gPanelHandle,PANEL_INFOTEXTBOX,"\r\n");   	//���յ������ݻ��д���
			}
#endif			
			
			
			RecvDeal(recbuf,reclen);										//�������ݴ���
			//InsertTextBoxLine (gPanelHandle, PANEL_COM1RECVTEXTBOX,-1, recbuf);
		}
		
		GetCtrlVal(gPanelHandle,PANEL_COM1SENDTEXTBOX,sendbuf);			
	
////////////////////////////////////////////////////////////////���ݷ���
		if(		
					gsCom1Config.open == 1
				&&  gSendDataFlg != 0					//���ݷ��������ݷ���
		  )
		{
			//SendCom1(sendbuf,strlen(sendbuf));
			if(gSendDataFlg == 1)						//���͵�֡����
			{
				SendTextBox();							//	
				gSendDataFlg = 0;							//�������

			} 
			else if(gSendDataFlg == 2)					//�����ļ�
			{
				SendFileData();							
			}
				
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


int CVICALLBACK SetSendFlgCallback (int panel, int control, int event,
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


int CVICALLBACK ClearTextBoxCallback (int panel, int control, int event,
									  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			
			 ResetTextBox (panel, PANEL_INFOTEXTBOX, "");
			break;
	}
	return 0;
}


int   g_systime = 0; 
 

/********************************************************************************************/
/*CVI_COM_Tick_Handl���ڴ�����,��com��弰�������¼�							            */
/********************************************************************************************/
void CVI_COMPanelHandle(int panel)
{
	static	int	times = 0;
	
	if(times == 0)								
	{
		times++;
		if(ReadCom1ConfigFromFile(&gsCom1Config))	//�紮�������ļ���Ч��ֱ�Ӵ�
		{
			Com1Config();							//���ô���
		}
		ReadMainPanelData_File();
	}
	
	if(gsCom1Config.open == 1)	  //���ڴ�
	{
		SetCtrlAttribute (panel, PANEL_OPEN_BIN_FILE, ATTR_DIMMED, 0);
		SetCtrlAttribute (panel, PANEL_COM1_SEND_DATA, ATTR_DIMMED, 0);
		SetCtrlAttribute (panel, OPEN_COM1_PANEL, ATTR_LABEL_TEXT, "�رմ���");

		//SetCtrlAttribute (panel, PANEL_AUTOSENDFLG, ATTR_DIMMED, 0);
	}
	
	if(gsCom1Config.open == 0)	  //���ڹر�
	{
		SetCtrlAttribute (panel, PANEL_OPEN_BIN_FILE, ATTR_DIMMED, 1);
		SetCtrlAttribute (panel, PANEL_COM1_SEND_DATA, ATTR_DIMMED, 1);
		SetCtrlAttribute (panel, OPEN_COM1_PANEL, ATTR_LABEL_TEXT, "�򿪴���");

		//SetCtrlAttribute (panel, PANEL_AUTOSENDFLG, ATTR_DIMMED, 1);
		//SetCtrlVal(panel,PANEL_AUTOSENDFLG,0);
	}	
}

/********************************************************************************************/
/*CVI_Set_Data_Handl���ڴ�����,����������ؾ��								            */
/********************************************************************************************/
void CVI_SentDataHandle(int panel)
{
	static	int	autotime = 0;
	double	autoflg = 0;
	int		i= 10;
	
	
	GetCtrlVal (panel, PANEL_TIMER, &autoflg);	//ȡ�Զ����ͱ�ʾ  

	GetCtrlAttribute (panel, PANEL_TIMER, ATTR_INTERVAL,&autoflg );
	
	if( autoflg < 0.01)				//�Զ�����
	{
		if(autotime < 10 )		//10ms  
		{
			GetCtrlVal (panel, PANEL_DELAYMS, &autotime);
			gSendDataFlg = 1;	//���ñ�ʾ��1
		}
		else
		{
			i = 10;
			while(i--)
				autotime--;
		}
	}else
	{
		 autotime = 0;
	}
	
////////////////////////////////////////////////////////////ȡЭ���ڵ�ֵ			
	GetCtrlVal(panel,PANEL_SOURCEADDR,&gsCsnrProtocolPara.sourceaddr);	
	GetCtrlVal(panel,PANEL_DESTADDR,&gsCsnrProtocolPara.destaddr);
	GetCtrlVal(panel,PANEL_FRAMNUM,&gsCsnrProtocolPara.framnum);
	GetCtrlVal(panel,PANEL_FRAMCODE,&gsCsnrProtocolPara.framcode);

////////////////////////////////////////////////////////////������
	GetCtrlVal(panel,PANEL_SENDFMT,&gsmainpara.sendfmt);
	GetCtrlVal(panel,PANEL_RECVFMT,&gsmainpara.recvfmt);
}

void	DisplayTimeOnSetPanel(void);
/********************************************************************************************/
/*CVI_SetPara���ڲ���������ؾ��												            */
/********************************************************************************************/
void CVI_SetParaHandle(int panel)
{
	DisplayTimeOnSetPanel();
	Com_SetParaTask();									//���ڷ��Ͳ�������ָ��	
}

int  InsertoilRecToDBTable (void);
/********************************************************************************************/
/*CVI_SetPara���ڲ���������ؾ��												            */
/********************************************************************************************/
void CVI_DataBaseHandle(int panel)
{
	if(gsRecvOilRecordCtrl.storeflg == 1 && db_panelHandle)
	{
		gsRecvOilRecordCtrl.storeflg = 0;
		
		InsertoilRecToDBTable();
	}
	
	if(db_panelHandle && gsRecvOilRecordCtrl.ICflg == 1)
	{
		SetCtrlVal(db_panelHandle,SETM_PANEL_CUR_NUM,gsRecvOilRecordCtrl.currecnum);
		SetCtrlVal(db_panelHandle,SETM_PANEL_ICREAD_NUM,gsRecvOilRecordCtrl.ICreadnum);
	}
}

/********************************************************************************************/
/*��嶨ʱ������																            */
/********************************************************************************************/
int CVICALLBACK SysTickCallback (int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{

	switch (event)
	{
		case EVENT_TIMER_TICK:
			
			g_systime++;									   	//ϵͳ��ʱ				
			
///////////////////////////////////////////////////////////////////������崦��			
			CVI_COMPanelHandle(panel);						
		
//////////////////////////////////////////////////////////////////�������ݷ��������ش���	
			CVI_SentDataHandle(panel);
			
//////////////////////////////////////////////////////////////////�������ݷ��������ش���	
			CVI_SetParaHandle(panel);
			
//////////////////////////////////////////////////////////////////�������ݷ��������ش���	
			CVI_DataBaseHandle(panel);
			
			break;
	}
	return 0;
}


//�����������
#define	MNAI_PANEL_FILE	"cvi_main_myinfo.ini"
typedef	struct	_stcmianpanelpara{
	struct	_csnchead_
	{
		char	source;
		char	denst;
		char 	code;
		char	framenum;
	}stccsnchead ;
	
	char	sentbuf[256];
}stcmianpanelpara;

//������
stcmianpanelpara	smainpanelpara;

/********************************************************************************************/
/* �������ļ�д������					                							    	*/
/********************************************************************************************/
char	WriteMainPanelData_File(void)
{
	char	dirstring[512];
	char	filestring[512];
	int		filehandle;

	GetCtrlVal(gPanelHandle,PANEL_COM1SENDTEXTBOX,smainpanelpara.sentbuf);
	GetCtrlVal(gPanelHandle,PANEL_SOURCEADDR,&smainpanelpara.stccsnchead.source);
	GetCtrlVal(gPanelHandle,PANEL_DESTADDR,&smainpanelpara.stccsnchead.denst);
	GetCtrlVal(gPanelHandle,PANEL_FRAMCODE,&smainpanelpara.stccsnchead.code);
	GetCtrlVal(gPanelHandle,PANEL_FRAMNUM,&smainpanelpara.stccsnchead.framenum);

	GetProjectDir (dirstring);
	MakePathname (dirstring, MNAI_PANEL_FILE, filestring);
	
	filehandle = OpenFile (filestring, VAL_READ_WRITE, VAL_OPEN_AS_IS, VAL_BINARY);					 
	SetFilePtr (filehandle, 0, 0);
	
	WriteFile(filehandle,(char *)&smainpanelpara,sizeof(smainpanelpara));
	
	CloseFile(filehandle);
	
	return 1;
}

/********************************************************************************************/
/* �������ļ��ж�ȡ����					                								*/
/********************************************************************************************/
int	ReadMainPanelData_File(void)
{
	char	dirstring[512];
	char	filestring[512];
	int		filehandle;
	int	datalen = 0;
	
	GetProjectDir (dirstring);
	MakePathname (dirstring, MNAI_PANEL_FILE, filestring);
	
	filehandle = OpenFile (filestring, VAL_READ_WRITE, VAL_OPEN_AS_IS, VAL_BINARY);					 
	SetFilePtr (filehandle, 0, 0);
	datalen = ReadFile(filehandle,(char *)&smainpanelpara,sizeof(smainpanelpara));   
	
	CloseFile(filehandle);
	
	SetCtrlVal(gPanelHandle,PANEL_COM1SENDTEXTBOX,smainpanelpara.sentbuf);
	
	SetCtrlVal(gPanelHandle,PANEL_SOURCEADDR,smainpanelpara.stccsnchead.source);
	SetCtrlVal(gPanelHandle,PANEL_DESTADDR,smainpanelpara.stccsnchead.denst);
	SetCtrlVal(gPanelHandle,PANEL_FRAMCODE,smainpanelpara.stccsnchead.code);
	SetCtrlVal(gPanelHandle,PANEL_FRAMNUM,smainpanelpara.stccsnchead.framenum);

	return datalen;
}

