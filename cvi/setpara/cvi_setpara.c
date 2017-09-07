/*******************************************************************************
 *   Filename:       cvi_SetPara.c
 *   Revised:        All copyrights reserved to redmorningcn.
 *   Revision:       v1.0
 *   Writer:	     redmorningcn.
 *
 *   Description:    
					 1、接收面板操作指令和参数。



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
#include <cvirte.h>		
#include <userint.h>
#include <ansi_c.h>
#include <cvi_setpara.h>
#include <setpara.h>


/********************************************************************************************/
/* Constants																				*/
/********************************************************************************************/
//uir文件名
#define		UIR_SETPARA_NAME		"cvi_setpara.uir"

//参数配置文件名																																									                             
#define		MAX_PATHNAME_LEN		512
#define		SETPRAR_FILE_NAME		"cvi_setpara.ini"

#define		Enabled	

/********************************************************************************************/
/* Globals																					*/
/********************************************************************************************/
//面板变量
int 	gsetpara_panelHandle;
extern	int	l_eqiupmentcode;

extern	void	WriteSetParaToFile(void);
extern	int		ReadSetPararomFile(void);

/********************************************************************************************/
/*打开子面板.																	            */
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
/*打开参数设置面板																	        */
/********************************************************************************************/
int CVICALLBACK OpenSetParaPanelCallback (int panel, int control, int event,
								  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:

			gsetpara_panelHandle	= GetMainChildSetParaPanel();
			
			DisplayPanel(gsetpara_panelHandle);										//显示设置面板
			
			ReadSetPararomFile();													//调出以前存储的参数
			
			//HidePanel(gPanelHandle);												//隐藏主面板
			break;
	}
	return 0;
}

/********************************************************************************************/
/*在面板上显示时间																	        */
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
/*退出面板      																	        */
/********************************************************************************************/
void	QuitPanel(void)
{
	WriteSetParaToFile();			//退出，保存参数
	
	HidePanel (gsetpara_panelHandle);
}

int CVICALLBACK QuitCallback (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int	comNum;
	switch (event)
	{
		case EVENT_COMMIT:
			QuitPanel();			//t退出面板操作
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
				MessagePopup ("ErrMessage", "信息内容错误，请重新输入");
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
/* 给控制面板赋值
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
/* 取控制面板赋值
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
/* ReadSetParaConfigFromFile  		从配置文件中读出串口的配置文件，如果有则按配置文件配置；
否则，显示默认值	*/
/********************************************************************************************/
int	ReadSetPararomFile(void)
{
	char 	*fileName;									//文件指针
	char 	projectDir[MAX_PATHNAME_LEN];				//工程路径
	char 	fullPath[MAX_PATHNAME_LEN];					//文件全路径
	ssize_t size;										//文件大小
	int		filecom1hand;								//文件句柄
	int		readsize;
	int		flg = 0;
	
	fileName = SETPRAR_FILE_NAME;
	
	GetProjectDir (projectDir) ;					//取项目路径
//
	MakePathname (projectDir, fileName, fullPath);	//文件路劲 where fullPath has the following format: c:\myproject\myfile.dat
	
	if(GetFileInfo (fullPath, &size) == 0)			//文件不存在
	{
		return 0;
	}else{
		filecom1hand = OpenFile (fullPath, VAL_READ_WRITE, VAL_OPEN_AS_IS, VAL_BINARY);	//打开文件
		SetFilePtr (filecom1hand, 0, 0);												//文件从头开始
		
		ReadFile (filecom1hand, (char *)&gsOilPara, sizeof(gsOilPara));					//配置文件读取
		ReadFile (filecom1hand, (char *)&gstrProductInfo, sizeof(gstrProductInfo));		//配置文件读取 
		
		SetSetParaPanelVal();						//设置参数面板值
	}
	
	CloseFile(filecom1hand);						//关闭文件
	
	return	flg;
}


/********************************************************************************************/
/* WriteCom1ConfigToFile  		写配置数据到文件
/********************************************************************************************/
void	WriteSetParaToFile(void)
{
	char 	*fileName;									//文件指针
	char 	projectDir[MAX_PATHNAME_LEN];				//工程路径
	char 	fullPath[MAX_PATHNAME_LEN];					//文件全路径
	int		filecom1hand;								//文件句柄
	
	fileName = SETPRAR_FILE_NAME;
	
	GetProjectDir (projectDir) ;					//取项目路径

	MakePathname (projectDir, fileName, fullPath);	//文件路劲 where fullPath has the following format: c:\myproject\myfile.dat
	
	filecom1hand = OpenFile (fullPath, VAL_READ_WRITE, VAL_OPEN_AS_IS, VAL_BINARY);		//打开文件
		
	SetFilePtr (filecom1hand, 0, 0);													//文件从头开始

	GetSetParaPanelVal();																//取面板值
	
	WriteFile (filecom1hand, (char *)&gsOilPara, sizeof(gsOilPara));					//存运算参数
	WriteFile (filecom1hand, (char *)&gstrProductInfo, sizeof(gstrProductInfo));		//存产品信息			
	
	CloseFile(filecom1hand);															//关闭文件     				
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
				QuitPanel();				//退出面板操作
			break;
	}
	return 0;
}
