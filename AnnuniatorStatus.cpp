#include "stdafx.h"
#include <chrono>
#include <thread>
#include <list>
#include <atomic>
#include "AnnuniatorCommonDef.h"
#include "tcpClientsMgrInfc.h"
#include "AnnuniatorStatus.h"

using namespace std;
using namespace chrono;
AnnuniatorStatus annuniatorStatus;
atomic_int AnnuniatorStatus::busy_flag = 0;
AnnuniatorStatus::AnnuniatorStatus()
	: AnnuniatorDataMgrUnit(), AnnuniatorQueryCmdUnit()
{
	check_end_once = true;
	thread_count = 0;
}


AnnuniatorStatus::~AnnuniatorStatus()
{
}

int AnnuniatorStatus::StatusThreadFunc(void* lparam)
{
	int idx = 0;
	AnnuniatorStatus *pAnnuStatus;
	pAnnuStatus = (AnnuniatorStatus*)lparam;
	pAnnuStatus->thread_count++;
	while (1)
	{
		if (busy_flag == 1)
		{
			chrono::seconds	delayms(1);
			this_thread::sleep_for(delayms);
			continue;
		}
		AnnuniatorStatusItem item;
		memset((char*)&item, 0, sizeof(struct AnnuniatorStatusItem));
		// checkEnd
		if (pAnnuStatus->checkCompleted4Status() == 1)
		{
			pAnnuStatus->check_end_once = true;
			break;
		}
		// getCommand
		idx = pAnnuStatus->getCmd();
		// get Annuniator by Pos
		pAnnuStatus->queryItem(item);
		// get A data
		if (item.mode == SUPPORT_MODE_TCPCLIENT_SB4U)
		{
			pAnnuStatus->QueryDataByCmd(pAnnuStatus->cmdArray[idx], item);
		}
		else continue;
		// set Status
		//chrono::milliseconds	delayms(10);
		chrono::seconds	delayms(1);
		this_thread::sleep_for(delayms);
	}
	pAnnuStatus->thread_count--;
	return 0;
}
int AnnuniatorStatus::StatusThreadMgrFunc(void* lparam)
{
	AnnuniatorStatus *pAnnuStatus;
	pAnnuStatus = (AnnuniatorStatus*)lparam;
	time_point start = system_clock::now();
	time_point end = system_clock::now();
	int beFirst = 1;
	while (1)
	{
		if ((pAnnuStatus->check_end_once == true) && (pAnnuStatus->thread_count == 0))
		{
			pAnnuStatus->check_end_once = false;
			end = system_clock::now();
			if (beFirst == 1)
			{
				beFirst = 0;
			}
			else
			{
				char msg[100];
				memset(msg, 0, sizeof(msg));
				auto duration = duration_cast<seconds>(end - start);
				sprintf(msg, "take %lld s for once checking\r\n", duration.count());
				printf("take %lld s for once checking\r\n", duration.count());
			}
			pAnnuStatus->resetCmd();
			pAnnuStatus->resetParams();
			pAnnuStatus->CreateThreadLogic();
			start = system_clock::now();
		}
		else
		{
			
		}
		chrono::seconds	delayms(1);
		this_thread::sleep_for(delayms);
	}
	return 0;
}
int AnnuniatorStatus::checkCompleted4Status(void)
{
	if (isCompleted_cmd == true && beCompleted_DataUnit == true) return 1;
	if (isCompleted_cmd == false)
	{
		if (beCompleted_DataUnit == true)
		{
			setNextCmd(0);
			resetParams();
		}
	}
	return 0;
}
int AnnuniatorStatus::StartMonitor(void)
{
	thread t(StatusThreadMgrFunc, (void*)this);
	t.detach();
	return 0;
}
int AnnuniatorStatus::ConncetAnnuniator(char *ip, int port)
{
	int idxCli = setClient(ip, port, SERVERTYPE_4U_ANNUNIATOR);
	if (idxCli == 0)return 0;
	return idxCli;
}
int AnnuniatorStatus::QueryDataByCmd(unsigned char cmd, AnnuniatorStatusItem &item)
{
	int ret = 0;
	int idxCli = ConncetAnnuniator(item.ip, item.port);
	if (idxCli == 0)
	{
		clearStatus(item.id);
		return 0;
	}
	switch (cmd)
	{
	case 0x88:
		GetAnnuniatorTime(idxCli, item.id);
		break;
	case 0x87:
		break;
	case 0xA1:
		break;
	case 0xF1:
		break;
	case 0xF3:
		break;
	}
	clearClient(idxCli);
	idxCli = 0;

	return 1;
}
int AnnuniatorStatus::GetAnnuniatorTime(int idxCli, int id)
{
	char SendBuf[100], rcvBuf[100];
	memset(SendBuf, 0, sizeof(SendBuf));
	memset(rcvBuf, 0, sizeof(rcvBuf));

	int length = GetUtcTimeSB4UFmt(SendBuf, sizeof(SendBuf));
	if (length == 0)return 0;

	if (getInfoByType(SendBuf, length, SERVERTYPE_4U_ANNUNIATOR, rcvBuf, sizeof(rcvBuf), 5, idxCli) > 0)
	{
		
		if ((unsigned char)rcvBuf[0] == 0x88)
		{
			int nYear = 2000 + (rcvBuf[1] / 16 * 10 + rcvBuf[1] % 16);
			int nMonth = (rcvBuf[2] / 16 * 10 + rcvBuf[2] % 16);
			int nDay = (rcvBuf[3] / 16 * 10 + rcvBuf[3] % 16);
			int nHour = (rcvBuf[4] / 16 * 10 + rcvBuf[4] % 16);
			int nMinute = (rcvBuf[5] / 16 * 10 + rcvBuf[5] % 16);
			int nSecond = (rcvBuf[6] / 16 * 10 + rcvBuf[6] % 16);
			if (nMonth <= 0 || nMonth > 12)nMonth = 1;
			if (nDay <= 0 || nDay > 31)nDay = 1;
			if (nHour < 0 || nHour>23)nHour = 0;
			if (nMinute < 0 || nMinute>59)nMinute = 0;
			if (nSecond < 0 || nSecond>59)nSecond = 0;
			if (checkTimeBeOK(nYear, nMonth, nDay, nHour, nMinute, nSecond) == 1)
			{
				SetAnnuniatorTime(idxCli, id);
			}
			setTime(id);
			return 1;
		}
		return 0;
	}
	clearStatus(id);
	return 0;
}
int AnnuniatorStatus::SetAnnuniatorTime(int idxCli, int id)
{
	char SendBuf[100], rcvBuf[100];
	memset(SendBuf, 0, sizeof(SendBuf));
	memset(rcvBuf, 0, sizeof(rcvBuf));

	int length = SetUtcTimeSB4UFmt(SendBuf, sizeof(SendBuf));
	if (length == 0)return 0;

	if (getInfoByType(SendBuf, length, SERVERTYPE_4U_ANNUNIATOR, rcvBuf, sizeof(rcvBuf), 5, idxCli) > 0)
	{
		if ((unsigned char)rcvBuf[0] == 0x87)
		{
			if (ChkNormalACK(rcvBuf) == 0)return 0;
			return 1;
		}
	}
	return 0;
}
int AnnuniatorStatus::GetUtcTimeSB4UFmt(char* pszCmdBuf, int maxlen)
{
	unsigned char szCmd[32];
	memset(szCmd, 0, sizeof(szCmd));

	szCmd[0] = 0x53; //S
	szCmd[1] = 0x42; //B
	szCmd[2] = 0x44; //D
	szCmd[3] = 0x5A; //Z

	szCmd[4] = 0x00; //Addr
	szCmd[5] = 0x88; //指令
	szCmd[6] = 0x00; //Year

	szCmd[7] = GetXOR(szCmd + 4, 3); //异或校验

	szCmd[8] = 0xAA; //
	szCmd[9] = 0x55; //
	if (maxlen < 10)
	{
		return 0;
	}
	else
	{
		memcpy(pszCmdBuf, szCmd, 10);
		return 10;
	}
}
int AnnuniatorStatus::SetUtcTimeSB4UFmt(char* pszCmdBuf, int maxlen)
{
	unsigned char szCmd[32];
	memset(szCmd, 0, sizeof(szCmd));

	szCmd[0] = 0x53; //S
	szCmd[1] = 0x42; //B
	szCmd[2] = 0x44; //D
	szCmd[3] = 0x5A; //Z

	time_t tmNow = time(NULL);
	tm* localTm = localtime(&tmNow);

	szCmd[4] = 0x00; //Addr
	szCmd[5] = 0x87; //指令

	int nYear = localTm->tm_year + 1900 - 2000;
	int nMonth = localTm->tm_mon + 1;
	int nDay = localTm->tm_mday;
	int nHour = localTm->tm_hour;
	int nMinute = localTm->tm_min;
	int nSecond = localTm->tm_sec;

	szCmd[6] = (nYear / 10) * 16 + nYear % 10; //Year
	szCmd[7] = (nMonth / 10) * 16 + nMonth % 10; //Month
	szCmd[8] = (nDay / 10) * 16 + nDay % 10; //Day
	szCmd[9] = (nHour / 10) * 16 + nHour % 10; //Hour
	szCmd[10] = (nMinute / 10) * 16 + nMinute % 10; //Minute
	szCmd[11] = (nSecond / 10) * 16 + nSecond % 10; //Second

	szCmd[12] = GetXOR(szCmd + 4, 8); //异或校验

	szCmd[13] = 0xAA; //
	szCmd[14] = 0x55; //

	if (maxlen < 15)
	{
		return 0;
	}
	else
	{
		memcpy(pszCmdBuf, szCmd, 15);
		return 15;
	}
}
int AnnuniatorStatus::ChkNormalACK(char* buf)
{
	if ((unsigned char)buf[1] == 0xC2) return 1;
	return 0;
}
int AnnuniatorStatus::checkTimeBeOK(int year, int month, int day, int hour, int minute, int second)
{
	//time_t tt = system_clock::to_time_t(system_clock::now());
	time_t tt = time(NULL);
	tm* localTm = localtime(&tt);
	if ((localTm->tm_year + 1900) != year) return 1;
	if ((localTm->tm_mon + 1) != month)return 1;
	if (localTm->tm_mday != day)return 1;
	int secsA = localTm->tm_hour * 60 * 60 + localTm->tm_min * 60 + localTm->tm_sec;
	int secsB = hour * 60 * 60 + minute * 60 + second;
	if (abs(secsA - secsB) > 10) return 1;
	return 0;
}
unsigned char AnnuniatorStatus::GetXOR(const unsigned char* pszBuf, int nDataLenth)
{
	unsigned uCRC = 0;//校验初始值

	for (int i = 0; i < nDataLenth; i++)
	{
		uCRC ^= pszBuf[i];
	}

	return uCRC;
}
void AnnuniatorStatus::CreateThreadLogic(void)
{
	// get total annuniators
	int nums = getTotalItems();
	CreateThread();
	//if (nums <= 6)	// single thread
	//{
	//	CreateThread();
	//}
	//else if (nums < 20)	// two threads
	//{
	//	CreateThread(); Sleep(1000);
	//	CreateThread();
	//}
	//else   // three threads
	//{
	//	CreateThread(); Sleep(1000);
	//	CreateThread(); Sleep(1000);
	//	CreateThread();
	//}
}
void AnnuniatorStatus::CreateThread(void)
{
	thread t(StatusThreadFunc, (void*)this);
	t.detach();
}
AnnuniatorDataMgrUnit::AnnuniatorDataMgrUnit()
{
	beReadable = false;	// when inserted 1 item at least, set it to true
	Clear();
	curPosIt = statusList.begin();
	beCompleted_DataUnit = false;
}
AnnuniatorDataMgrUnit::~AnnuniatorDataMgrUnit()
{

}
void AnnuniatorDataMgrUnit::Clear()
{
	list_StatusAnnuniator::iterator data_it;
	for (data_it = statusList.begin(); data_it != statusList.end();)
	{
		data_it = statusList.erase(data_it);
	}
}
void AnnuniatorDataMgrUnit::checkEnd(void)
{
	if (curPosIt == statusList.end())
	{
		beCompleted_DataUnit = true;
		curPosIt = statusList.begin();
	}
}
int AnnuniatorDataMgrUnit::resetParams(void)
{
	beCompleted_DataUnit = false;
	curPosIt = statusList.begin();
	return 0;
}
void AnnuniatorDataMgrUnit::insertItem(char* name, int id, char* ip, int port, int mode)
{
	if (strlen(name) > 90) return;
	if (strlen(ip) > 24) return;
	beReadable = false;
	AnnuniatorStatusItem newdata;
	memset((char*)&newdata, 0, sizeof(struct AnnuniatorStatusItem));
	strcpy(newdata.name, name);
	newdata.id = id;
	newdata.mode = mode;
	//if (mode == SUPPORT_MODE_TCPCLIENT_SB4U)newdata.online = 1;
	strcpy(newdata.ip, ip);
	newdata.port = port;
	statusList.push_front(newdata);
	curPosIt = statusList.begin();
	beReadable = true;
}
void AnnuniatorDataMgrUnit::UpdateItemStatus(int id, int status)
{
	beReadable = false;
	list_StatusAnnuniator::iterator data_it;
	for (data_it = statusList.begin(); data_it != statusList.end();)
	{
		if (data_it->id == id)
		{
			data_it->online = status;
			//curPosIt = statusList.erase(data_it);
			//checkEnd();
			break;
		}
		data_it++;
	}
	beReadable = true;
}
void AnnuniatorDataMgrUnit::deleteItem(int id)
{
	beReadable = false;
	list_StatusAnnuniator::iterator data_it;
	for (data_it = statusList.begin(); data_it != statusList.end();)
	{
		if (data_it->id == id)
		{
			curPosIt = statusList.erase(data_it);
			checkEnd();
			break;
		}
		data_it++;
	}
	beReadable = true;
}
int AnnuniatorDataMgrUnit::queryItem(struct AnnuniatorStatusItem& item)
{
	if (statusList.empty() == true)return 0;
	if (beReadable == false)return 0;
	beReadable = false;
	//memcpy((char*)&item, (char*)curPosIt->, sizeof(struct AnnuniatorStatusItem));
	item.id = curPosIt->id;
	strcpy(item.ip, curPosIt->ip);
	item.mode = curPosIt->mode;
	item.port = curPosIt->port;
	curPosIt++;
	checkEnd();
	beReadable = true;
	return 1;
}
int AnnuniatorDataMgrUnit::getItemById(int id, struct AnnuniatorStatusItem& item)
{
	//beReadable = false;
	list_StatusAnnuniator::iterator data_it;
	for (data_it = statusList.begin(); data_it != statusList.end();)
	{
		if (data_it->id == id)
		{
			ItemCopy(item, data_it);
			return 1;
		}
		data_it++;
	}
	//beReadable = true;
	return 0;
}
void AnnuniatorDataMgrUnit::ItemCopy(struct AnnuniatorStatusItem& dstitem, list_StatusAnnuniator::iterator srcitem)
{
	strcpy(dstitem.ip, srcitem->ip);
	strcpy(dstitem.name, srcitem->name);
	dstitem.id = srcitem->id;
	dstitem.laststatus = (char)srcitem->laststatus;
	dstitem.lasttrouble = srcitem->lasttrouble;
	dstitem.mode = srcitem->mode;
	dstitem.online = (char)srcitem->online;
	dstitem.port = srcitem->port;
	dstitem.timelastcomm = srcitem->timelastcomm;
}
void AnnuniatorDataMgrUnit::setTime(int id)
{
	if (statusList.empty() == true)return;
	beReadable = false;
	list_StatusAnnuniator::iterator data_it;
	for (data_it = statusList.begin(); data_it != statusList.end();)
	{
		if (data_it->id == id)
		{
			data_it->online = 3;
			//data_it->laststatus = 0; time don't chage status
			data_it->timelastcomm = time(NULL);
			break;
		}
		data_it++;
	}
	beReadable = true;
}
void AnnuniatorDataMgrUnit::clearStatus(int id)
{
	if (statusList.empty() == true)return;
	beReadable = false;
	list_StatusAnnuniator::iterator data_it;
	for (data_it = statusList.begin(); data_it != statusList.end();)
	{
		if (data_it->id == id)
		{
			if (data_it->online > 0)
			{
				data_it->online--;
				//data_it->laststatus = 0; time don't chage status
				if(data_it->online==0)data_it->timelastcomm = time(NULL);
			}
			break;
		}
		data_it++;
	}
	beReadable = true;
}
int AnnuniatorDataMgrUnit::getTotalItems(void)
{
	int size = statusList.size();
	return size;
}
//atomic_int AnnuniatorQueryCmdUnit::commandCnt = 0;
AnnuniatorQueryCmdUnit::AnnuniatorQueryCmdUnit()
{
	cmdArray[0] = 0x88;	// check time
	cmdArray[1] = 0xA1;	// set time if necessary
	cmdArray[2] = 0xF1;	// trouble info
	cmdArray[3] = 0xF3;	// should be modified, car count info
	cmdArray[4] = 0x00;	// trouble info
	cmdArray[5] = 0x00;	// should be modified	operation log
	commandCnt = 0;
	maxcmdNums = 4;
	isCompleted_cmd = false;
}
AnnuniatorQueryCmdUnit::~AnnuniatorQueryCmdUnit()
{

}
int AnnuniatorQueryCmdUnit::getCmd(void)
{
	int ret = commandCnt;
	//commandCnt++; 
	//if (commandCnt >= maxcmdNums)
	//{
	//	commandCnt = 0;
	//	isCompleted_cmd = true;
	//}
	return ret;
}
int AnnuniatorQueryCmdUnit::getCmdJustOnce(void)
{
	int ret = commandCnt;
	commandCnt++; if (commandCnt >= maxcmdNums)commandCnt = 0;
	return ret;
}
int AnnuniatorQueryCmdUnit::setNextCmd(int next)
{
	commandCnt++;
	if (commandCnt >= maxcmdNums)
	{
		commandCnt = 0;
		isCompleted_cmd = true;
	}
	return 1;
}

