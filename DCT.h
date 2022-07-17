#pragma once

#include <iostream>
#include <cmath>
#include <array>
#include <memory>

#include "DynamicMultiArray.h"

using byte = unsigned char;
const int DCT_N = 8;

class DCT_2D
{
private:
	using BasisFunction = DynamicMultiArray<int, 2>;
	using BasisFunctionMap = DynamicMultiArray<std::unique_ptr<BasisFunction>, 2>;

	BasisFunctionMap basisFunctions;

	void calculateBasisFunctions();

public:

	DCT_2D();

	DynamicMultiArray<int, 2>  transform(DynamicMultiArray<byte, 2>& image);

	DynamicMultiArray<int, 2>  restore(DynamicMultiArray<int, 2>& DCT);
};

