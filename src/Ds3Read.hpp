// RaspberryPiでDs3Readの信号を読取る
#pragma once

#include <fstream>
#include <thread>

#define UPDATELOOP(c, x) for((c).update(); (x); (c).update())	// コントローラcの状態をupdateしながら条件xでループ

namespace rbutil{
	enum ButtonsNum {SELECT, LEFT_STICK, RIGHT_STICK, START, UP, RIGHT, DOWN, LEFT, L2, R2, L1, R1, TRIANGLE, CIRCLE, CROSS, SQUARE};
  enum ButtonsNum {H_SELECT = 0x0001, H_LEFT_STICK = 0x0002, H_RIGHT_STICK = 0x0004, H_START = 0x0008, H_UP = 0x0010, H_RIGHT = 0x0020, H_DOWN = 0x0040, H_LEFT = 0x0080, H_L2 = 0x0100, H_R2 = 0x0200, H_L1 = 0x0400, H_R1 = 0x0800, H_TRIANGLE = 0x1000, H_CIRCLE = 0x2000, H_CROSS = 0x4000, H_SQUARE = 0x8000};
	const int NumButtons = 16;
	enum SticksNum {LEFT_X, LEFT_Y, RIGHT_X, RIGHT_Y, LEFT_T, RIGHT_T};
	const int NumSticks = 6;
	enum AxisNum {X_AXIS, Y_AXIS, Z_AXIS};
	const int NumAxis = 3;
	class Ds3Read {
	public:
		Ds3Read();
		Ds3Read(bool, int timeout = 0);
		Ds3Read(const char*, bool precision = false, int timeout = 0);	// 必要ならファイル名を入れる デフォルトは/dev/input/js0
		void init(const char*, bool, int);
		bool isConnected();
		void precisionMode(bool precision = true);
		void read();
		void update();		// コントローラの状態を更新
		void yReverseSet(bool setVar = true);
		bool button(ButtonsNum, bool onlyFlag = false);	// 指定されたボタンが押されているか返す 第2引数がtrueだとそのボタンだけが押されている場合のみtrueを返す
    unsigned short hexButton();
		bool press(ButtonsNum);
		bool release(ButtonsNum);
		int stick(SticksNum);	// 指定されたスティックの状態を返す
		int acceleration(AxisNum);
		virtual ~Ds3Read();
	private:
		void readLoop();
		std::fstream JoyStick;
		bool loopFlag;
		bool yReverse;
		bool connectedFlag;
		bool precisionFlag;
		bool threadFlag;
		std::thread readThread;
		bool readButtonData[NumButtons];
    unsigned short readHexButtonData;
		int readStickData[NumSticks];
		int readAxisData[NumAxis];
		bool buttonData[NumButtons];
    unsigned short hexButtonData;
		int stickData[NumSticks];
		int axisData[NumAxis];
		bool beforeButtonData[NumButtons];
	};
}
