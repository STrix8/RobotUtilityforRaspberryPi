#include <cassert>
#include <chrono>
#include <cstring>
#include <fstream>
#include <thread>
#include <vector>
#include "Ds3Read.hpp"

using namespace std;
using namespace rbutil;

Ds3Read::Ds3Read() {
	init("/dev/input/js0", false, 0);
}

Ds3Read::Ds3Read(bool precision, int timeout) {
	init("/dev/input/js0", precision, timeout);
}
Ds3Read::Ds3Read(const char* fileName, bool precision, int timeout) {
	init(fileName, precision, timeout);
}

void Ds3Read::init(const char* fileName, bool precision, int timeout) {
	precisionFlag = precision;
	loopFlag = false;
	for (int i = 0; i < NumButtons; ++i) {
		readButtonData[i] = false;
		buttonData[i] = false;
	} 
	for (int i = 0; i < NumSticks; ++i) {
		readStickData[i] = false;
		stickData[i] = false;
	}
	auto startTime = chrono::system_clock::now();
	connectedFlag = false;
	for (;;) {
		if (timeout) {
			if (chrono::time_point<chrono::system_clock>(startTime + chrono::seconds(timeout)) < chrono::system_clock::now()) {
				break;
			}
		}
		try {
			JoyStick.open(fileName);
			if (JoyStick.is_open()) {
				connectedFlag = true;
				break;
			}
		}
		catch (ifstream::failure error) {
			continue;
		}
	}
	yReverse = false;
	if (connectedFlag) {
		loopFlag = true;
		readThread = thread([&]{ readLoop(); });
	}
}

bool Ds3Read::isConnected() {
	return connectedFlag;
}

void Ds3Read::precisionMode(bool precision) {
	precisionFlag = precision;
}

void Ds3Read::read() {
	vector<char> data;
	char c;

	while (data.size() != 8) {
		c = JoyStick.get();
		data.push_back(c);
	}
	if (data[6] == 0x01) {
		if (data[7] >= 0 && data[7] <= NumButtons) {
			int index = data[7];
			if (data[4] == 0x00) {
				readButtonData[index] = false;
			} else if (data[4] == 0x01) {
				readButtonData[index] = true;
			}
		}
	} else if (data[6] == 0x02) {
/*		if (data[7] > 0x10) {
			data.clear();
			assert(data.empty());
		}
		*/
		if ((data[7] >= 0 && data[7] < NumSticks - 2) || data[7] == 0x0c || data[7] == 0x0d) {
			int index = data[7];
			if (data[7] == 0x0c || data[7] == 0x0d) {
				index -= 8;
			}
			readStickData[index] = data[5];
			if (precisionFlag) {
				readStickData[index] *= 0x100;
				readStickData[index] += data[4];
				if (readStickData[index] >= 32768) 
					readStickData[index] -= 65536;
				/*
				if (data[7] != index)
					readStickData[index] += 32767;
				*/
			} else {
				if (readStickData[index] >= 128) 
					readStickData[index] -= 256;
				/*
				if (data[7] != index)
					readStickData[index] += 128;
				*/
			}
		}
		if (data[7] >= 0x04 && data[7] <= 0x06) {
			int index = data[7] - 0x04;
			if (precisionFlag)
				readAxisData[index] = data[4] + data[5] * 0x100;
				if (readAxisData[index] >= 32768) 
					readAxisData[index] -= 65536;
			else
				readAxisData[index] = data[5];
				if (readAxisData[index] >= 128)
					readAxisData[index] -= 256;
		}
	}
}

void Ds3Read::readLoop() {
	while (loopFlag) {
		read();
	}
}

void Ds3Read::update() {
	memcpy(beforeButtonData, buttonData, sizeof(beforeButtonData));
	memcpy(buttonData, readButtonData, sizeof(buttonData));
	memcpy(stickData, readStickData, sizeof(stickData));
	memcpy(axisData, readAxisData, sizeof(axisData));
}

void Ds3Read::yReverseSet(bool setVar) {
	yReverse = setVar;
}

bool Ds3Read::button(ButtonsNum Button, bool onlyFlag) {
	if (onlyFlag) {
		for (int i = 0; i < NumButtons; ++i) {
			if (buttonData[i]) {
				if (i != Button)
					return false;
			}
		}
	}
	return buttonData[Button];
}

int Ds3Read::stick(SticksNum Stick) {
	if (yReverse && (Stick == LEFT_Y || Stick == RIGHT_Y)) {
		return -stickData[Stick];
	}
	return stickData[Stick];
}

int Ds3Read::acceleration(AxisNum Axis) {
	return axisData[Axis];
}

Ds3Read::~Ds3Read() {
	loopFlag = false;
	readThread.join();
	JoyStick.close();
}

bool Ds3Read::press(ButtonsNum Button) {
	return !beforeButtonData[Button] && buttonData[Button];
}

bool Ds3Read::release(ButtonsNum Button) {
	return beforeButtonData[Button] && !buttonData[Button];
}
