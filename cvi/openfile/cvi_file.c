/*******************************************************************************
 *   Filename:       cvi_file.c
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
#include "recvdata.h"


/********************************************************************************************/
/* Constants																				*/
/********************************************************************************************/
#define		UIR_BINFILE_PANEL_NAME		"iap.uir"


/********************************************************************************************/
/* Globals																					*/
/********************************************************************************************/
//������
int	l_binfilehandle = 0;		//�ļ��ؼ�

int	gBinFilePanelHandle;

 extern int 	gPanelHandle;										//װ�������
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
			HidePanel (gBinFilePanelHandle);										//�˳�����
			DisplayPanel (gPanelHandle);										//��ʾ�������

			break;
		}
	return 0;
}

/********************************************************************************************/
/*�������.																	            */
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
/*���ļ����.																	            */
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
				
				
				DisplayPanel (gBinFilePanelHandle);										//��ʾ�������
				
				//HidePanel(gPanelHandle);
				
				//SetCtrlAttribute (panel, OPEN_COM1_PANEL, ATTR_LABEL_TEXT, "�رմ���");

	
			}else{
				//SetCtrlAttribute (panel, OPEN_COM1_PANEL, ATTR_LABEL_TEXT, "�򿪴���");			
				
				status = 0;
				gBinFilePanelHandle	= GetMainChildBinFilePanel();
				
				DisplayPanel (gBinFilePanelHandle);										//��ʾ�������			
			}
			break;
	}
	return 0;
}


/********************************************************************************************/
/*���ļ�ѡ��																            */
/********************************************************************************************/
int CVICALLBACK OpenBinFileCallback (int panel, int control, int event,
									 void *callbackData, int eventData1, int eventData2)
{
	char		binpathstring[512];
	int			len;
	switch (event)
	{
		case EVENT_COMMIT:
			
			if(GetHexFileDir(binpathstring))						//�ļ�ѡ����Ч
			{
				SetCtrlVal(panel,IAP_PANEL_BINPAHT,binpathstring);		
			
				GetCtrlAttribute (panel, IAP_PANEL_BINPAHT, ATTR_STRING_TEXT_LENGTH, &len);
				if(len)
				{
					SetCtrlAttribute (panel, IAP_PANEL_DOWNLOAD, ATTR_DIMMED, 0);
				}
			}else{
				MessagePopup ("��Ϣ����", "�ļ����쳣��������ѡ���ļ�");
			}

			break;
	}
	return 0;
}

/********************************************************************************************/
/* GetHexFileDir ȡ�ļ�·��,�����ļ���ȡ�ļ�����							                */
/********************************************************************************************/
int	GetHexFileDir(char * filedir)
{
	
	char 	curparhstring[512];
	int		status;
	
	GetProjectDir(curparhstring);
	
	status = FileSelectPopup ("curparhstring", "*.bin", "*.txt;*.doc;*.bin", "ѡ�������ļ�", VAL_OK_BUTTON, 0, 0, 1, 0, filedir); 
	
	if(status)
	{
		gsBinFileInfo.handle = OpenFile (filedir, VAL_READ_ONLY, VAL_OPEN_AS_IS, VAL_BINARY); 
	
		GetFileInfo (filedir,&gsBinFileInfo.size );
		
		return 1;
	}
	return 0;
}

/********************************************************************************************/
/* CloseBinFile�ر��ļ�						                								*/
/********************************************************************************************/
void	CloseBinFile(void)
{
	CloseFile (gsBinFileInfo.handle);
}

/********************************************************************************************/
/* ��bin�ļ���ȡ����						                								*/
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
/* �ļ��Ѵ�����ʼ��						                								*/
/********************************************************************************************/
void	StartFileFromHome(void)
{
	SetFilePtr (gsBinFileInfo.handle, 0, 0);
	gsBinFileInfo.deallen = 0;
}

/********************************************************************************************/
/* �����ļ�����ٷֱ�						                								*/
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
/* �������ļ�д������					                							    	*/
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
/* �������ļ��ж�ȡ����					                								*/
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
/* �������ؿ�ʼ				                								*/
/********************************************************************************************/
int CVICALLBACK SetProgramFlgCallback (int panel, int control, int event,
									   void *callbackData, int eventData1, int eventData2)
{
	static	status = 0;
	switch (event)
	{
		case EVENT_COMMIT:
			if(status == 0)					//�����ļ����ر�ʾ
			{
				//InitCom1Contrl();			//��մ��ڻ�����
				gsIapCtrl.startflg = 1;
				
				gSendDataFlg = 2;
				SetCtrlAttribute (panel, IAP_PANEL_DOWNLOAD, ATTR_LABEL_TEXT, "ȡ������");
				status =1;
			}
			else							//ȡ���ļ����ر�ʾ�����������
			{			
				gSendDataFlg = 0;
				SetCtrlAttribute (panel, IAP_PANEL_DOWNLOAD, ATTR_LABEL_TEXT, "��ʼ����");
				
				//InitCom1Contrl();			//��մ��ڻ�����
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
