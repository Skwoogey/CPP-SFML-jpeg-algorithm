#include "colours.h"

#include <iostream>

YCbCr::YCbCr(const RGBA& colour)
{
	auto sgn = [] (int val) {
		return (0 < val) - (val < 0);
	};

	int temp;
	temp = ((299 * (int)colour.R) + (587 * (int)colour.G) + (114 * (int)colour.B));
	temp = (temp + sgn(temp) * 500) / 1000;
	Y = (byte)temp;

	temp = ((-169 * (int)colour.R) + (-331 * (int)colour.G) + (500 * (int)colour.B));
	temp = (temp + sgn(temp) * 500) / 1000 + 128;
	Cb = (byte)temp;

	temp = ((500 * (int)colour.R) + (-419 * (int)colour.G) + (-81 * (int)colour.B));
	temp = (temp + sgn(temp) * 500) / 1000 + 128;
	Cr = (byte)temp;
}

std::ostream& operator<<(std::ostream& os, const RGBA& colour)
{
	os << "R: " << static_cast<int>(colour.R) <<
		" G: " << static_cast<int>(colour.G) <<
		" B: " << static_cast<int>(colour.B) <<
		" A: " << static_cast<int>(colour.A);

	return os;
}

std::ostream& operator<<(std::ostream& os, const YCbCr& colour)
{
	os << "Y: " << static_cast<int>(colour.Y) <<
		" Cb: " << static_cast<int>(colour.Cb) <<
		" Cr: " << static_cast<int>(colour.Cr);

	return os;
}
