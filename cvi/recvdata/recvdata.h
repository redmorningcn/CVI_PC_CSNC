/*******************************************************************************
 *   Filename:       cvi_file.h
 *   Revised:        All copyrights reserved to redmorningcn.
 *   Revision:       v1.0
 *   Writer:	     redmorningcn.
 *
 *   Description:    
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
/�ṹ��																				 
*******************************************************************************************/
typedef	struct	_stcIapCtrl_
{
	int		filesize;		//�ļ���С
	int		sendsize;		//�ѷ����ļ���С
	short	sendnum;		//��ǰ���
	short	recvnum;		//�������
	char	endflg;			//������ʾ
	char	echoflg;		//Ӧ���ʾ
	char	resendflg;		//�ط���ʾ
	char	startflg;
}stcIapCtrl;


/********************************************************************************************/
/* Globals																					*/
/********************************************************************************************/
//���ڽṹ�弰ͨѶ

extern	stcCsnrProtocolPara	gsRecCsnrProtocolPara;


/*******************************************************************************
 *              end of file                                                    *
 *******************************************************************************/

#endif
