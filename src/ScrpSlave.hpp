#pragma once

namespace rbutil {
	class ScrpSlave {
	public:
		ScrpSlave(int , unsigned char, const char*, int);
		void addCMD(unsigned char, bool (*proc)(int, int&));
		bool check(void);
	private:
		int serialDev;
		unsigned char id;
		unsigned char redePin;
		bool (*procs[256])(int, int&);
		bool initFlag = false;
	};
}
		
