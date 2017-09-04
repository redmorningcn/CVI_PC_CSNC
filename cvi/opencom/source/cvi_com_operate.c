/*******************************************************************************
 *   Filename:       cvi_com1con.c
 *   Revised:        All copyrights reserved to redmorningcn.
 *   Revision:       v1.0
 *   Writer:	     redmorningcn.
 *
 *   Description:    
					 1、打开和关闭串口。
					 2、串口数据发送，串口数据读取。
					 3、通过串口回调函数及异步定时器控制串口收发管理。

本函数采用了串口回调函数，异步时钟，多线程，文件操作等函数。实现串口配置，串口发送接收。 对外接口函数如下
////////////////////////////////////////////////////////////////////////////
对外接口（外部工程调用）：
1、comconfig.uir参数设置页面接口；
2、OpenCom1Callback 打开串口回调函数；（CVI回调函数）
3、ReadCom1			从串口1中读取数据；
4、SendCom1			向串口1中写入数据；
5、CloseCom1	    关闭串口1；
6、GetCom1RecEndFlg 取数据接收完成标示；


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
//uir文件名
#define		UIR_COMCONFIG_NAME		"comconfig.uir"

//参数配置文件名																																									                             
#define		MAX_PATHNAME_LEN		512
#define		COM_CONFIG_FILE_NAME	"commconfig.ini"

#define		Enabled					1

/********************************************************************************************/
/* Globals																					*/
/********************************************************************************************/
//面板变量
static int 		gCom1PanelHandle;									//串口1面板

//串口结构体及通讯
extern	stcUartConfig   gsCom1Config;								//串口1配置
extern 	stcUartContrl	gsCom1Contrl;								//串口1通讯

int				g_com1systictimes = 0;
//定时器变量
int				g_com1timerId;										//异步定时器编号

/********************************************************************************************/
/* local Prototypes																				*/
/********************************************************************************************/
int 	GetMainChildPanel(void);										//子面板

void 	CVICALLBACK Com1RecvAndSend (int portNo,int eventMask,void *callbackData);  //串口收发回调函数 

void 	WriteCom1ConfigToFile(stcUartConfig * sUartConfig);				//写配置数据到文件    
void 	InitCom1Contrl(void);											//初始化COM1的控制结构体     
void	SendByteCom(int port,char temp);
void	SetCviCtrlCom1Config(stcUartConfig * sUartConfig); 


/********************************************************************************************/
/* CloseCom1 关串口1																		*/
/********************************************************************************************/
void CloseCom1(void)
{
	if( gsCom1Config.open )
	{
		DiscardAsyncTimer (g_com1timerId);							//关闭异步定时器
			/*	Close the open COM port before exiting */
		CloseCom (gsCom1Config.port);
	}
}
	

/********************************************************************************************/
/* 读取接收标示																		    	*/
/********************************************************************************************/
char	GetCom1RecEndFlg(void)
{
	return	(char)gsCom1Contrl.rdend;
}

/********************************************************************************************/
/* 读取接收标示																		    	*/
/********************************************************************************************/
void	ClearCom1RecEndFlg(void)
{
	gsCom1Contrl.rdend = 0;
}

/********************************************************************************************/
/* 从串口1取数据  																	        */
/********************************************************************************************/
int	ReadCom1(char *buf,int len)
{
	int	temp;
	
	if(len < 1)
		return 0;
	
	temp = len < gsCom1Contrl.rdlen?len:gsCom1Contrl.rdlen; 

	memcpy(buf,gsCom1Contrl.rdbuf,temp);						//复制数据到指定的数据区
	
	gsCom1Contrl.rdlen = 0;										//清控制块相关信息
	gsCom1Contrl.p_rd  = gsCom1Contrl.rdbuf;
	gsCom1Contrl.rdend = 0;
	
	return	temp;												//返回读取的信息
}

/********************************************************************************************/
/* 从串口1发送数据																	        */
/********************************************************************************************/
int	SendCom1(char *buf,int len)
{

	if( len < 1 )
		return 0;

	//增加接收状态判断。如果正在接收，则等待10ms再发送。
	int	times = g_com1systictimes;								//取串口定时器时间。
	while(gsCom1Contrl.timeout <= 1 )
	{
		 if((g_com1systictimes > times + 100) || g_com1systictimes < times)	//超过定时器50个周期装置还是接收状态，退出数据发送
			 return 0;
	}
	
	//增加发送判断，缓冲区不空，则等待10ms再发送。
	times = g_com1systictimes;								//取串口定时器时间。
	while( gsCom1Contrl.wrlen  )							//发送区不空，等待
	{
		 if((g_com1systictimes > times + 100) || g_com1systictimes < times)	//超过定时器50个周期装置还是接收状态，退出数据发送
			 return 0;
	}
	
	//////////////////////准备数据发送
	int	temp;
	temp = len;	
	
	if( len > sizeof(gsCom1Contrl.wrbuf))
		temp = sizeof(gsCom1Contrl.wrbuf);
	
	gsCom1Contrl.p_wr  = gsCom1Contrl.wrbuf;
	memcpy(gsCom1Contrl.wrbuf,buf,temp);	
	gsCom1Contrl.wrlen = temp;									//清控制块相关信息

	Com1RecvAndSend(gsCom1Config.port,LWRS_TXEMPTY,0);

	return	temp;												//返回读取的信息
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
	
	if(gsCom1Contrl.timeout >3 && gsCom1Contrl.rdlen)										//串口接收结束标示
	{
		gsCom1Contrl.rdend = 1;
	}
    
	g_com1systictimes++;
	return 0;
}

