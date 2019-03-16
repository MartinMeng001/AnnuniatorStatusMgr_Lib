#pragma once
#define SUPPORT_MODE_TCPCLIENT_SB4U	1

struct AnnuniatorStatusItem
{
	int id;	
	int port;
	int mode;	// udp or tcp, or serial	
	int lasttrouble;
	char online;
	char laststatus;
	char ip[26];
	char name[100];
	long long timelastcomm;
};