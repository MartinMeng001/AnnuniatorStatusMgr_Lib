#pragma once
// Data Structure for Status
struct AnnuniatorStatusItem
{
	char name[100];
	int id;
	char ip[25];
	int port;
	int mode;	// udp or tcp, or serial
	int timenow;
	int lasttrouble;
	int carcountPerHour;
	char online;
	char laststatus;
};
typedef std::list<AnnuniatorStatusItem> list_StatusAnnuniator;
class AnnuniatorStatus
{
private:
	static int StatusThreadFunc(void* lparam, int begin);
public:
	AnnuniatorStatus();
	~AnnuniatorStatus();
	void setStatusData(char* buffer, int length, int id);	// time, trouble, carcount, oprRecord
	void setStatus(char status, int id);	// from other operation, guard or greenband
	void setOnOff(char beOnline, int id);	// from other operation

	void insertItem(char* name, int id, char* ip, int port, int mode);
	void Clear();
	int getStatusDataAsFmt(int type);
	int StartMonitor(void);
protected:
	void getStatusDataAsJson(void);
	void getStatusDataAsOrg(void);

	void CreateThread(void);
	void SyncSystemTime();	// 
	int GetUtcTimeSB4UFmt(int nAddr, char* pszCmdBuf, int maxlen);
	int SetUtcTimeSB4UFmt(int nAddr, char* pszCmdBuf, int maxlen, CTime tm);
	void GetAnnuniatorTrouble();

	int checkExistById(int id);
	int deleteById(int id);
	int insertById(int id);
private:
	list_StatusAnnuniator	statusList;
	int maxid;
	int commandCount;
};
