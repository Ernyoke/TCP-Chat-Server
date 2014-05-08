#pragma once

class Data {
private:
	int userId;
	int roomId;

public:
	Data();
	int getUserId();
	int getRoomId();
	void setRoomId(int);
	void setUserId(int);
};