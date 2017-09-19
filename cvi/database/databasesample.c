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
* ������ ������ƿ飨TCB��
*/


/***********************************************
* ������ �����ջ��STACKS��
*/


/*******************************************************************************
* LOCAL VARIABLES
*/
int cvi_hdbc_mysql_handle = 0;			//���ݿ����Ӿ��

/*******************************************************************************
* GLOBAL VARIABLES
*/
void	ConnectDB(void);
void	CloseDB(void);
void	ChangeRecordValue(void);


//��ȡ���ݣ���ˢ������
int CVICALLBACK readdata (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	uint8 	modelnum;

	switch (event)
	{
		case EVENT_COMMIT:
			
			GetCtrlVal(panel,SETM_PANEL_MODEL_NUM, &modelnum);  //ȡҪ���õ�ģ�ͱ��
			ReadRecFromDB(TABLE_NAME,modelnum);  				//��ȡ���ݱ��е�����    

			break;
	}
	return 0;
}

//�������ݱ�
int CVICALLBACK createtable (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			
			CreateDBTable(TABLE_NAME);		//�������ݱ�
			
			SetCtrlAttribute (db_panelHandle, SETM_PANEL_CMD_READDATA, ATTR_DIMMED, 0);  
			break;
	}
	return 0;
}

//�������
int CVICALLBACK DataBaseQuitCallback (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			HidePanel (db_panelHandle);		//�������		
			CloseDB();						//�Ͽ����ݿ�
			break;
	}
	return 0;
}

//ɾ����¼
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
			
			//��û���Ԫ
			GetActiveTableCell (db_panelHandle, SETM_PANEL_TABLE, &colrow);
			
			//��û��Ԫ�������У�������Ϊ��1�е�Ԫ���ڵ�����
			GetTableCellVal (db_panelHandle, SETM_PANEL_TABLE, MakePoint(1, colrow.y), &value);
			
			//���������ݿ�ɹ�ʱ�����������
			if (cvi_hdbc_mysql_handle > 0)
	   		{
				//���ڴ����������ݿ��ѯ������׼��ִ��SQL��ѯ����
				hstat = DBPrepareSQL (cvi_hdbc_mysql_handle, "DELETE FROM table2 WHERE ID = ?");
				
				//����һ������������Ԥ����ѯ��ʽ
				resultcode = DBCreateParamInt (hstat, "", DB_PARAM_INPUT, value);
				
				//ִ�д�������SQL��ѯ				
				resultcode = DBExecutePreparedSQL (hstat);
			}
			
			DBClosePreparedSQL (hstat);
			
			//ˢ������
			readdata (panel, SETM_PANEL_CMD_READDATA, EVENT_COMMIT, NULL, 0, 0); 
			
			break;
	}
	return 0;
}

//�����¼
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

//�޸ļ�¼
int  InsertoilRecToDBTable (void)
{
	InsertoilRecToDB(OILRECORD_TABLE_NAME,&gsFlshRec);
	return 1;
}


//�޸ļ�¼
int CVICALLBACK modifydata (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	char labeltext[10];
	int comparisonresult;
	int hstat;
	int value;
	Point colrow;
	
	//����iΪ��̬�ֲ��������ڱ������ڲ�ֵ���ᶪʧ
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
			
			//���ñ��Ϊ���޸�״̬
			SetCtrlAttribute (db_panelHandle, SETM_PANEL_TABLE, ATTR_CTRL_MODE, VAL_HOT);
			
			//���³����ʾ����������޸ġ���ťʱ����ť��ǩ����Ϊ�����桱
			if (i)
			{
				SetCtrlAttribute (db_panelHandle, SETM_PANEL_CMD_MODIFY, ATTR_LABEL_TEXT, "����");
				i = 0;
			}
			else
			{
				SetCtrlAttribute (db_panelHandle, SETM_PANEL_CMD_MODIFY, ATTR_LABEL_TEXT, "�޸�"); 
				SetCtrlAttribute (db_panelHandle, SETM_PANEL_TABLE, ATTR_CTRL_MODE, VAL_INDICATOR);
				i = 1;
			}
			
			//�õ���ť�ı�ǩ�ı�
			GetCtrlAttribute (db_panelHandle, SETM_PANEL_CMD_MODIFY, ATTR_LABEL_TEXT, labeltext);
			
			//���õ����ı��롰�޸ġ����������
			comparisonresult = strcmp (labeltext, "�޸�");
			
			//���ȷ����ǩ�ı��ϵ��ַ����ǡ��޸ġ����֣���ִ�����²���
			if (comparisonresult == 0)
			{
				//����һ�����ݿ�ӳ���ϵ
				hmap = DBBeginMap (cvi_hdbc_mysql_handle);
				
				//����Ϊ�����ݿ��е�ÿ�����ݣ����ֶηŵ����Ӧ������ӳ����
				resultcode = DBMapColumnToInt (hmap, "ID", &idvalue, &idstatus);
				resultcode = DBMapColumnToChar (hmap, "NAME", 10, namevalue, &namestatus, "");
				resultcode = DBMapColumnToChar (hmap, "SEX", 10, sexvalue, &sexstatus, "");
				resultcode = DBMapColumnToInt (hmap, "AGE", &agevalue, &agestatus);
				
				//����ӳ��
				hstat = DBActivateMap (hmap, "table1");
				if (hstat > 0)
				{
					//��λ����ָ��
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
				
				//�������ݿ�
				resultcode = DBPutRecord (hstat);
				
				//�ͷű�����ľ��
				resultcode = DBDeactivateMap (hmap);				
			}
			break;
	}
	return 0;
}

//ɾ�����ݱ�
int CVICALLBACK deletetable (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int hstat;
	switch (event)
	{
		case EVENT_COMMIT:
			DisableBreakOnLibraryErrors ();
			
			//����SQL��ѯ
			hstat = DBActivateSQL (cvi_hdbc_mysql_handle, "SELECT * FROM table1");
			
			//�����������ݣ�ִ�����²���
			if (hstat > 0)
			{
				//�ͷű�������
				DBDeactivateSQL (hstat);
				
				//ɾ�����ݱ�
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
		//��������еı��ؼ���˫��ʱ���������²���
		case EVENT_LEFT_DOUBLE_CLICK:
			
			//��û���ǰ��������
			GetActiveTableCell (db_panelHandle, SETM_PANEL_TABLE, &colrow);
		
			//��ñ���һ�������ݵ�ID��
			GetTableCellVal (db_panelHandle, SETM_PANEL_TABLE, MakePoint(1, colrow.y), &value);
		
			//��ID����Ϊȫ�ֱ�������
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


