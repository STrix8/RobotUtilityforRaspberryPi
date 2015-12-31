// Motorのサンプル 
// コマンドライン引数を3つ受け取ってそれらからMotorを定義し通信を行う 
#include <iostream>
#include <cstring>
#include <RobotUtil.hpp>

using namespace std;
using namespace rbutil;

int main(int argc, char *argv[]) {
	ScrpMaster sm;	// インスタンスの生成 
	sm.setTimeOut(1000);	// タイムアウトの設定 
	try {		// 初期化の際, 例外が出る可能性があるのでtryで囲む 
		sm.init();		// 初期化 
	}
	catch(runtime_error exception) {
		cout << "Setup Error" << endl;
		return -1;
	}
	unsigned char id, mNum;
	double magni;
	if (argc < 4) {
		id = 1;
		mNum = 2;
		magni = 1.0;
	} else {
		char* ends;
		id = (unsigned char)stoi(argv[1]);
		mNum = (unsigned char)stoi(argv[2]);
		magni = (double)strtod(argv[3], &ends);
	}
	Motor motor(id, mNum, magni, sm);
	for(;;) {
		int power;
		cin >> power;
		try {
			motor.spin(power);
		}
		catch(runtime_error exception) {
			cout << "Communication Error." << endl;
			return -1;
		}
		if (power == 0)
			break;
	}
	return 0;
}
