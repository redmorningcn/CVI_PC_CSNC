/*******************************************************************************
 *   Filename:       cvi_file.c
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
#include "recvdata.h"


/********************************************************************************************/
/* Constants																				*/
/********************************************************************************************/
#define		UIR_BINFILE_PANEL_NAME		"iap.uir"


/********************************************************************************************/
/* Globals																					*/
/********************************************************************************************/
//面板变量
int	l_binfilehandle = 0;		//文件控件

int	gBinFilePanelHandle;

 extern int 	gPanelHandle;										//装置主面板
 extern	int		gSendDataFlg;
/********************************************************************************************/
/* local Prototypes																				*/
/********************************************************************************************/
int	GetHexFileDir(char * filedir);



stcFileInfo		gsBinFileInfo;

/********************************************************************************************/
/* local Prototypes																			*/
/********************************************************************************************/

/********************************************************************************************/
/* PanelCB ():  Quits this application.														*/
/********************************************************************************************/
int CVICALLBACK BinFilePanelCB (int panel, int event, void *callbackData,
							int eventData1, int eventData2)
{
	switch (event) 
		{
		case EVENT_CLOSE:
			HidePanel (gBinFilePanelHandle);										//退出窗口
			DisplayPanel (gPanelHandle);										//显示设置面板

			break;
		}
	return 0;
}

/********************************************************************************************/
/*打开子面板.																	            */
/********************************************************************************************/
int GetMainChildBinFilePanel(void)
{
    static int mainChildPanel = 0;

//	InstallPopup (UIR_BINFILE_PANEL_NAME);

    if(!mainChildPanel)
    {
         mainChildPanel  = LoadPanel (0,"IAP.uir", IAP_PANEL);
    }

    return mainChildPanel;
}

/********************************************************************************************/
/*打开文件面板.																	            */
/********************************************************************************************/
int CVICALLBACK OpenBinFilePanelCallback (int panel, int control, int event,
									 void *callbackData, int eventData1, int eventData2)
{
	static	char	status = 0;
	switch (event)
	{
		case EVENT_COMMIT:
			if(status == 0)
			{
				status = 1;
				gBinFilePanelHandle	= GetMainChildBinFilePanel();
				
				
				DisplayPanel (gBinFilePanelHandle);										//显示设置面板
				
				//HidePanel(gPanelHandle);
				
				//SetCtrlAttribute (panel, OPEN_COM1_PANEL, ATTR_LABEL_TEXT, "关闭串口");

	
			}else{
				//SetCtrlAttribute (panel, OPEN_COM1_PANEL, ATTR_LABEL_TEXT, "打开串口");			
				
				status = 0;
				gBinFilePanelHandle	= GetMainChildBinFilePanel();
				
				DisplayPanel (gBinFilePanelHandle);										//显示设置面板			
			}
			break;
	}
	return 0;
}


/********************************************************************************************/
/*打开文件选项																            */
/********************************************************************************************/
int CVICALLBACK OpenBinFileCallback (int panel, int control, int event,
									 void *callbackData, int eventData1, int eventData2)
{
	char		binpathstring[512];
	int			len;
	switch (event)
	{
		case EVENT_COMMIT:
			
			if(GetHexFileDir(binpathstring))						//文件选择有效
			{
				SetCtrlVal(panel,IAP_PANEL_BINPAHT,binpathstring);		
			
				GetCtrlAttribute (panel, IAP_PANEL_BINPAHT, ATTR_STRING_TEXT_LENGTH, &len);
				if(len)
				{
					SetCtrlAttribute (panel, IAP_PANEL_DOWNLOAD, ATTR_DIMMED, 0);
				}
			}else{
				MessagePopup ("消息提醒", "文件打开异常，请重新选择文件");
			}

			break;
	}
	return 0;
}

/********************************************************************************************/
/* GetHexFileDir 取文件路径,并打开文件，取文件长度							                */
/********************************************************************************************/
int	GetHexFileDir(char * filedir)
{
	
	char 	curparhstring[512];
	int		status;
	
	GetProjectDir(curparhstring);
	
	status = FileSelectPopup ("curparhstring", "*.bin", "*.txt;*.doc;*.bin", "选择下载文件", VAL_OK_BUTTON, 0, 0, 1, 0, filedir); 
	
	if(status)
	{
		gsBinFileInfo.handle = OpenFile (filedir, VAL_READ_ONLY, VAL_OPEN_AS_IS, VAL_BINARY); 
	
		GetFileInfo (filedir,&gsBinFileInfo.size );
		
		return 1;
	}
	return 0;
}

