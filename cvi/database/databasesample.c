#include "cvi_db.h"
#include <ansi_c.h>
#include <utility.h>
#include <formatio.h>
#include <cvirte.h>		
#include <userint.h>
#include "databasesample.h"  
#include "setmodel.h"
#include "oilrecord.h"
#include "setpara.h"
#include "CrcCheck.h"





/*******************************************************************************
* TYPEDEFS
*/							
#define		TABLE_NAME				"oilmodeltest"
#define		OILRECORD_TABLE_NAME	"busi_oil_data"


stcCalcModel	gsCalcModel;

stcFlshRec		gsFlshRec;


/***********************************************
* 描述： 任务控制块（TCB）
*/


/***********************************************
* 描述： 任务堆栈（STACKS）
*/


/*******************************************************************************
* LOCAL VARIABLES
*/
int cvi_hdbc_mysql_handle = 0;			//数据库连接句柄

/*******************************************************************************
* GLOBAL VARIABLES
*/
void	ConnectDB(void);
void	CloseDB(void);
void	ChangeRecordValue(void);


//读取数据，即刷新数据
int CVICALLBACK readdata (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	uint8 	modelnum;

	switch (event)
	{
		case EVENT_COMMIT:
			
			GetCtrlVal(panel,SETM_PANEL_MODEL_NUM, &modelnum);  //取要设置的模型编号
			ReadRecFromDB(TABLE_NAME,modelnum);  				//读取数据表中的数据    

			break;
	}
	return 0;
}

//创建数据表
int CVICALLBACK createtable (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			
			CreateDBTable(TABLE_NAME);		//创建数据表
			
			SetCtrlAttribute (db_panelHandle, SETM_PANEL_CMD_READDATA, ATTR_DIMMED, 0);  
			break;
	}
	return 0;
}

//隐藏面板
int CVICALLBACK DataBaseQuitCallback (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			HidePanel (db_panelHandle);		//隐藏面板		
			CloseDB();						//断开数据库
			break;
	}
	return 0;
}

//删除记录
int CVICALLBACK deletedata (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	Point colrow;
	int resultcode;
	int hstat;
	int value = 0;
	switch (event)
	{
		case EVENT_COMMIT:
			
			//获得活动表格单元
			GetActiveTableCell (db_panelHandle, SETM_PANEL_TABLE, &colrow);
			
			//获得活动单元格所在行，且列数为第1列单元格内的数据
			GetTableCellVal (db_panelHandle, SETM_PANEL_TABLE, MakePoint(1, colrow.y), &value);
			
			//当联接数据库成功时进行下面操作
			if (cvi_hdbc_mysql_handle > 0)
	   		{
				//对于带参数的数据库查询，首先准备执行SQL查询声明
				hstat = DBPrepareSQL (cvi_hdbc_mysql_handle, "DELETE FROM table2 WHERE ID = ?");
				
				//创建一个整型量参数预备查询方式
				resultcode = DBCreateParamInt (hstat, "", DB_PARAM_INPUT, value);
				
				//执行带参数的SQL查询				
				resultcode = DBExecutePreparedSQL (hstat);
			}
			
			DBClosePreparedSQL (hstat);
			
			//刷新数据
			readdata (panel, SETM_PANEL_CMD_READDATA, EVENT_COMMIT, NULL, 0, 0); 
			
			break;
	}
	return 0;
}

//插入记录
int CVICALLBACK insertdata (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int resultcode;
	int hstat;
	int numberofrecords;
	switch (event)
	{
		case EVENT_COMMIT:
			
			//InsertRecToDB(TABLE_NAME,&gsCalcModel);
			InsertoilRecToDB(OILRECORD_TABLE_NAME,&gsFlshRec);
			
			
			break;
	}
	return 0;
}

//修改记录
int  InsertoilRecToDBTable (void)
{
	InsertoilRecToDB(OILRECORD_TABLE_NAME,&gsFlshRec);
	return 1;
}


