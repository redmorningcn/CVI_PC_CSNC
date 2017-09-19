/*******************************************************************************
 *   Filename:       cvi_file.h
 *   Revised:        All copyrights reserved to redmorningcn.
 *   Revision:       v1.0
 *   Writer:	     redmorningcn.
 *
 *   Description:    
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

#ifndef	RECV_DATA_H
#define	RECV_DATA_H

/********************************************************************************************/
/* Include files																			*/
/********************************************************************************************/
#include "asynctmr.h"
#include <utility.h>
#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include <formatio.h> 
#include <comconfig.h>         
#include "pc_csnr.h"   
#include "csnr_package_deal.h"    


/********************************************************************************************/
/* Constants																					*/
/********************************************************************************************/



/*******************************************************************************************
/结构体																				 
*******************************************************************************************/
typedef	struct	_stcIapCtrl_
{
	int		filesize;		//文件大小
	int		sendsize;		//已发送文件大小
	short	sendnum;		//当前序号
	short	recvnum;		//接收序号
	char	endflg;			//结束标示
	char	echoflg;		//应答标示
	char	resendflg;		//重发标示
	char	startflg;
}stcIapCtrl;


/********************************************************************************************/
/* Globals																					*/
/********************************************************************************************/
//串口结构体及通讯

extern	stcCsnrProtocolPara	gsRecCsnrProtocolPara;


/*******************************************************************************
 *              end of file                                                    *
 *******************************************************************************/

#endif
