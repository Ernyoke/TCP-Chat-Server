#pragma once

#include "Data.h"

Data::Data() {
	this->userId = 0;
	this->roomId = 0;
}

int Data::getUserId() {
	return userId + 1;
}

int Data::getRoomId() {
	return roomId + 1;
}

void Data::setUserId(int id) {
	this->userId = id;
}

void Data::setRoomId(int id) {
	this->roomId = id;
}