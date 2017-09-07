#include "cvi_db.h"
#include <ansi_c.h>
#include <utility.h>
#include <formatio.h>
#include <cvirte.h>		
#include <userint.h>
#include "databasesample.h"
#include "setpara.h"
#include "CrcCheck.h"




/*******************************************************************************
* CONSTANTS
*/

/*******************************************************************************
* MACROS
*/
//ODBC 数据源(32 位),DSN
#define		ODBC_DNS	"MySql32"
#define		TABLE_NAME	"oilmodeltest"


/*******************************************************************************
* TYPEDEFS
*/

typedef struct _stcModPot_						//IC 卡cd参数
{
	unsigned short	HigVal;						//压力值
	unsigned short	DipVal;						//油量值
}stcModPot;

typedef struct _stcCalcModel_					
{
	stcModPot			sModPot[100];					// 4*100 	内容
	unsigned short		PotNum;					// 2 		模型有效点数 
	unsigned short		StartOil;             	// 2    	模型初始油量   	
	unsigned short		ModelNum;				// 1		模型编号
	//unsigned char		valid;					// 1        使用
	unsigned short		CrcCheck;				// 2 		CrcCheck;
}stcCalcModel;

stcCalcModel	gsCalcModel;

/***********************************************
* 描述： 任务控制块（TCB）
*/


/***********************************************
* 描述： 任务堆栈（STACKS）
*/


/*******************************************************************************
* LOCAL VARIABLES
*/
static int cvi_hdbc_mysql_handle;			//数据库连接句柄
static int globalvalue;
static int db_panelHandle;

/*******************************************************************************
* GLOBAL VARIABLES
*/
void	ConnectDB(void);
void	CloseDB(void);
void	ChangeRecordValue(void);


/***********************************************
* 描述： 软定时器声明
*/
/********************************************************************************************/
/*打开数据库面板																	            */
/********************************************************************************************/
int CVICALLBACK OpenSetModelPanelCallback (int panel, int control, int event,
								  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:

			if ((db_panelHandle = LoadPanel (0, "databasesample.uir", SETM_PANEL)) < 0)
				return -1;	
	
			ConnectDB();	//连接数据库
	
			DisplayPanel (db_panelHandle);

			break;
	}
	
	return 0;
}


/*******************************************************************************
* 名    称： ConnectDB
* 功    能： 连接数据库
* 入口参数： 无
* 出口参数： 无
* 作　 　者：redmorningcn.
* 创建日期： 2017-09-05
* 修    改：
* 修改日期：
* 备    注： 和数据库建立连接。ODBC 数据源(32 位)支持，提供DNS
*******************************************************************************/
void	ConnectDB(void)
{
	//建立数据库联接，并获得联接句柄
	char	dsnbuf[64];
	sprintf(dsnbuf,"DSN=%s",ODBC_DNS);
	cvi_hdbc_mysql_handle = DBConnect (dsnbuf);
}

/*******************************************************************************
* 名    称： CloseDB
* 功    能： 连接数据库
* 入口参数： 无
* 出口参数： 无
* 作　 　者：redmorningcn.
* 创建日期： 2017-09-05
* 修    改：
* 修改日期：
* 备    注： 
*******************************************************************************/
void	CloseDB(void)
{
	//当退出应用程序时，关闭数据库连接
	DBDisconnect (cvi_hdbc_mysql_handle); 	
}


/*******************************************************************************
* 名    称： CreateDBTableMap
* 功    能： 创建数据表映射
* 入口参数： 无
* 出口参数： 无
* 作　 　者：redmorningcn.
* 创建日期： 2017-09-05
* 修    改：
* 修改日期：
* 备    注： 
*******************************************************************************/
int	CreateDBTableMap(int * hmap,stcCalcModel *sCalcModel)
{
	long	tmp64;
	int		i,j=0;
	char	tmpbuf[32];
	int 	resultcode;

	DBMapColumnToShort (*hmap, "MODEL_NUM",&sCalcModel->ModelNum,&tmp64);	//油箱模型编号
	DBMapColumnToShort (*hmap, "POT_NUM", &sCalcModel->PotNum,&tmp64);		//模型点数
	DBMapColumnToShort (*hmap, "SET_OIL", &sCalcModel->StartOil, &tmp64);	//模型初始值
	
	for(i = 0;i < 100;i++)						   
	{
		sprintf(tmpbuf,"HIG%02d",i);
		resultcode = DBMapColumnToShort (*hmap, tmpbuf, &sCalcModel->sModPot[i].HigVal, &tmp64);	//高度
		
		sprintf(tmpbuf,"OIL%02d",i);
		resultcode = DBMapColumnToShort (*hmap, tmpbuf, &sCalcModel->sModPot[i].DipVal, &tmp64);	//油量
	}	
	
	return 1;
}

/*******************************************************************************
* 名    称： CreateDBBindColCalcModel
* 功    能： 创建数据表绑定
* 入口参数： 无
* 出口参数： 无
* 作　 　者：redmorningcn.
* 创建日期： 2017-09-06
* 修    改：
* 修改日期：
* 备    注： 
*******************************************************************************/
int	CreateDBBindColCalcModel(int * hstat,stcCalcModel *sCalcModel)
{
	long	tmp64;
	int		i,j=1;

	j=1;
	
	DBBindColShort (*hstat, j++, &sCalcModel->ModelNum,&tmp64);  			//油箱模型编号 
	DBBindColShort (*hstat, j++, &sCalcModel->PotNum,&tmp64);				//模型点数  
	DBBindColShort (*hstat, j++, &sCalcModel->StartOil, &tmp64);			//模型初始值   
	
	for(i = 0;i < 100;i++)						   
	{
		DBBindColShort (*hstat, j++, &sCalcModel->sModPot[i].HigVal, &tmp64);	//高度
		
		DBBindColShort (*hstat, j++, &sCalcModel->sModPot[i].DipVal, &tmp64);	//油量
	}	

	return 1;
}

