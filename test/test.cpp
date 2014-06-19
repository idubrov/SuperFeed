#include <cstdint>
#include <iostream>
#include <cmath>
#include <string>
#include "stepgen.hpp"

using namespace ::std;

int main(int argc, char** argv)
{
	int rpm = 120;
	int leadscrew = 16; // 16 TPI
	int spr = 200; // steps per revolution
	//int microsteps = 1;
	int thread = 8;
	int K = leadscrew * spr / thread; // steps per revolution
	int F = 1000000;
	int Fusec = F / 1000000;
	int a = 1000;
	int v = K * rpm / 60; // steps per sec
	int c0 = static_cast<int>(Fusec * sqrt(2000000 / a) * 676) << 8;
	int cs = static_cast<int>(F / v) << 8;

	int steps = 10;
	if (argc > 1)
	{
		steps = stoi(argv[1]);
	}

	cout << "Slew step: " << (cs >> 8) << endl;
	cout << "First step: " << (c0 >> 8) << endl;
	stepgen::stepgen gen(steps, c0, cs);
	uint32_t delay;
	while ((delay = gen.next()) != 0)
	{
		cout << (delay >> 8) << endl;
	}

	return 0;
}
