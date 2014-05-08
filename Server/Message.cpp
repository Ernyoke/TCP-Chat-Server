#pragma once
#include "message.h"

Message::Message(int type, int from, int dest, int isLast, char* buffer) {
	msg.type = type;
	msg.from = from;
	msg.dest = dest;
	msg.islast = isLast;
	strcpy(msg.data, buffer);
}

Message::Message() {
}

void Message::createMessage(char *buffer, int size){
    memcpy(&msg, buffer, size);
    //this->buffer = new char[strlen(msg.data)];
    //strcpy(this->buffer, msg.data);
}

void Message::sendTo(SOCKET* socket) {
    void* myPtr = &msg;
    int msize = sizeof(msg);
    void* asd = &msize;
    char* ssize = (char*)asd;
    char *data = (char*)myPtr;
    char* sendBuf = new char[sizeof(int) + sizeof(msg)];
    memcpy(sendBuf, ssize, sizeof(int));
    memcpy(sendBuf + sizeof(int), data, sizeof(msg));
	send(*socket, sendBuf, msize + sizeof(msize), 0 );
}

char* Message::getBuffer() {
	return msg.data;
}

int Message::getType() {
	return msg.type;
}

int Message::getDest() {
	return msg.dest;
}

int Message::getFrom() {
	return msg.from;
}