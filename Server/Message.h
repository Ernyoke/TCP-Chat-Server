#pragma once

#include "messagestruct.h"
#include <iostream>
#include "winsock2.h"

using namespace std;

class Message
{
public:
    Message(int, int, int, int, char*);
	Message();

    int getType();
    int getDest();
	int getFrom();
    void setDest();
    void setType();
    void createMessage(char*, int);
    char* getBuffer();
	void sendTo(SOCKET*);

private:
	MessageStruct msg;
    
};