/********************************************************************************************/
/* Event_Char_Detect_Func ():  			*/
/* 收发回调函数*/
/********************************************************************************************/
void CVICALLBACK Com1RecvAndSend (int portNo,int eventMask,void *callbackData)
{
	int	len;
	if(eventMask ==  LWRS_TXEMPTY )												//发送数据
	{
		if(gsCom1Contrl.wrlen)													//发送缓冲区还有数据未发送完成，继续发送。
		{
			ComWrt(gsCom1Config.port,gsCom1Contrl.p_wr,gsCom1Contrl.wrlen);
			gsCom1Contrl.wrlen = 0;
		}
	}
	
	if(eventMask ==  LWRS_RXCHAR )												//接收到数据   （怎么确认，数据接收完成？）
	{
		while(GetInQLen(gsCom1Config.port))									//有数据未取
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
			
			int i = 150000;					//延时，调试debug>???(i donnt know why)
			while(i--)
				gsCom1Contrl.timeout = 0;
		}

	}
	
	return;
}	

/********************************************************************************************/
/*打开串口1.																	            */
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
				
				DisplayPanel (gCom1PanelHandle);										//显示设置面板
				SetCtrlAttribute (panel, OPEN_COM1_PANEL, ATTR_LABEL_TEXT, "关闭串口");
			
				gsCom1Config.open = 1;
				
				if(ReadCom1ConfigFromFile(&gsCom1Config))								//从文件中读取配置文件
				{
					SetCviCtrlCom1Config(&gsCom1Config);								//设置串口显示值，恢复为上次配置值
				}

			}else{
				SetCtrlAttribute (panel, OPEN_COM1_PANEL, ATTR_LABEL_TEXT, "打开串口");			
				
				gsCom1Config.open = 0;
				
				CloseCom (gsCom1Config.port);											//关闭串口
				
				if(g_com1timerId)	
					DiscardAsyncTimer (g_com1timerId);									//关闭异步定时器
			}
			break;
	}
	return 0;
}

/********************************************************************************************/
/*打开子面板.																	            */
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
/* SendByteCom ():  		从指定串口发送数据	*/
/********************************************************************************************/
void	SendByteCom(int port,char temp)
{
	
	int	i = 10;
	while(i--);
	 //printf("%02x ",(unsigned char )temp);
    
	ComWrt(port,&temp,sizeof(temp));
}

/********************************************************************************************/
/* InitCom1Contrl ():  		初始化COM1的控制结构体	*/
/********************************************************************************************/
void	InitCom1Contrl(void)
{
	gsCom1Contrl.p_rd 	= gsCom1Contrl.rdbuf;		//接收缓冲区
	gsCom1Contrl.p_wr 	= gsCom1Contrl.wrbuf;		//发送缓冲区
	gsCom1Contrl.rdlen	= 0;						//接收字长度
	gsCom1Contrl.wrlen 	= 0;						//发送字长度
	gsCom1Contrl.timeout= 0;						//超时
	gsCom1Contrl.rdend	= 0;						//接收结束标示
}


/********************************************************************************************/
/* 在com1_panel面板上显示配置信息*/
/********************************************************************************************/
void	SetCviCtrlCom1Config(stcUartConfig * sUartConfig) 
{
	 SetCtrlVal (gCom1PanelHandle, COM1_PANEL_PORT, sUartConfig->port);			//取端口
	 SetCtrlVal (gCom1PanelHandle, COM1_PANEL_BAUT, sUartConfig->baut);			//取波特率
	 SetCtrlVal (gCom1PanelHandle, COM1_PANEL_BITS, sUartConfig->bits);			//取数据长度
	 SetCtrlVal (gCom1PanelHandle, COM1_PANEL_PARITY, sUartConfig->parity);		//取校验
	 SetCtrlVal (gCom1PanelHandle, COM1_PANEL_STOPS, sUartConfig->stops);		//取停止位			   
}