/*******************************************************************************
* 名    称： CreateDBTable
* 功    能： 创建数据表
* 入口参数： 无
* 出口参数： 无
* 作　 　者：redmorningcn.
* 创建日期： 2017-09-05
* 修    改：
* 修改日期：
* 备    注：DBBindColInt 
*******************************************************************************/
int	CreateDBTable(char *tabelname)
{
	int 	resultcode;
	int 	hmap;	
 
	hmap = DBBeginMap (cvi_hdbc_mysql_handle); 				//创建一个数据库映射

	CreateDBTableMap(&hmap,&gsCalcModel);					//创建一个数据表映射

	resultcode = DBCreateTableFromMap (hmap, tabelname);	//由此映射创建一个数据表

	DBDeactivateMap (hmap); 								//结束映射 

	return 1;
}

/*******************************************************************************
* 名    称： ReadRecFromDB
* 功    能： 从数据库表中，读取数据记录
* 入口参数： 无
* 出口参数： 无
* 作　 　者：redmorningcn.
* 创建日期： 2017-09-06
* 修    改：
* 修改日期：
* 备    注： 从数据库中读取指定的数据，并返回 
*******************************************************************************/
int	ReadRecFromDB(char *tabelname,int	keyval)
{
	int 	resultcode;
	long 	idstatus;
	int 	idvalue;
	int 	hstat;
	int 	total = 0;
	int 	i = 1;
	char	tmpbuf[64];

	DisableBreakOnLibraryErrors ();
			
	//激活SQL查询
	
	sprintf(tmpbuf,"SELECT * FROM %s WHERE MODEL_NUM = %d",tabelname,keyval);	//查找指定键值数据
	hstat = DBActivateSQL (cvi_hdbc_mysql_handle, tmpbuf);

	CreateDBBindColCalcModel(&hstat,&gsCalcModel);  							//绑定记录到相关记录的数据结构中 
	
	total = DBNumberOfRecords (hstat); 											//获得记录的总数

	if (total > 0)
	{
		//取数据记录，并打印
		//while (DBFetchNext(hstat) != DB_EOF)
		if (DBFetchNext(hstat) != DB_EOF)    
		{
			printf("模型编号：%d \r\n",gsCalcModel.ModelNum);
			printf("模型点数：%d \r\n",gsCalcModel.PotNum);
			printf("起始油量：%d \r\n",gsCalcModel.StartOil);
			
			for(i = 0;i < gsCalcModel.PotNum;i++)
			{
				printf("高度：%4d, 油量：%4d \r\n",gsCalcModel.sModPot[i].HigVal,gsCalcModel.sModPot[i].DipVal);
			}
			printf("\r\n");
			
			gsCalcModel.CrcCheck = GetCrc16Check((uint8 *)&gsCalcModel,sizeof(gsCalcModel)-2);	//计算校验	
			
			l_eqiupmentcode = RUN_MODEL_PARA;								//设置为参数模型卡
		}
	}
	
	//激活关闭查询
	DBDeactivateSQL (hstat);
	
	return 1;
}

/*******************************************************************************
* 名    称： InsertRecToDB
* 功    能： 插入数据记录
* 入口参数： 无
* 出口参数： 无
* 作　 　者：redmorningcn.
* 创建日期： 2017-09-05
* 修    改：
* 修改日期：
* 备    注： 由于数据库操作，现建立映射，在赋值改写数据，再数据入库。 
*******************************************************************************/
int CVICALLBACK InsertRecToDB (char *tabelname,stcCalcModel *sCalcModel)
{
	int 	resultcode;
	int 	hstat;
	int 	numberofrecords;
	int 	hmap;	
	
	hmap = DBBeginMap (cvi_hdbc_mysql_handle);  //创建一个数据库映射关系
	
	CreateDBTableMap(&hmap,sCalcModel);			//创建一个数据表映射

	hstat = DBActivateMap (hmap, tabelname); 	//激活映射
	
	ChangeRecordValue();	  					//修改记录值 
	
	DBCreateRecord(hstat);  					//创建记录  

	resultcode = DBPutRecord (hstat);			//更新数据库  
			
	resultcode = DBDeactivateMap (hmap);		//释放被激活的句柄 

	return 0;
}

//读取数据，即刷新数据
int CVICALLBACK readdata (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int resultcode;
	long idstatus;
	int idvalue;
	int hstat;
	int total = 0;
	int i = 1;
	switch (event)
	{
		case EVENT_COMMIT:
			
			ReadRecFromDB(TABLE_NAME,1);  	//读取数据表中的数据    

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
			
			InsertRecToDB(TABLE_NAME,&gsCalcModel);

			break;
	}
	return 0;
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

//保存到数据库时，赋值操作
void	ChangeRecordValue(void)
{
	gsCalcModel.ModelNum++;
	gsCalcModel.PotNum = 3;
	
	gsCalcModel.sModPot[0].HigVal = 0;
	gsCalcModel.sModPot[0].DipVal = 1200;
	
	gsCalcModel.sModPot[1].HigVal = 1558;
	gsCalcModel.sModPot[1].DipVal = 3400;
	
	gsCalcModel.sModPot[2].HigVal = 7529;
	gsCalcModel.sModPot[2].DipVal = 9000;
	
	for(int i = 3;i<100;i++)
	{
		gsCalcModel.sModPot[i].HigVal = 7529;
		gsCalcModel.sModPot[i].DipVal = 9000;
	}
}
