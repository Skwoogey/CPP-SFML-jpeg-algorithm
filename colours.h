#pragma once

#include <iostream>

using byte = unsigned char;

struct RGBA;
struct YCbCr;

struct RGBA
{
	byte R;
	byte G;
	byte B;
	byte A;

	friend std::ostream& operator<<(std::ostream& os, const RGBA& colour);
};

struct YCbCr
{
	byte Y = 0;
	byte Cb = 0;
	byte Cr = 0;

	YCbCr(const RGBA& colour);
	YCbCr() {};

	friend std::ostream& operator<<(std::ostream& os, const YCbCr& colour);
};