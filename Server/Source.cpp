#include "winsock2.h"
#include <map>
#include <iostream>
#include "User.h"
#include "ChatRoom.h"
#include "Data.h"
#include "Message.h"

using namespace std;

char RecvBuf[1024], SendBuf[1024];
int BufLen;

int main() {
	map<int, User*> users;
	map<int, ChatRoom*> rooms;
	Data data;
	// Initialize Winsock. 
		WSACleanup();
	WSADATA wsaData; 
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData); 
	if (iResult != NO_ERROR) { 
		printf("Error at WSAStartup()\n"); 
	} 

	//---------------------- 
	// Create a SOCKET for listening for 
	// incoming connection requests. 
	BufLen = strlen(RecvBuf);
	SOCKET ListenSocket, SendSocket;
	ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
	SendSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
	if (ListenSocket == INVALID_SOCKET) { 
		cout << "Error at socket(): %ld\n" << WSAGetLastError() << endl; 
		WSACleanup(); 
		return 1; 
	} 

	//---------------------- 
	// The sockaddr_in structure specifies the address family, 
	// IP address, and port for the socket that is being bound. 
	sockaddr_in service; 
	service.sin_family = AF_INET; 
	service.sin_addr.s_addr = INADDR_ANY; 
	service.sin_port = htons(12000); 

	ChatRoom room(0);
	rooms.insert(pair<int, ChatRoom*>(0, &room));

	if (bind( ListenSocket, (SOCKADDR*) &service, sizeof(service)) == SOCKET_ERROR) { 
			printf("bind() failed.\n"); 
			closesocket(ListenSocket); 
			WSACleanup(); 
			return 1; 
	} 

	//---------------------- 
	// Listen for incoming connection requests. 
	// on the created socket 
	if (listen( ListenSocket, 1 ) == SOCKET_ERROR) { 
		cout << "Error listening on socket.\n"; 
		closesocket(ListenSocket); 
		WSACleanup(); 
		return 1; 
	} 

	//---------------------- 
	// Create a SOCKET for accepting incoming requests. 
	SOCKET AcceptSocket;
	char recBuf[4];
	int recSize;
	bool exists = false;
	cout << "Waiting for client to connect...\n"; 

	//---------------------- 
	// Accept the connection. 
	while(true) {
		AcceptSocket = accept( ListenSocket, NULL, NULL ); 
		if (AcceptSocket == INVALID_SOCKET) {
			cout << "accept failed: %d\n" <<  WSAGetLastError() << endl; 
			closesocket(ListenSocket); 
			WSACleanup(); 
			return 1; 
		} else 
			cout << "Client connected.\n"; 
		iResult = recv(AcceptSocket, recBuf, sizeof(int), MSG_WAITALL);
		memcpy(&recSize, recBuf, sizeof(int));
		char* rec = new char[recSize];
		int x = recv(AcceptSocket, rec, recSize, MSG_WAITALL);
		Message message;
		message.createMessage(rec, recSize);
		string uname = message.getBuffer();
		User *u;
		CRITICAL_SECTION critical; 
		InitializeCriticalSection(&critical); 
		EnterCriticalSection(&critical);
		exists = false;
		for(map<int, User*>::iterator it = users.begin(); it != users.end(); ++it) {
			User* user = (*it).second;
			if(user->getUserName().compare(uname) == 0) {
				exists = true;
				break;
			}
		}
		if(!exists) {
			int userID = data.getUserId();
			u = new User(AcceptSocket, userID);
			users.insert(pair<int, User*>(userID, u));
			data.setUserId(userID);
			u->setUserId(userID);
			room.addUser(u);
			u->assigneToRoom(&room);
			u->setRoomList(&rooms);
			u->setUserList(&users);
			u->setData(&data);
			string uname = message.getBuffer();
			u->setUserName(uname);
		}
		LeaveCriticalSection(&critical);
		if(!exists) {
			Message m(1, 0, u->getUserId(), 1, "OK");
			m.sendTo(u->getSocket());
			u->start();
		}
		else {
			Message m(1, 0, 0, 1, "ERROR");
			m.sendTo(&AcceptSocket);
		}
	}

	//--------------------------------------------- 
	// Clean up and quit. 
	printf("Exiting.\n"); 
	WSACleanup(); 
	return 0; 
}