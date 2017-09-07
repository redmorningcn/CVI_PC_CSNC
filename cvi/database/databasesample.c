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
//ODBC ����Դ(32 λ),DSN
#define		ODBC_DNS	"MySql32"
#define		TABLE_NAME	"oilmodeltest"


/*******************************************************************************
* TYPEDEFS
*/

typedef struct _stcModPot_						//IC ��cd����
{
	unsigned short	HigVal;						//ѹ��ֵ
	unsigned short	DipVal;						//����ֵ
}stcModPot;

typedef struct _stcCalcModel_					
{
	stcModPot			sModPot[100];					// 4*100 	����
	unsigned short		PotNum;					// 2 		ģ����Ч���� 
	unsigned short		StartOil;             	// 2    	ģ�ͳ�ʼ����   	
	unsigned short		ModelNum;				// 1		ģ�ͱ��
	//unsigned char		valid;					// 1        ʹ��
	unsigned short		CrcCheck;				// 2 		CrcCheck;
}stcCalcModel;

stcCalcModel	gsCalcModel;

/***********************************************
* ������ ������ƿ飨TCB��
*/


/***********************************************
* ������ �����ջ��STACKS��
*/


/*******************************************************************************
* LOCAL VARIABLES
*/
static int cvi_hdbc_mysql_handle;			//���ݿ����Ӿ��
static int globalvalue;
static int db_panelHandle;

/*******************************************************************************
* GLOBAL VARIABLES
*/
void	ConnectDB(void);
void	CloseDB(void);
void	ChangeRecordValue(void);


/***********************************************
* ������ ��ʱ������
*/
/********************************************************************************************/
/*�����ݿ����																	            */
/********************************************************************************************/
int CVICALLBACK OpenSetModelPanelCallback (int panel, int control, int event,
								  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:

			if ((db_panelHandle = LoadPanel (0, "databasesample.uir", SETM_PANEL)) < 0)
				return -1;	
	
			ConnectDB();	//�������ݿ�
	
			DisplayPanel (db_panelHandle);

			break;
	}
	
	return 0;
}


/*******************************************************************************
* ��    �ƣ� ConnectDB
* ��    �ܣ� �������ݿ�
* ��ڲ����� ��
* ���ڲ����� ��
* ���� ���ߣ�redmorningcn.
* �������ڣ� 2017-09-05
* ��    �ģ�
* �޸����ڣ�
* ��    ע�� �����ݿ⽨�����ӡ�ODBC ����Դ(32 λ)֧�֣��ṩDNS
*******************************************************************************/
void	ConnectDB(void)
{
	//�������ݿ����ӣ���������Ӿ��
	char	dsnbuf[64];
	sprintf(dsnbuf,"DSN=%s",ODBC_DNS);
	cvi_hdbc_mysql_handle = DBConnect (dsnbuf);
}

/*******************************************************************************
* ��    �ƣ� CloseDB
* ��    �ܣ� �������ݿ�
* ��ڲ����� ��
* ���ڲ����� ��
* ���� ���ߣ�redmorningcn.
* �������ڣ� 2017-09-05
* ��    �ģ�
* �޸����ڣ�
* ��    ע�� 
*******************************************************************************/
void	CloseDB(void)
{
	//���˳�Ӧ�ó���ʱ���ر����ݿ�����
	DBDisconnect (cvi_hdbc_mysql_handle); 	
}


/*******************************************************************************
* ��    �ƣ� CreateDBTableMap
* ��    �ܣ� �������ݱ�ӳ��
* ��ڲ����� ��
* ���ڲ����� ��
* ���� ���ߣ�redmorningcn.
* �������ڣ� 2017-09-05
* ��    �ģ�
* �޸����ڣ�
* ��    ע�� 
*******************************************************************************/
int	CreateDBTableMap(int * hmap,stcCalcModel *sCalcModel)
{
	long	tmp64;
	int		i,j=0;
	char	tmpbuf[32];
	int 	resultcode;

	DBMapColumnToShort (*hmap, "MODEL_NUM",&sCalcModel->ModelNum,&tmp64);	//����ģ�ͱ��
	DBMapColumnToShort (*hmap, "POT_NUM", &sCalcModel->PotNum,&tmp64);		//ģ�͵���
	DBMapColumnToShort (*hmap, "SET_OIL", &sCalcModel->StartOil, &tmp64);	//ģ�ͳ�ʼֵ
	
	for(i = 0;i < 100;i++)						   
	{
		sprintf(tmpbuf,"HIG%02d",i);
		resultcode = DBMapColumnToShort (*hmap, tmpbuf, &sCalcModel->sModPot[i].HigVal, &tmp64);	//�߶�
		
		sprintf(tmpbuf,"OIL%02d",i);
		resultcode = DBMapColumnToShort (*hmap, tmpbuf, &sCalcModel->sModPot[i].DipVal, &tmp64);	//����
	}	
	
	return 1;
}