/********************************************************************************************/
/* CloseBinFile关闭文件						                								*/
/********************************************************************************************/
void	CloseBinFile(void)
{
	CloseFile (gsBinFileInfo.handle);
}

/********************************************************************************************/
/* 从bin文件读取数据						                								*/
/********************************************************************************************/
int	ReadFileData(int	addr,char *buf,int	reclen)
{
	int		len = 0;	
	int		i;
	
	if(gsBinFileInfo.handle == 0)
		return len;
	
	SetFilePtr (gsBinFileInfo.handle, addr, 0);
	
	len = ReadFile (gsBinFileInfo.handle, buf, reclen);
	
	//gsBinFileInfo.deallen += len;
	
	return len;
}

/********************************************************************************************/
/* 文件已处理，初始化						                								*/
/********************************************************************************************/
void	StartFileFromHome(void)
{
	SetFilePtr (gsBinFileInfo.handle, 0, 0);
	gsBinFileInfo.deallen = 0;
}

/********************************************************************************************/
/* 设置文件处理百分比						                								*/
/********************************************************************************************/
char	SetFileDealPer(void)
{
	char	per = 0;
	
	if((gsBinFileInfo.size /100)&&gsBinFileInfo.deallen)
	{
		per	 =  (char)(gsBinFileInfo.deallen/(gsBinFileInfo.size /100));
	}	
	
	return per;
}


#define	SOTREFILE	"cvi_file_myinfo.ini"
/********************************************************************************************/
/* 向配置文件写入数据					                							    	*/
/********************************************************************************************/
char	WriteData_File(int	addr,char	buf[],int  len)
{
	char	dirstring[512];
	char	filestring[512];
	int		filehandle;
	
	GetProjectDir (dirstring);
	MakePathname (dirstring, SOTREFILE, filestring);
	
	filehandle = OpenFile (filestring, VAL_READ_WRITE, VAL_OPEN_AS_IS, VAL_BINARY);					 
	SetFilePtr (filehandle, addr, 0);
	
	WriteFile(filehandle,buf,len);
	
	CloseFile(filehandle);
	
	return 1;
}

/********************************************************************************************/
/* 从配置文件中读取数据					                								*/
/********************************************************************************************/
int	ReadData_File(int  addr,char	buf[],int  len)
{
	char	dirstring[512];
	char	filestring[512];
	int		filehandle;
	int	datalen = 0;
	
	GetProjectDir (dirstring);
	MakePathname (dirstring, SOTREFILE, filestring);
	
	filehandle = OpenFile (filestring, VAL_READ_WRITE, VAL_OPEN_AS_IS, VAL_BINARY);					 
	SetFilePtr (filehandle, addr, 0);
	datalen = ReadFile(filehandle,buf,len);
	
	CloseFile(filehandle);
	
	return datalen;
}


stcIapCtrl	gsIapCtrl;

extern	void	InitCom1Contrl(void);

/********************************************************************************************/
/* 设置下载开始				                								*/
/********************************************************************************************/
int CVICALLBACK SetProgramFlgCallback (int panel, int control, int event,
									   void *callbackData, int eventData1, int eventData2)
{
	static	status = 0;
	switch (event)
	{
		case EVENT_COMMIT:
			if(status == 0)					//设置文件下载标示
			{
				//InitCom1Contrl();			//清空串口缓存区
				gsIapCtrl.startflg = 1;
				
				gSendDataFlg = 2;
				SetCtrlAttribute (panel, IAP_PANEL_DOWNLOAD, ATTR_LABEL_TEXT, "取消下载");
				status =1;
			}
			else							//取消文件下载标示，清空下载区
			{			
				gSendDataFlg = 0;
				SetCtrlAttribute (panel, IAP_PANEL_DOWNLOAD, ATTR_LABEL_TEXT, "开始下载");
				
				//InitCom1Contrl();			//清空串口缓存区
				status = 0;
			}
			
			StartFileFromHome();
			break;
	}
	return 0;
}


void printftext(int	panel,int id,const char *format, ...) 
{ 
   va_list  argptr;
   unsigned int   cnt;
   char     a[255];
   
   strlen(format);
   if(strlen(format) > (sizeof(a) - 55))
   {
		return	;
   }
   va_start(argptr, format);
   
   cnt = vsprintf(a, format, argptr);
   
   va_end(argptr);
   
   SetCtrlVal(panel,id,a);
}
