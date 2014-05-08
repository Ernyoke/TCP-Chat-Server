#pragma once

#include "ChatRoom.h"

ChatRoom::ChatRoom(int id) {
	this->id = id;
	onlineUsers = 1;
	newUsers = 0;
}

ChatRoom::~ChatRoom() {

}

void ChatRoom::addUser(User* user) {
	newUsers = 1;
	users.insert(pair<int, User*>(user->getUserId(), user));
}

void ChatRoom::forward(Message* message) {
	CRITICAL_SECTION critical; 
	InitializeCriticalSection(&critical);
	if(newUsers == 1) {
		newUsers = 0;
		onlineUsers++;
	}
	for(map<int, User*>::iterator it = users.begin(); it != users.end(); ++it) {
		EnterCriticalSection(&critical);
		if((*it).second->getUserId() != message->getFrom()) {
			message->sendTo((*it).second->getSocket());
		}
		LeaveCriticalSection(&critical);
	}
}

int ChatRoom::getID() {
	return this->id;
}

map<int, User*>* ChatRoom::getUserList() {
	return &users;
}

void ChatRoom::leaveRoom(int userID) {
	CRITICAL_SECTION critical; 
	InitializeCriticalSection(&critical);
	Message m(6, userID, id, 1, "");
	for(map<int, User*>::iterator it = users.begin(); it != users.end(); ++it) {
		EnterCriticalSection(&critical);
		if((*it).first != userID) {
			m.sendTo((*it).second->getSocket());
		}
		LeaveCriticalSection(&critical);
	}
	EnterCriticalSection(&critical);
	users.erase(userID);
	onlineUsers--;
	LeaveCriticalSection(&critical);
}

void ChatRoom::inviteUser(User* user, int invID) {
	CRITICAL_SECTION critical; 
	InitializeCriticalSection(&critical);
	if(user != NULL) {
		newUsers = 1;
		EnterCriticalSection(&critical);
		char* temp = intToChar(user->getUserId());
		LeaveCriticalSection(&critical);
		int n = strlen(temp);
		char* buffer = new char[n + 1];
		strcpy(buffer, temp);
		buffer[n] = ' ';
		buffer[n + 1] = '\0';
		Message m(4, invID, id, 1, buffer);
		for(map<int, User*>::iterator it = users.begin(); it != users.end(); ++it) {
			EnterCriticalSection(&critical);
			if((*it).first != user->getUserId()) {
				m.sendTo((*it).second->getSocket());
			}
			LeaveCriticalSection(&critical);
		}
		EnterCriticalSection(&critical);
		addUser(user);
		user->assigneToRoom(this);
		LeaveCriticalSection(&critical);
	}
}

char* ChatRoom::intToChar(int value) {
	char* res = (char*)malloc(sizeof(char));
	int i = 0;
	if(value == 0) {
		res[0] = '0';
	}
	while(value > 0) {
		if(i >= 1) {
			res = (char*)realloc(res, (i + 1) * sizeof(char));
		}
		res[i] = value % 10 + 48;
		value /= 10;
		++i;
	}
	res[i] = '\0';
	return res;
}

int ChatRoom::getOnlineUsers() {
	return this->onlineUsers;
}

int ChatRoom::getNewUsers() {
	return this->newUsers;
}