/*******************************************************************************
* ��    �ƣ� CreateDBBindColCalcModel
* ��    �ܣ� �������ݱ��
* ��ڲ����� ��
* ���ڲ����� ��
* ���� ���ߣ�redmorningcn.
* �������ڣ� 2017-09-06
* ��    �ģ�
* �޸����ڣ�
* ��    ע�� 
*******************************************************************************/
int	CreateDBBindColCalcModel(int * hstat,stcCalcModel *sCalcModel)
{
	long	tmp64;
	int		i,j=1;

	j=1;
	
	DBBindColShort (*hstat, j++, &sCalcModel->ModelNum,&tmp64);  			//����ģ�ͱ�� 
	DBBindColShort (*hstat, j++, &sCalcModel->PotNum,&tmp64);				//ģ�͵���  
	DBBindColShort (*hstat, j++, &sCalcModel->StartOil, &tmp64);			//ģ�ͳ�ʼֵ   
	
	for(i = 0;i < 100;i++)						   
	{
		DBBindColShort (*hstat, j++, &sCalcModel->sModPot[i].HigVal, &tmp64);	//�߶�
		
		DBBindColShort (*hstat, j++, &sCalcModel->sModPot[i].DipVal, &tmp64);	//����
	}	

	return 1;
}

/*******************************************************************************
* ��    �ƣ� CreateDBTable
* ��    �ܣ� �������ݱ�
* ��ڲ����� ��
* ���ڲ����� ��
* ���� ���ߣ�redmorningcn.
* �������ڣ� 2017-09-05
* ��    �ģ�
* �޸����ڣ�
* ��    ע��DBBindColInt 
*******************************************************************************/
int	CreateDBTable(char *tabelname)
{
	int 	resultcode;
	int 	hmap;	
 
	hmap = DBBeginMap (cvi_hdbc_mysql_handle); 				//����һ�����ݿ�ӳ��

	CreateDBTableMap(&hmap,&gsCalcModel);					//����һ�����ݱ�ӳ��

	resultcode = DBCreateTableFromMap (hmap, tabelname);	//�ɴ�ӳ�䴴��һ�����ݱ�

	DBDeactivateMap (hmap); 								//����ӳ�� 

	return 1;
}

/*******************************************************************************
* ��    �ƣ� ReadRecFromDB
* ��    �ܣ� �����ݿ���У���ȡ���ݼ�¼
* ��ڲ����� ��
* ���ڲ����� ��
* ���� ���ߣ�redmorningcn.
* �������ڣ� 2017-09-06
* ��    �ģ�
* �޸����ڣ�
* ��    ע�� �����ݿ��ж�ȡָ�������ݣ������� 
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
			
	//����SQL��ѯ
	
	sprintf(tmpbuf,"SELECT * FROM %s WHERE MODEL_NUM = %d",tabelname,keyval);	//����ָ����ֵ����
	hstat = DBActivateSQL (cvi_hdbc_mysql_handle, tmpbuf);

	CreateDBBindColCalcModel(&hstat,&gsCalcModel);  							//�󶨼�¼����ؼ�¼�����ݽṹ�� 
	
	total = DBNumberOfRecords (hstat); 											//��ü�¼������

	if (total > 0)
	{
		//ȡ���ݼ�¼������ӡ
		//while (DBFetchNext(hstat) != DB_EOF)
		if (DBFetchNext(hstat) != DB_EOF)    
		{
			printf("ģ�ͱ�ţ�%d \r\n",gsCalcModel.ModelNum);
			printf("ģ�͵�����%d \r\n",gsCalcModel.PotNum);
			printf("��ʼ������%d \r\n",gsCalcModel.StartOil);
			
			for(i = 0;i < gsCalcModel.PotNum;i++)
			{
				printf("�߶ȣ�%4d, ������%4d \r\n",gsCalcModel.sModPot[i].HigVal,gsCalcModel.sModPot[i].DipVal);
			}
			printf("\r\n");
			
			gsCalcModel.CrcCheck = GetCrc16Check((uint8 *)&gsCalcModel,sizeof(gsCalcModel)-2);	//����У��	
			
			l_eqiupmentcode = RUN_MODEL_PARA;								//����Ϊ����ģ�Ϳ�
		}
	}
	
	//����رղ�ѯ
	DBDeactivateSQL (hstat);
	
	return 1;
}

/*******************************************************************************
* ��    �ƣ� InsertRecToDB
* ��    �ܣ� �������ݼ�¼
* ��ڲ����� ��
* ���ڲ����� ��
* ���� ���ߣ�redmorningcn.
* �������ڣ� 2017-09-05
* ��    �ģ�
* �޸����ڣ�
* ��    ע�� �������ݿ�������ֽ���ӳ�䣬�ڸ�ֵ��д���ݣ���������⡣ 
*******************************************************************************/
int CVICALLBACK InsertRecToDB (char *tabelname,stcCalcModel *sCalcModel)
{
	int 	resultcode;
	int 	hstat;
	int 	numberofrecords;
	int 	hmap;	
	
	hmap = DBBeginMap (cvi_hdbc_mysql_handle);  //����һ�����ݿ�ӳ���ϵ
	
	CreateDBTableMap(&hmap,sCalcModel);			//����һ�����ݱ�ӳ��

	hstat = DBActivateMap (hmap, tabelname); 	//����ӳ��
	
	ChangeRecordValue();	  					//�޸ļ�¼ֵ 
	
	DBCreateRecord(hstat);  					//������¼  

	resultcode = DBPutRecord (hstat);			//�������ݿ�  
			
	resultcode = DBDeactivateMap (hmap);		//�ͷű�����ľ�� 

	return 0;
}

//��ȡ���ݣ���ˢ������
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
			
			ReadRecFromDB(TABLE_NAME,1);  	//��ȡ���ݱ��е�����    

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
			
			InsertRecToDB(TABLE_NAME,&gsCalcModel);

			break;
	}
	return 0;
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

//���浽���ݿ�ʱ����ֵ����
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
