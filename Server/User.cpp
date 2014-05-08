#pragma once
#include <iostream>
#include "User.h"
#include <math.h>
#include "ChatRoom.h"
#include <string>
//#include <conio.h>

using namespace std;
User::User(SOCKET sck, int id) {
	this->AcceptSocket = sck;
	this->userID = id;
}

User::~User() {
	for(map<int, ChatRoom*>::iterator it = myRooms.begin(); it != myRooms.end(); ++it) {
		(*it).second->leaveRoom(userID);
	}
	users->erase(userID);
	cout << "destructor";
}

void User::run() {
	int bufSize;
	int type;
	char *userIDchar = intToChar(userID);
	Message m(1, 0, userID, 1, userIDchar);
	m.sendTo(&AcceptSocket);
	int iResult = 1;
	cout << userName << "connected!" << endl;
	while(iResult > 0) {
		char recBuf[4];
		iResult = recv(AcceptSocket, recBuf, sizeof(int), MSG_WAITALL);
		int recSize;
		memcpy(&recSize, recBuf, sizeof(int));
		char* rec = new char[recSize];
		int x = recv(AcceptSocket, rec, recSize, MSG_WAITALL);
		Message message;
		message.createMessage(rec, recSize);
		switch(message.getType()) {
		case 2: 
			{
				//send mainlist
				char* myUserName = stringToChar(userName);
				char* myID = intToChar(userID);
				char* loginBuffer = new char[8000];
				strcpy(loginBuffer, myID);
				int csize = strlen(loginBuffer);
				loginBuffer[csize++] = 1;
				strcpy(loginBuffer + csize, myUserName);
				csize += strlen(myUserName);
				loginBuffer[csize++] = 1;
				loginBuffer[csize] = '\0';
				Message login(2, userID, 0, 1, loginBuffer);

			//get userlist
			int currentSize = 0;
			char *sendBuf = (char*)malloc(8000 * sizeof(char));
			//set message type
			for(map<int, User*>::iterator it = users->begin(); it != users->end(); ++it) {
				if((*it).first != userID) {
					login.sendTo((*it).second->getSocket());
				}
				char* username = stringToChar((*it).second->getUserName());
				int userNameSize = strlen(username);
				if(currentSize + userNameSize > 8000) {
					//send
					Message m(2, 0, userID, 0, sendBuf);
					currentSize = 0;
				}
				strcpy(sendBuf + currentSize, intToChar((*it).first));
				currentSize = strlen(sendBuf);
				sendBuf[currentSize++] = 1;
				strcpy(sendBuf + currentSize, stringToChar((*it).second->getUserName()));
				currentSize = currentSize + userNameSize;
				sendBuf[currentSize++] = 1;
			}
			sendBuf[currentSize] = '\0';
			Message m(2, 0, userID, 0, sendBuf);
			m.sendTo(&AcceptSocket);
			currentSize = 0;
			break;
			}
		case 3:
			{
				//new chatroom
				int rId = data->getRoomId();
				ChatRoom *room = new ChatRoom(rId);
				data->setRoomId(rId);
				room->addUser(this);
				rooms->insert(pair<int, ChatRoom*>(rId, room));
				int x = charToInt(message.getBuffer());
				map<int, User*>::iterator it = users->find(x);
				User *u = (*it).second;
				room->addUser(u);
				u->assigneToRoom(room);
				this->assigneToRoom(room);
				Message createRoom(3, userID, rId, 1, "");
				createRoom.sendTo(&AcceptSocket);
				break;
			}
		case 4: 
			{
				//get a chatroom's userlist
				map<int, ChatRoom*>::iterator it;
				it = rooms->find(message.getDest());
				ChatRoom* r = (*it).second;
				map<int, User*>* ulist = r->getUserList();
				char *sendBuf = (char*)malloc(8000 * sizeof(char));
				int cLength = 0;
				for(map<int, User*>::iterator i = ulist->begin(); i != ulist->end(); ++i) {
					int id = (*i).first;
					char* idch = intToChar(id);
					strcpy(sendBuf + cLength, idch);
					cLength += strlen(idch);
					strcpy(sendBuf + cLength, " ");
					cLength++;
				}
				sendBuf[cLength] = '\0';
				Message ul(4, 0, message.getDest(), 1, sendBuf);
				ul.sendTo(&AcceptSocket);
				break;
			}
		case 5:
			{
				//forward message
				map<int, ChatRoom*>::iterator it;
				it = rooms->find(message.getDest());
				ChatRoom* r = (*it).second;
				r->forward(&message);
				break;
			}
		case 6: 
			{
				//invite user to a conversation
				map<int, ChatRoom*>::iterator it;
				it = rooms->find(message.getDest());
				ChatRoom* r = (*it).second;
				map<int, User*>::iterator it2;
				it2 = users->find(charToInt(message.getBuffer()));
				User *u = (*it2).second;
				r->inviteUser(u, message.getFrom());
				break;

			}
		case 7: 
			{
				//leave chatroom;
				map<int, ChatRoom*>::iterator it;
				it = rooms->find(message.getDest());
				ChatRoom* r = (*it).second;
				r->leaveRoom(userID);
				CRITICAL_SECTION critical; 
				InitializeCriticalSection(&critical); 
				EnterCriticalSection(&critical);
				myRooms.erase(message.getDest());
				LeaveCriticalSection(&critical);
				if(r->getOnlineUsers() == 0) {
					delete r;
				}
				break;
			}
		case 9: 
			{
				//initiate file transfer
				map<int, ChatRoom*>::iterator it;
				it = rooms->find(message.getDest());
				ChatRoom* r = (*it).second;
				r->forward(&message);
				break;
			}
		case 10: 
			{

				//accept file 
				map<int, ChatRoom*>::iterator it;
				it = rooms->find(message.getDest());
				ChatRoom* r = (*it).second;
				r->forward(&message);
				break;
			}
		case 11: 
			{
				//denie file transfer / stop file transfer
				map<int, ChatRoom*>::iterator it;
				it = rooms->find(message.getDest());
				ChatRoom* r = (*it).second;
				r->forward(&message);
				break;
			}
		case 12:
			{
				//transfering file
				map<int, ChatRoom*>::iterator it;
				it = rooms->find(message.getDest());
				ChatRoom* r = (*it).second;
				r->forward(&message);
				break;
			}
		}
	}
	cout << userName << " loged out!" << endl;
	for(map<int, ChatRoom*>::iterator it = myRooms.begin(); it != myRooms.end(); ++it) {
		if((*it).second->getID() != 0) {
			(*it).second->leaveRoom(userID);
		}
	}
	Message logout(8, userID, 0, 1, "");
	for(map<int, User*>::iterator it = users->begin(); it != users->end(); ++it) {
		logout.sendTo((*it).second->getSocket());
	}
	CRITICAL_SECTION critical; 
	InitializeCriticalSection(&critical); 
	EnterCriticalSection(&critical);
	users->erase(userID);
	LeaveCriticalSection(&critical);
}

void User::setUserName(string name) {
	this->userName = name;
}

int User::getUserId() {
	return userID;
}

void User::setUserId(int id) {
	this->userID = id;
}

SOCKET* User::getSocket() {
	return &AcceptSocket;
}

void User::setRooms(ChatRoom* room) {
	rooms->insert(pair<int, ChatRoom*>(room->getID(), room));
	//ChatRoom *asd = room;
}

char* User::intToChar(int value) {
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

char* User::stringToChar(string& username) {
	char *name = new char[username.size()];
	strcpy(name, username.c_str());
	return name;
}

int User::charToInt(char* x) {
	int res = 0;
	int n = strlen(x);
	int mult = pow(10, n - 1);
	for(int i = 0; i < n; ++i) {
		res = res * mult + x[i] - 48;
		mult = mult / 10;
	}
	return res;
}

string User::getUserName() {
	return userName;
}

void User::setUserList(map<int, User*>* users) {
	this->users = users;
}

void User::setData(Data *data) {
	this->data = data;
}

void User::assigneToRoom(ChatRoom* r) {
	myRooms.insert(pair<int, ChatRoom*>(r->getID(), r));
}

void User::setRoomList(map<int, ChatRoom*>* rooms) {
	this->rooms = rooms;
}
