#pragma once
#define MAXLENGTH_SB4U_CMD_STATUS		100
struct StatusPackage
{
	int id;
	long rcvTime;
	short length;
	char rcvData[MAXLENGTH_SB4U_CMD_STATUS];
};
typedef std::list<StatusPackage> list_StatusData;
class StatusBuffer
{
public:
	StatusBuffer();
	~StatusBuffer();

	int getStatusById(int id, char* buffer, int len);
	void setStatus(int id, char* data, int len);
	void Clear();
	void ClearTimeOutPackage();	// 10 minute for the data max time interval
protected:
	list_StatusData	statusList;
	int timeout;
	bool beWriting;
};

