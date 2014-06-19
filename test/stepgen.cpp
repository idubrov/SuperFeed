#include <cstdint>
#include <iostream>
#include <cmath>
#include <string>
#include "stepgen.hpp"

using namespace ::std;

int main(int argc, char** argv)
{
	int microsteps = 1;
	int steps = 10;
	if (argc > 1)
	{
		steps = stoi(argv[1]);
	}
	if (argc > 2)
	{
		microsteps = stoi(argv[2]);
	}
	steps *= microsteps;

	int rpm = 120;
	int leadscrew = 16; // 16 TPI
	int spr = 200 * microsteps; // steps per revolution
	int thread = 8;
	int K = leadscrew * spr / thread; // steps per revolution
	int F = 1000000;
	int Fusec = F / 1000000;
	int a = 1000 * microsteps;
	int v = K * rpm / 60; // steps per sec

	uint32_t c0 = stepgen::stepgen::first(F, a);
	if (c0 == 0)
	{
		cout << "First step is too long!" << endl;
	}
	uint32_t cs = stepgen::stepgen::slew(F, v);

	stepgen::stepgen gen(steps, c0, cs);
	uint32_t delay;
	while ((delay = gen.next()) != 0)
	{
		cout << delay << endl;
	}

	return 0;
}
