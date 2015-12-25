#include <iostream>
#include <unistd.h>
#include <RobotUtil.hpp>

using namespace std;
using namespace rbutil;

int main(void) {
	cout << "Connect DualShock3." << endl;
	Ds3Read Controller;
	if (!Controller.isConnected()) {
		cout << "Couldn't connect DualShock3." << endl;
		return -1;
	}
	cout << "Connected." << endl;
	Controller.update();
	UPDATELOOP (Controller, !Controller.button(START, true)) {
		for (int j = 0; j < NumButtons; ++j)
			cout << "B" << j << ":" << Controller.button(ButtonsNum(j)) << " ";

		for (int j = 0; j < NumSticks; ++j) {
			cout << "S" << j << ":";
			cout.width(4);
			cout << Controller.stick(SticksNum(j));
			cout << " ";
		}

		for (int j = 0; j < NumAxis; ++j) {
			cout << "A" << j << ":";
			cout.width(4);
			cout << Controller.acceleration(AxisNum(j));
			cout << " ";
		}

		cout << "\r";
		usleep(14000);
	}
	cout << endl;
	return 0;
}
