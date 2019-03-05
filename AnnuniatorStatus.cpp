#include "stdafx.h"
#include <chrono>
#include <thread>
#include "tcpClientsMgrInfc.h"
#include "AnnuniatorStatus.h"

using namespace std;
using namespace chrono;

AnnuniatorStatus::AnnuniatorStatus()
{
}


AnnuniatorStatus::~AnnuniatorStatus()
{
}

int AnnuniatorStatus::StatusThreadFunc(void* lparam, int begin)
{
	int beginPos = begin;
	AnnuniatorStatus *pAnnuStatus;
	pAnnuStatus = (AnnuniatorStatus*)lparam;
	int checkPos = beginPos;
	while (1)
	{
		// getCommand
		// get Annuniator by Pos
		// get A data
		// set Status
		chrono::seconds	delayms(6);
		this_thread::sleep_for(delayms);
	}
}
