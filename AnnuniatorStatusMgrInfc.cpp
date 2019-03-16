#include "stdafx.h"
#include <chrono>
#include <thread>
#include <list>
#include <atomic>
#include "AnnuniatorStatus.h"
#include "AnnuniatorStatusMgrInfc.h"
using namespace std;
extern AnnuniatorStatus annuniatorStatus;
int InsertAnnuniator(char* name, int id, char* ip, int port, int mode)
{
	annuniatorStatus.insertItem(name, id, ip, port, mode);
	return 0;
}
int UpdateAnnuniatorStatus(int id, int status)
{
	annuniatorStatus.UpdateItemStatus(id, status);
	return 0;
}
int setBusyFlag(void)
{
	AnnuniatorStatus::busy_flag = 1;
	return 0;
}
int clearBusyFlag(void)
{
	AnnuniatorStatus::busy_flag = 0;
	return 0;
}
int StartAnnuniatorMonitor(void)
{
	annuniatorStatus.StartMonitor();
	return 0;
}

int getAnnuniatorStatusById(int id, struct AnnuniatorStatusItem &item)
{
	return annuniatorStatus.getItemById(id, item);
}