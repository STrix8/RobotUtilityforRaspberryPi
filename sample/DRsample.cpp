#include <iostream>
#include <unistd.h>
#include <RobotUtil.hpp>

using namespace std;
using namespace rbutil;

int main(void) {
	cout << "Start" << endl;
	Ds3Read Controller;
	Controller.update();
	UPDATELOOP (Controller, !Controller.button(START)) {
		for (int j = 0; j < NumButtons; ++j)
			cout << "B" << j << ":" << Controller.button(ButtonsNum(j)) << " ";

		for (int j = 0; j < NumSticks; ++j)
			cout << "S" << j << ":" Controller.stick(SticksNum(j)) << " ";

		for (int j = 0; j < NumAxis; ++j)
			cout << "A" << j << ":" Controller.acceleration(AxisNum(j)) << " ";
		cout << "\r";
		usleep(14000);
	}
	cout << endl;
	return 0;
}
