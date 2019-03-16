#pragma once
#include <atomic>
// Data Structure for Status
#include"AnnuniatorCommonDef.h"


typedef std::list<AnnuniatorStatusItem> list_StatusAnnuniator;
class AnnuniatorDataMgrUnit
{
public:
	AnnuniatorDataMgrUnit();
	~AnnuniatorDataMgrUnit();
	// Action, insert, query, delete, params id
	void insertItem(char* name, int id, char* ip, int port, int mode); // disabled read, new insert will change the POSITION to current
	void UpdateItemStatus(int id, int status);
	int getItemById(int id, struct AnnuniatorStatusItem& item);
	void deleteItem(int id);	// disabled read
	int queryItem(struct AnnuniatorStatusItem& item);	// disabled read, read and set Next position
	int getTotalItems(void);
	void setTime(int id);
	void clearStatus(int id);
	int resetParams(void);
protected:
	void Clear();
	void checkEnd(void);
	void ItemCopy(struct AnnuniatorStatusItem& dstitem, list_StatusAnnuniator::iterator srcitem);
protected:
	bool beReadable;
	bool beCompleted_DataUnit;
	list_StatusAnnuniator::iterator curPosIt;
	list_StatusAnnuniator statusList;
};

class AnnuniatorQueryCmdUnit
{
public:
	AnnuniatorQueryCmdUnit();
	~AnnuniatorQueryCmdUnit();

	int getCmd(void);
	int getCmdJustOnce(void);
	int setNextCmd(int next);
	void resetCmd() { isCompleted_cmd = false; commandCnt = 0; }
protected:
	std::atomic_int commandCnt;
	unsigned char cmdArray[6];
	bool isCompleted_cmd;
private:
	int maxcmdNums;
};
class AnnuniatorStatus:public AnnuniatorDataMgrUnit, public AnnuniatorQueryCmdUnit
{
private:
	static int StatusThreadFunc(void* lparam);
	static int StatusThreadMgrFunc(void* lparam);
public:
	static std::atomic_int busy_flag;
public:
	AnnuniatorStatus();
	~AnnuniatorStatus();

	//int getStatusDataAsFmt(int type);
	int StartMonitor(void);
protected:
	//void getStatusDataAsJson(void);
	//void getStatusDataAsOrg(void);

	int QueryDataByCmd(unsigned char cmd, AnnuniatorStatusItem &item);
	//int GetUtcTimeSB4UFmt(char* pszCmdBuf, int maxlen);
	int ChkNormalACK(char* buf);

	int ConncetAnnuniator(char *ip, int port);
	void CreateThread(void);
	void CreateThreadLogic(void);
	int GetAnnuniatorTime(int idxCli, int id);
	int SetAnnuniatorTime(int idxCli, int id);
	int GetUtcTimeSB4UFmt(char* pszCmdBuf, int maxlen);
	int SetUtcTimeSB4UFmt(char* pszCmdBuf, int maxlen);

	unsigned char GetXOR(const unsigned char* pszBuf, int nDataLenth);
	int checkTimeBeOK(int year, int month, int day, int hour, int minute, int second);
	int checkCompleted4Status(void);
private:
	bool check_end_once;
	std::atomic_int thread_count;
};


