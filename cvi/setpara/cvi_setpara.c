/*******************************************************************************
 *   Filename:       cvi_SetPara.c
 *   Revised:        All copyrights reserved to redmorningcn.
 *   Revision:       v1.0
 *   Writer:	     redmorningcn.
 *
 *   Description:    
					 1������������ָ��Ͳ�����



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
#include <utility.h>
#include <cvirte.h>		
#include <userint.h>
#include <ansi_c.h>
#include <cvi_setpara.h>
#include <setpara.h>


/********************************************************************************************/
/* Constants																				*/
/********************************************************************************************/
//uir�ļ���
#define		UIR_SETPARA_NAME		"cvi_setpara.uir"

//���������ļ���																																									                             
#define		MAX_PATHNAME_LEN		512
#define		SETPRAR_FILE_NAME		"cvi_setpara.ini"

#define		Enabled	

/********************************************************************************************/
/* Globals																					*/
/********************************************************************************************/
//������
int 	gsetpara_panelHandle;
extern	int	l_eqiupmentcode;

extern	void	WriteSetParaToFile(void);
extern	int		ReadSetPararomFile(void);

/********************************************************************************************/
/*�������.																	            */
/********************************************************************************************/
int GetMainChildSetParaPanel(void)
{
    static int mainChildPanel = 0;

    if(!mainChildPanel)
    {
         mainChildPanel  = LoadPanel (0,UIR_SETPARA_NAME, SETP_PANEL);
    }

    return mainChildPanel;
}

/********************************************************************************************/
/*�򿪲����������																	        */
/********************************************************************************************/
int CVICALLBACK OpenSetParaPanelCallback (int panel, int control, int event,
								  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:

			gsetpara_panelHandle	= GetMainChildSetParaPanel();
			
			DisplayPanel(gsetpara_panelHandle);										//��ʾ�������
			
			ReadSetPararomFile();													//������ǰ�洢�Ĳ���
			
			//HidePanel(gPanelHandle);												//���������
			break;
	}
	return 0;
}

/********************************************************************************************/
/*���������ʾʱ��																	        */
/********************************************************************************************/
void	DisplayTimeOnSetPanel(void)
{
	struct tm *ptr;
    time_t lt; 
	char  databuf[64];  
	
    lt=time(NULL);
    ptr=localtime((time_t*)&lt);
	
	
	sprintf(databuf,"%02d-%02d-%02d %02d:%02d:%02d",ptr->tm_year-100,
		ptr->tm_mon+1,ptr->tm_mday,ptr->tm_hour,ptr->tm_min,ptr->tm_sec);
		
	if(gsetpara_panelHandle)
		SetCtrlVal (gsetpara_panelHandle, SETP_PANEL_DATEANDTIME, databuf);  
}

/********************************************************************************************/
/*�˳����      																	        */
/********************************************************************************************/
void	QuitPanel(void)
{
	WriteSetParaToFile();			//�˳����������
	
	HidePanel (gsetpara_panelHandle);
}

int CVICALLBACK QuitCallback (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int	comNum;
	switch (event)
	{
		case EVENT_COMMIT:
			QuitPanel();			//t�˳�������
			break;
	}
	return 0;
}

int CVICALLBACK SetLocoCallBack (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			l_eqiupmentcode = FIX_CARD;
			
			GetCtrlVal(panel,SETP_PANEL_LOCOTYPE,&gstrProductInfo.sLocoId.Type);
			GetCtrlVal(panel,SETP_PANEL_LOCONUM,&gstrProductInfo.sLocoId.Num);
			
			if(gstrProductInfo.sLocoId.Num == 0|| gstrProductInfo.sLocoId.Type==0)
			{
				MessagePopup ("ErrMessage", "��Ϣ���ݴ�������������");
			}
			break;
	}
	return 0;
}

int CVICALLBACK SetTimeCallBack (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
		   	l_eqiupmentcode = TIME_CARD; 
			break;
	}
	return 0;
}


int CVICALLBACK SetDenCallBack (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
		   	l_eqiupmentcode = DENSITY_CARD; 
			GetCtrlVal(panel,SETP_PANEL_DEN,&gsOilPara.Density);
			break;
	}
	return 0;
}

int CVICALLBACK SetHigCallBack (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
		   	l_eqiupmentcode = HIGHT_CARD; 
			GetCtrlVal(panel,SETP_PANEL_HIG,&gsOilPara.Hig);
			break;
	}
	return 0;
}

int CVICALLBACK SetSelCallBack (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
		   	l_eqiupmentcode = MODEL_SELECT_CARD; 
			GetCtrlVal(panel,SETP_PANEL_SEL,&gsOilPara.ModelNum);
			break;
	}
	return 0;
}

int CVICALLBACK SetReadDataCallBack (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			l_eqiupmentcode = DATA_CARD_DIS; 
			break;
	}
	return 0;
}

int CVICALLBACK SetReadDataFinalCallBack (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			l_eqiupmentcode = DATA_CARD_FIN; 
			break;
	}
	return 0;
}

int CVICALLBACK ClearAllCallBack (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			l_eqiupmentcode = CLEAR_ALL;  
			break;
	}
	return 0;
}


int CVICALLBACK RstSysCallBack (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			l_eqiupmentcode = RST_SYS;  
			break;
	}
	return 0;
}


