#include "ScrpMaster.hpp"
#include "Motor.hpp"
#include <cstring>
#include <string>
#include <fstream>
#include <unistd.h>

using namespace std;
using namespace rbutil;

int rbutil::MaxMotorPower = 200;

Motor::Motor() {
	initFlag = false;
}

Motor::Motor(unsigned char id, unsigned char mNum, double magni, ScrpMaster &sm, short maxPower) {
	this->id = id;
	this->mNum = mNum;
	this->magni = magni;
	this->sm = &sm;
	this->maxPower = maxPower;
	initFlag = true;
}

Motor::Motor(MotorDataFormat MotorData, ScrpMaster &sm, short maxPower) {
	this->id = MotorData.id;
	this->mNum = MotorData.mNum;
	this->magni = MotorData.magni;
	this->sm = &sm;
	this->maxPower = maxPower;
	initFlag = true;
}

short Motor::changeMaxPower(short maxPower) {
	return (this->maxPower = maxPower);
}

bool Motor::spin(short motorPower, bool asyncFlag) {
	if (!initFlag)
		return false;
	motorPower = (short)(magni * motorPower);
	if (motorPower > maxPower)
		motorPower = maxPower;
	if (motorPower < -maxPower)
		motorPower = -maxPower;
	return motorPower == sm->send(id, 2 + mNum, motorPower, asyncFlag);
}

Motor::~Motor() {
	spin(0);
}

int rbutil::loadMotorSetting(char* FileName, rbutil::MotorDataFormat* MotorDatas, int NumMotors) {
	// 現在のパスを取得
	char buf[512] = {};
	readlink("/proc/self/exe", buf, sizeof(buf) - 1); 	// 実行ファイルのパスを取得
	string path(buf);
	path += FileName;
	// モーターの設定を読み込む
	ifstream settingFile;
	settingFile.open(path);
	if (!settingFile)
		return -1;
	for (int i = 0;(i < NumMotors) && (!settingFile.eof()); ++i) {
		for (int j = 0;j < 3 && (!settingFile.eof());++j) {
			string str;
			char* ends;
			settingFile >> str;
			if ((str.c_str()[0] >= '0' && str.c_str()[0] <= '9') || (j == 2 && str.c_str()[0] == '-')) {
				switch(j) {
				case 0:
					MotorDatas[i].id = atoi(str.c_str());
					break;
				case 1:
					MotorDatas[i].mNum = atoi(str.c_str());
					break;
				case 2:
					MotorDatas[i].magni = strtod(str.c_str(), &ends);
					break;
				}
			}
		}
	}
	return 0;
}
