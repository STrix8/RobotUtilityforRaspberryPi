#pragma once

#include <fstream>
#include <thread>
#include <queue>
#include <mutex>

namespace rbutil {
	typedef struct {
		unsigned char id;
		unsigned char cmd;
		short argData;
	} SendDataFormat;

	class ScrpMaster {
	public:
		ScrpMaster();
		ScrpMaster(int, int timeout = 10, const char* devFileName = "/dev/ttyAMA0", int bRate = 115200);
		void init();
		void setTimeOut(int);
		short send(unsigned char, unsigned char, short, bool asyncFlag = false);
		short send(SendDataFormat, bool asyncFlag = false);
		short sending(unsigned char, unsigned char, short);
		virtual ~ScrpMaster();
		bool sumCheckSuccess;
		short recentReceivedData;
	private:
		void init(int, int, const char*, int);
		void asyncSending(void);
		bool nowSendingFlag;
		bool threadLoopFlag;
		char* serialFileName;
		int timeOut;
		int serialDev;
		int bRate;
		int redePin;
		std::thread sendThread;
		std::queue<SendDataFormat> sendDataQueue;
		std::mutex mtx;
	};
}