int CVICALLBACK SetCalcParaCallBack (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			l_eqiupmentcode = CALC_PARA; 
			
			GetCtrlVal(panel,SETP_PANEL_CALCPARA,(uint32 *)&gsOilPara.Slope);
			GetCtrlVal(panel,SETP_PANEL_HIG,&gsOilPara.Hig);
			break;
	}
	return 0;
}

int CVICALLBACK CardStaticIndicateCallBack (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	return 0;
}

/********************************************************************************************/
/* ��������帳ֵ
/********************************************************************************************/
void	SetSetParaPanelVal(void)
{
	SetCtrlVal(gsetpara_panelHandle,SETP_PANEL_LOCOTYPE,gstrProductInfo.sLocoId.Type);
	SetCtrlVal(gsetpara_panelHandle,SETP_PANEL_LOCONUM,gstrProductInfo.sLocoId.Num);	
	
	SetCtrlVal(gsetpara_panelHandle,SETP_PANEL_DEN,gsOilPara.Density);
	SetCtrlVal(gsetpara_panelHandle,SETP_PANEL_HIG,gsOilPara.Hig);
	SetCtrlVal(gsetpara_panelHandle,SETP_PANEL_SEL,gsOilPara.ModelNum);
	SetCtrlVal(gsetpara_panelHandle,SETP_PANEL_CALCPARA,gsOilPara.Slope);	
}

/********************************************************************************************/
/* ȡ������帳ֵ
/********************************************************************************************/
void	GetSetParaPanelVal(void)
{
	GetCtrlVal(gsetpara_panelHandle,SETP_PANEL_LOCOTYPE,&gstrProductInfo.sLocoId.Type);
	GetCtrlVal(gsetpara_panelHandle,SETP_PANEL_LOCONUM,&gstrProductInfo.sLocoId.Num);	
	
	GetCtrlVal(gsetpara_panelHandle,SETP_PANEL_DEN,&gsOilPara.Density);
	GetCtrlVal(gsetpara_panelHandle,SETP_PANEL_HIG,&gsOilPara.Hig);
	GetCtrlVal(gsetpara_panelHandle,SETP_PANEL_SEL,&gsOilPara.ModelNum);
	GetCtrlVal(gsetpara_panelHandle,SETP_PANEL_CALCPARA,&gsOilPara.Slope);	
}


/********************************************************************************************/
/* ReadSetParaConfigFromFile  		�������ļ��ж������ڵ������ļ���������������ļ����ã�
������ʾĬ��ֵ	*/
/********************************************************************************************/
int	ReadSetPararomFile(void)
{
	char 	*fileName;									//�ļ�ָ��
	char 	projectDir[MAX_PATHNAME_LEN];				//����·��
	char 	fullPath[MAX_PATHNAME_LEN];					//�ļ�ȫ·��
	ssize_t size;										//�ļ���С
	int		filecom1hand;								//�ļ����
	int		readsize;
	int		flg = 0;
	
	fileName = SETPRAR_FILE_NAME;
	
	GetProjectDir (projectDir) ;					//ȡ��Ŀ·��
//
	MakePathname (projectDir, fileName, fullPath);	//�ļ�·�� where fullPath has the following format: c:\myproject\myfile.dat
	
	if(GetFileInfo (fullPath, &size) == 0)			//�ļ�������
	{
		return 0;
	}else{
		filecom1hand = OpenFile (fullPath, VAL_READ_WRITE, VAL_OPEN_AS_IS, VAL_BINARY);	//���ļ�
		SetFilePtr (filecom1hand, 0, 0);												//�ļ���ͷ��ʼ
		
		ReadFile (filecom1hand, (char *)&gsOilPara, sizeof(gsOilPara));					//�����ļ���ȡ
		ReadFile (filecom1hand, (char *)&gstrProductInfo, sizeof(gstrProductInfo));		//�����ļ���ȡ 
		
		SetSetParaPanelVal();						//���ò������ֵ
	}
	
	CloseFile(filecom1hand);						//�ر��ļ�
	
	return	flg;
}


/********************************************************************************************/
/* WriteCom1ConfigToFile  		д�������ݵ��ļ�
/********************************************************************************************/
void	WriteSetParaToFile(void)
{
	char 	*fileName;									//�ļ�ָ��
	char 	projectDir[MAX_PATHNAME_LEN];				//����·��
	char 	fullPath[MAX_PATHNAME_LEN];					//�ļ�ȫ·��
	int		filecom1hand;								//�ļ����
	
	fileName = SETPRAR_FILE_NAME;
	
	GetProjectDir (projectDir) ;					//ȡ��Ŀ·��

	MakePathname (projectDir, fileName, fullPath);	//�ļ�·�� where fullPath has the following format: c:\myproject\myfile.dat
	
	filecom1hand = OpenFile (fullPath, VAL_READ_WRITE, VAL_OPEN_AS_IS, VAL_BINARY);		//���ļ�
		
	SetFilePtr (filecom1hand, 0, 0);													//�ļ���ͷ��ʼ

	GetSetParaPanelVal();																//ȡ���ֵ
	
	WriteFile (filecom1hand, (char *)&gsOilPara, sizeof(gsOilPara));					//���������
	WriteFile (filecom1hand, (char *)&gstrProductInfo, sizeof(gstrProductInfo));		//���Ʒ��Ϣ			
	
	CloseFile(filecom1hand);															//�ر��ļ�     				
}

int CVICALLBACK SetParaPanelCB (int panel, int event, void *callbackData,
								int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
				QuitPanel();				//�˳�������
			break;
	}
	return 0;
}
