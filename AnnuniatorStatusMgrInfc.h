#pragma once
#include "AnnuniatorCommonDef.h"
//extern AnnuniatorStatus annuniatorStatus;
extern int InsertAnnuniator(char* name, int id, char* ip, int port, int mode);
extern int UpdateAnnuniatorStatus(int id, int status);
extern int StartAnnuniatorMonitor(void);
extern int getAnnuniatorStatusById(int id, struct AnnuniatorStatusItem &item);

extern int setBusyFlag(void);
extern int clearBusyFlag(void);
