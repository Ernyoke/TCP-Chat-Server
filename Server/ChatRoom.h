#pragma once
#include <map>
#include "Message.h"
#include "User.h"

class ChatRoom {
private:
	map<int, User*> users;
	int newUsers;
	int onlineUsers;
	int id;

	char* intToChar(int);

public:
	ChatRoom(map<int, User>*, int);
	ChatRoom(int);
	~ChatRoom();
	void addUser(User*);
	void forward(Message*);
	void leaveRoom(int);
	void inviteUser(User*, int);
	map<int, User*>* getUserList();
	int getID();
	int getOnlineUsers();
	int getNewUsers();
};