//修改记录
int CVICALLBACK modifydata (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	char labeltext[10];
	int comparisonresult;
	int hstat;
	int value;
	Point colrow;
	
	//设置i为静态局部变量，在本函数内部值不会丢失
	static int i = 1;
	long agestatus;
	int agevalue;
	long sexstatus;
	char sexvalue[10];
	int resultcode;
	long idstatus;
	int idvalue;
	long namestatus;
	char namevalue[10];
	int hmap;
	
	switch (event)
	{
		case EVENT_COMMIT:
			
			//设置表格为可修改状态
			SetCtrlAttribute (db_panelHandle, SETM_PANEL_TABLE, ATTR_CTRL_MODE, VAL_HOT);
			
			//以下程序表示：当点击“修改”按钮时，按钮标签将变为“保存”
			if (i)
			{
				SetCtrlAttribute (db_panelHandle, SETM_PANEL_CMD_MODIFY, ATTR_LABEL_TEXT, "保存");
				i = 0;
			}
			else
			{
				SetCtrlAttribute (db_panelHandle, SETM_PANEL_CMD_MODIFY, ATTR_LABEL_TEXT, "修改"); 
				SetCtrlAttribute (db_panelHandle, SETM_PANEL_TABLE, ATTR_CTRL_MODE, VAL_INDICATOR);
				i = 1;
			}
			
			//得到按钮的标签文本
			GetCtrlAttribute (db_panelHandle, SETM_PANEL_CMD_MODIFY, ATTR_LABEL_TEXT, labeltext);
			
			//将得到的文本与“修改”二字相对照
			comparisonresult = strcmp (labeltext, "修改");
			
			//如果确定标签文本上的字符就是“修改”二字，则执行以下操作
			if (comparisonresult == 0)
			{
				//创建一个数据库映射关系
				hmap = DBBeginMap (cvi_hdbc_mysql_handle);
				
				//以下为将数据库中的每列数据，即字段放到相对应的类型映射中
				resultcode = DBMapColumnToInt (hmap, "ID", &idvalue, &idstatus);
				resultcode = DBMapColumnToChar (hmap, "NAME", 10, namevalue, &namestatus, "");
				resultcode = DBMapColumnToChar (hmap, "SEX", 10, sexvalue, &sexstatus, "");
				resultcode = DBMapColumnToInt (hmap, "AGE", &agevalue, &agestatus);
				
				//激活映射
				hstat = DBActivateMap (hmap, "table1");
				if (hstat > 0)
				{
					//定位数据指针
					while ((resultcode = DBFetchNext (hstat)) != DB_EOF)
					{
						if (idvalue == globalvalue)
						{
							break;
						}
					}
				}
				
				GetTableCellVal (db_panelHandle, SETM_PANEL_TABLE, MakePoint(2, globalvalue), namevalue);
				GetTableCellVal (db_panelHandle, SETM_PANEL_TABLE, MakePoint(3, globalvalue), sexvalue); 
				GetTableCellVal (db_panelHandle, SETM_PANEL_TABLE, MakePoint(4, globalvalue), &agevalue); 	
				
				//更新数据库
				resultcode = DBPutRecord (hstat);
				
				//释放被激活的句柄
				resultcode = DBDeactivateMap (hmap);				
			}
			break;
	}
	return 0;
}

//删除数据表
int CVICALLBACK deletetable (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int hstat;
	switch (event)
	{
		case EVENT_COMMIT:
			DisableBreakOnLibraryErrors ();
			
			//激活SQL查询
			hstat = DBActivateSQL (cvi_hdbc_mysql_handle, "SELECT * FROM table1");
			
			//如果表存在数据，执行如下操作
			if (hstat > 0)
			{
				//释放被激活句柄
				DBDeactivateSQL (hstat);
				
				//删除数据表
				DBImmediateSQL (cvi_hdbc_mysql_handle, "DROP TABLE table1");
			}
			
			DeleteTableRows (db_panelHandle, SETM_PANEL_TABLE, 1, -1);
			
			SetCtrlAttribute (db_panelHandle, SETM_PANEL_CMD_READDATA, ATTR_DIMMED, 1);
			break;
	}
	return 0;
}



int CVICALLBACK tableCB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int value;
	Point colrow;
	switch (event)
	{
		//当在面板中的表格控件中双击时，产生以下操作
		case EVENT_LEFT_DOUBLE_CLICK:
			
			//获得活动表格当前的行与列
			GetActiveTableCell (db_panelHandle, SETM_PANEL_TABLE, &colrow);
		
			//获得本行一列中数据的ID号
			GetTableCellVal (db_panelHandle, SETM_PANEL_TABLE, MakePoint(1, colrow.y), &value);
		
			//将ID号作为全局变量处理
			globalvalue = value;	
			break;
	}
	return 0;
}

int CVICALLBACK RecvOilCallBack (int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	static	int	status = 0;
	switch (event)
	{
		case EVENT_COMMIT:

			if(status == 0 )
			{
				status = 1;
				gsRecvOilRecordCtrl.enableflg = 1;
				SetCtrlAttribute (db_panelHandle, SETM_PANEL_RECV_OIL, ATTR_LABEL_TEXT, "stop recv"); 

			}else
			{
				status = 0;	
				gsRecvOilRecordCtrl.enableflg = 0;
				SetCtrlAttribute (db_panelHandle, SETM_PANEL_RECV_OIL, ATTR_LABEL_TEXT, "contiue recv"); 
			}
			break;
	}
	return 0;
}


