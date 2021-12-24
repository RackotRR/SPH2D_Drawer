#pragma once

struct Particle {
	double x;
	double y;
	int itype;

	bool operator<(const Particle& p) const
	{
		double val1 = x * 10 + y - itype;
		double val2 = p.x * 10 + p.y - p.itype;
		return val1 < val2;
	}
};