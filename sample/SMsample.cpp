// ScrpMasterのサンプル 
// コマンドライン引数を3つ受け取ってそれらをID, CMD, DATAとして通信を行う 
#include <iostream>
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
	unsigned char id, cmd;
	unsigned short data;
	if (argc < 4) {		// コマンドライン引数が足りなければID:1 CMD:2 DATA:255で送信する 
		id = 1;
		cmd = 2;
		data = 255;
	} else {
		id = (unsigned char)stoi(argv[1]);		// コマンドライン引数からID, CMD, DATAを決めて送信
		cmd = (unsigned char)stoi(argv[2]);
		data = (short)stoi(argv[3]);
	}
	cout << (int)id << " " << (int)cmd << " " << (int)data << endl;
	try { 
		cout << sm.send(id, cmd, data) << endl;		// 通信
	}
	catch(runtime_error exception) {
		cout << "Communication Error." << endl;
		return -1;
	}
	cout << (sm.sumCheckSuccess ? "Receive Success" : "Receive Failed") << endl;	// 受信の成功の可否を見て結果を表示 
	return 0;
}
