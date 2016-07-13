#include "RobotUtil.hpp"
#include "ScrpMaster.hpp"

#include <wiringPi.h>
#include <wiringSerial.h>
#include <chrono>
#include <thread>
#include <queue>
#include <mutex>
#include <unistd.h>

const int STX = 0x41;

using namespace std;
using namespace rbutil;

const int SEND_DATA_NUM = 7;

bool rbutil::wiringPiSetupGpioFlag = false;

ScrpMaster::ScrpMaster() {
	init(4, 10, "/dev/ttyAMA0", 115200);
}

ScrpMaster::ScrpMaster(int rede, int timeout, const char* devFileName, int bRate) {
	init(rede, timeout, devFileName, bRate);
}

void ScrpMaster::init(int rede, int timeout, const char* devFileName, int bRate) {
	sumCheckSuccess = false;
	recentReceivedData = 0;
	threadLoopFlag = false;
	this->bRate = bRate;
	this->timeOut = timeout;
	this->redePin = rede;
	this->serialFileName = (char*)devFileName;
}

void ScrpMaster::init() {
	this->serialDev = serialOpen(serialFileName, bRate);
	if (serialDev < 0) {
		throw runtime_error("SerialOpenError");
	}
	if (!wiringPiSetupGpioFlag) {
		if (wiringPiSetupGpio() < 0) {
			serialClose(serialDev);
			throw runtime_error("WiringPiSetupError");
		}
		wiringPiSetupGpioFlag = true;
	}
	pinMode(this->redePin, OUTPUT);
}

void ScrpMaster::setTimeOut(int timeout) {
	timeOut = timeout;
}

short ScrpMaster::sending(unsigned char id, unsigned char cmd, short data) {
	// 送信
	unsigned short uData = (unsigned short)data;
	unsigned char sendDataArray[SEND_DATA_NUM] = {0xFF, STX, id, cmd, (unsigned char)(uData & 0xFF), (unsigned char)(uData >> 8), (unsigned char)((id + cmd + (uData & 0xFF) + (uData >> 8)) & 0xFF)};
	lock_guard<mutex> lock(mtx);

	digitalWrite(this->redePin, 1);
	for (int i = 0; i < SEND_DATA_NUM; ++i) {
		serialPutchar(serialDev, sendDataArray[i]);
		delayMicroseconds(90);
	}
	digitalWrite(this->redePin, 0);
	// 受信
	bool stxFlag = false;
	char receivedArray[5] = {};
	int i = 0;

	auto startTime = chrono::system_clock::now();
	sumCheckSuccess = false;
	while (chrono::time_point<chrono::system_clock>(startTime + chrono::milliseconds(timeOut)) >= chrono::system_clock::now() and !sumCheckSuccess) {

		while (serialDataAvail(serialDev) > 0) {
			char gotData = serialGetchar(serialDev);
			if (gotData == STX and !stxFlag) {
				stxFlag = true;
				continue;
			}
			if (stxFlag) {
				receivedArray[i++] = gotData;
			}
			if (i > 4) {
				unsigned char sum = 0;
				for (int j = 0; j < 4; ++j)
					sum += receivedArray[j];
				if (sum == receivedArray[4]) {
					sumCheckSuccess = true;
					break;
				}
			}
		}
	}
	if (serialDataAvail(serialDev) < 0) {
		throw runtime_error("SerialComError");
	}
	recentReceivedData = receivedArray[2] + (receivedArray[3] << 8);
	return receivedArray[2] + (receivedArray[3] << 8);
}

void ScrpMaster::asyncSending(void) {
	while (!sendDataQueue.empty()) {
		threadLoopFlag = true;
		SendDataFormat sendData = sendDataQueue.front();
		sendDataQueue.pop();
		send(sendData, false);
	}
	threadLoopFlag = false;
}

short ScrpMaster::send(unsigned char id , unsigned char cmd, short data, bool asyncFlag) {
	if (asyncFlag) {
		SendDataFormat sendData = {id, cmd, data};
		sendDataQueue.push(sendData);
		if (!threadLoopFlag) {
			if (sendThread.joinable())
				sendThread.join();
			sendThread = thread([&]{ asyncSending(); });
		}
		return 0;
	}
	return sending(id, cmd, data);
}

short ScrpMaster::send(SendDataFormat sendData, bool asyncFlag) {
	return send(sendData.id, sendData.cmd, sendData.argData, asyncFlag);
}

ScrpMaster::~ScrpMaster() {
	if (sendThread.joinable())
		sendThread.join();
	serialClose(serialDev);
}