/********************************************************************************************/
/* ReadCom1ConfigFromFile  		从配置文件中读出串口的配置文件，如果有则按配置文件配置；
否则，显示默认值	*/
/********************************************************************************************/
int	ReadCom1ConfigFromFile(stcUartConfig * sUartConfig)
{
	char 	*fileName;									//文件指针
	char 	projectDir[MAX_PATHNAME_LEN];				//工程路径
	char 	fullPath[MAX_PATHNAME_LEN];					//文件全路径
	ssize_t size;										//文件大小
	int		filecom1hand;								//文件句柄
	stcUartConfig   comconfig;
	int		readsize;
	int		flg = 0;
	
	fileName = COM_CONFIG_FILE_NAME;
	
	GetProjectDir (projectDir) ;					//取项目路径
//
	MakePathname (projectDir, fileName, fullPath);	//文件路劲 where fullPath has the following format: c:\myproject\myfile.dat
	
	if(GetFileInfo (fullPath, &size) == 0)			//文件不存在
	{
		filecom1hand = OpenFile (fullPath, VAL_READ_WRITE, VAL_OPEN_AS_IS, VAL_BINARY);   	//打开文件
		comconfig.baut = 57600;
		comconfig.port = 1;
		memcpy((char *)sUartConfig,(char *)&comconfig,sizeof(stcUartConfig));   
	}else{
		filecom1hand = OpenFile (fullPath, VAL_READ_WRITE, VAL_OPEN_AS_IS, VAL_BINARY);		//打开文件
		
		SetFilePtr (filecom1hand, 0, 0);													//文件从头开始

		readsize = ReadFile (filecom1hand, (char *)&comconfig, sizeof(stcUartConfig));		//配置文件读取
		
		if(readsize)																		//数据大于零
		{
			if(0 < comconfig.port && comconfig.port < 11)									//端口号正常
				if(comconfig.stops == 1  || comconfig.stops == 2)							//停止位1、2
					 if(5<comconfig.bits && comconfig.bits < 9)								//数据位5-8
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
	
	CloseFile(filecom1hand);							//关闭文件
	
	return	flg;
}


/********************************************************************************************/
/* WriteCom1ConfigToFile  		写配置数据到文件
/********************************************************************************************/
void	WriteCom1ConfigToFile(stcUartConfig * sUartConfig)
{
	char 	*fileName;									//文件指针
	char 	projectDir[MAX_PATHNAME_LEN];				//工程路径
	char 	fullPath[MAX_PATHNAME_LEN];					//文件全路径
	int		filecom1hand;								//文件句柄
	
	fileName = COM_CONFIG_FILE_NAME;
	
	GetProjectDir (projectDir) ;					//取项目路径

	MakePathname (projectDir, fileName, fullPath);	//文件路劲 where fullPath has the following format: c:\myproject\myfile.dat
	
	filecom1hand = OpenFile (fullPath, VAL_READ_WRITE, VAL_OPEN_AS_IS, VAL_BINARY);		//打开文件
		
	SetFilePtr (filecom1hand, 0, 0);													//文件从头开始

	WriteFile (filecom1hand, (char *)sUartConfig, sizeof(stcUartConfig));				
	
	CloseFile(filecom1hand);															//关闭文件     				
}

/********************************************************************************************/
/* PanelCB ():  串口1配置 gsCom1Config													   	*/
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
	
	if(status < 0)														//端口打开错误
	{
		MessagePopup ("配置错误信息", "该串口不能打开，请确认");
		return ;
	}

	/* 	Make sure Serial buffers are empty */
	FlushInQ (gsCom1Config.port);
	FlushOutQ (gsCom1Config.port);	
	
	/* 	串口回调函数,数据区空或接受数据，执行回调函数 */
	InstallComCallback (gsCom1Config.port, LWRS_RXCHAR | LWRS_TXEMPTY, 1, 'a' , Com1RecvAndSend, 0);
	

	gsCom1Config.open = 1;
	WriteCom1ConfigToFile(&gsCom1Config);								//将配置信息写入文件
	
	InitCom1Contrl();													//初始化串口1通讯控制块
	
	g_com1timerId = NewAsyncTimer ( 3*10* (1 / gsCom1Config.baut) , -1,		//开异步定时器，5个波特率时间。
           Enabled, com1TimerCallback, NULL);	
}


/********************************************************************************************/
/* PanelCB ():  串口1配置													             	*/
/********************************************************************************************/
int CVICALLBACK Com1ConfigOkCallback (int panel, int control, int event,
									  void *callbackData, int eventData1, int eventData2)
{
	int status;
	switch (event)
	{
		case EVENT_COMMIT:
			
			GetCtrlVal (panel, COM1_PANEL_PORT, &gsCom1Config.port);			//取端口
			GetCtrlVal (panel, COM1_PANEL_BAUT, &gsCom1Config.baut);			//取波特率
			GetCtrlVal (panel, COM1_PANEL_BITS, &gsCom1Config.bits);			//取数据长度
			GetCtrlVal (panel, COM1_PANEL_PARITY, &gsCom1Config.parity);		//取校验
			GetCtrlVal (panel, COM1_PANEL_STOPS, &gsCom1Config.stops);			//取停止位
		    
			Com1Config();														//配置串口1
			
			HidePanel (gCom1PanelHandle);										//退出窗口

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
			HidePanel (gCom1PanelHandle);										//退出窗口
			break;
		}
	return 0;
}
