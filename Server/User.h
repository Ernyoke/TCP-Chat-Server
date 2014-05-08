#pragma once
#include "SysThread.h"
#include <map>
#include <vector>
#include <iostream>
#include "MessageStruct.h"
#include "Message.h"
#include "Data.h"

using namespace std;
class ChatRoom;
class User : public SysThread {
private:
	SOCKET AcceptSocket;
	map<int, User*>* users;
	map<int, ChatRoom*>* rooms;
	map<int, ChatRoom*> myRooms;
	int bufLen;
	int userID;
	string userName;
	Data *data;

	char* intToChar(int);
	char* stringToChar(string&);
	int charToInt(char*);

public:
	User(SOCKET, int);
	~User();
	void run();
	void setUserList(map<int, User*>*);
	void setUserName(string);
	void setUserId(int);
	string getUserName();
	int getUserId();
	SOCKET* getSocket();
	void setRooms(ChatRoom*);
	void setRoomList(map<int, ChatRoom*>*);
	void setData(Data*);
	void assigneToRoom(ChatRoom*);

};
