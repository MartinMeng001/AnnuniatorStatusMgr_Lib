#include "stdafx.h"
#include <list>
#include <time.h>
#include <chrono>
#include <thread>
#include "StatusBuffer.h"

using namespace std;
using namespace chrono;

long GetCurrentTimeByMS()
{
	auto nowtime = chrono::system_clock::now();
	auto nowtimems = duration_cast<milliseconds>(nowtime.time_since_epoch());
	//milliseconds nowtimems(nowtime.time_since_epoch);

	return long(nowtimems.count());
}
long GetCurrentTimeBySec()
{
	//long nowtime = time(NULL);
	//return nowtime;
	auto nowtime = chrono::system_clock::now();
	auto nowtimesec = duration_cast<seconds>(nowtime.time_since_epoch());
	return long(nowtimesec.count());
}
StatusBuffer::StatusBuffer()
{
	Clear();
	timeout = 2;
	beWriting = false;
}


StatusBuffer::~StatusBuffer()
{
}

void StatusBuffer::Clear()
{
	list_StatusData::iterator data_it;
	for (data_it = statusList.begin(); data_it != statusList.end();)
	{
		data_it = statusList.erase(data_it);
	}
}

void StatusBuffer::ClearTimeOutPackage()
{
	long currTime = GetCurrentTimeBySec();
	list_StatusData::iterator data_it;
	for (data_it = statusList.begin(); data_it != statusList.end();)
	{
		if ((currTime - data_it->rcvTime) > timeout)
		{
			data_it = statusList.erase(data_it);
		}
		else data_it++;
	}
}

int StatusBuffer::getStatusById(int id, char* buffer, int len)
{
	int ret = 0;
	if (beWriting == true) return 0;
	ClearTimeOutPackage();
	memset(buffer, 0, len);
	list_StatusData::iterator data_it;
	for (data_it = statusList.begin(); data_it != statusList.end();)
	{
		if (data_it->id == id)
		{
			if (data_it->length > len)
			{
				ret = 0;
				break;
			}
			ret = data_it->length;
			memcpy(buffer, data_it->rcvData, data_it->length);
			data_it = statusList.erase(data_it);
			break;
		}
		else data_it++;
	}
	return ret;
}
void StatusBuffer::setStatus(int id, char* data, int len)
{
	beWriting = true;
	if (len > MAXLENGTH_SB4U_CMD_STATUS) return;
	StatusPackage newdata;
	memset(newdata.rcvData, 0, sizeof(newdata.rcvData));
	memcpy(newdata.rcvData, data, len);
	newdata.length = len;
	newdata.id = id;
	newdata.rcvTime = GetCurrentTimeBySec();
	statusList.push_front(newdata);
	ClearTimeOutPackage();
	beWriting = false;
}
