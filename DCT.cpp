#include "DCT.h"

#include <cmath>
#include <algorithm>
#include <array>
#include <iostream>
#include <functional>
#include <memory>
#include <iomanip>
#include <assert.h>

#include "DynamicMultiArray.h"

const double PI = 3.14159265358979323846;

const double doubleN = static_cast<const double>(DCT_N);

std::array<int, 2> arrayN = { DCT_N, DCT_N };

const int quantizationLevel = 8192;

const int quantizationDenominator = quantizationLevel * DCT_N;

double alphaFunc(int i)
{
	return i == 0 ?
		sqrt(1.0 / doubleN) :
		sqrt(2.0 / doubleN);
}

void DCT_2D::calculateBasisFunctions()
{
	using BasisFunc1D = std::array<int, DCT_N>;
	using BasisFuncs1D = std::array<BasisFunc1D, DCT_N>;

	BasisFuncs1D basisFuncs1D;

	for (int bf_i = 0; bf_i < DCT_N; bf_i++)
	{
		//std::cout << "basis func: " << bf_i << "\n";
		for (int sample_i = 0; sample_i < DCT_N; sample_i++)
		{
			double value = cos((PI * bf_i * (2.0 * sample_i + 1.0)) / (2.0 * doubleN)) * quantizationLevel;

			basisFuncs1D[bf_i][sample_i] = static_cast<int>(value);
			//std::cout << (int)basisFuncs1D[bf_i][sample_i] << " ";
		}
		//std::cout << "\n";
	}

	std::function<void(std::unique_ptr<BasisFunction>&, std::array<int,2>)> createAndInitMaps = 
		[&basisFuncs1D](std::unique_ptr<BasisFunction> &bf, std::array<int, 2> mapIndex) 
	{
		bf.reset(new BasisFunction(arrayN));

		std::array<int, 2> basisFuncIndex;

		/*std::cout << "basis func: "
			<< mapIndex[0] << " "
			<< mapIndex[1] << " "
			<< bf.get() << "\n";*/

		for (int d0 = 0; d0 < DCT_N; d0++)
		{
			for (int d1 = 0; d1 < DCT_N; d1++)
			{
				int value = basisFuncs1D[mapIndex[0]][d0] * basisFuncs1D[mapIndex[1]][d1] / quantizationLevel;

				basisFuncIndex = { d0, d1 };
				(*bf)[basisFuncIndex] = static_cast<int>(value);
				//std::cout << std::setw(4) << static_cast<int>((*bf)[basisFuncIndex]) << " ";
			}
			//std::cout << "\n";
		}
	};

	basisFunctions.forEachEnumerate(createAndInitMaps);
}

DCT_2D::DCT_2D() :
	basisFunctions(arrayN)
{
	calculateBasisFunctions();
}

DynamicMultiArray<int, 2> DCT_2D::transform(DynamicMultiArray<byte, 2>& image)
{
	assert(image.getSize() == arrayN);

	DynamicMultiArray<int, 2> encoding(arrayN);

	std::function<void(int&, std::array<int, 2>)> calculateCoefs =
		[this, &image](int& DCTcoef, std::array<int, 2> mapIndex)
	{
		int value = 0;
		std::array<int, 2> basisFuncIndex;

		for (int d0 = 0; d0 < DCT_N; d0++)
		{
			for (int d1 = 0; d1 < DCT_N; d1++)
			{
				basisFuncIndex = { d0, d1 };
				auto& basisFunc = basisFunctions[mapIndex];
				value +=
					(static_cast<int>(image[basisFuncIndex])) *
					static_cast<int>((*basisFunc)[basisFuncIndex]);
			}
		}

		if (mapIndex[0] != 0)
			value *= 2;

		if (mapIndex[1] != 0)
			value *= 2;
		
		//std::cout << value << '\n';

		value = (value + (quantizationDenominator / 2)) / quantizationDenominator;

		//std::cout << value << '\n';

		DCTcoef = value;
	};

	encoding.forEachEnumerate(calculateCoefs);

	//std::array<int, 2> index;
	//std::cout << "DCT coefs:\n";
	//for (int d0 = 0; d0 < N; d0++)
	//{
	//	for (int d1 = 0; d1 < N; d1++)
	//	{
	//		index = { d0, d1 };
	//		std::cout << std::setw(4) << (int)encoding[index] << " ";
	//	}
	//	std::cout << '\n';
	//}

	return encoding;
}

DynamicMultiArray<int, 2> DCT_2D::restore(DynamicMultiArray<int, 2>& DCT)
{
	assert(DCT.getSize() == arrayN);

	DynamicMultiArray<int, 2> image(arrayN);

	std::function<void(int&)> zeroAll = [](int& elem)
	{
		elem = 0;
	};
	image.forEach(zeroAll);

	std::function<void(std::unique_ptr<BasisFunction>&, std::array<int, 2>)> calculateImage =
		[this, &DCT, &image](std::unique_ptr<BasisFunction>& basisFunction, std::array<int, 2> mapIndex)
	{
		std::array<int, 2> imageIndex;

		int DCTcoef = DCT[mapIndex];
		if (mapIndex[0] != 0)
			DCTcoef *= 1;

		if (mapIndex[1] != 0)
			DCTcoef *= 1;

		//std::cout << DCTcoef << '\n';

		for (int d0 = 0; d0 < DCT_N; d0++)
		{
			for (int d1 = 0; d1 < DCT_N; d1++)
			{
				imageIndex = { d0, d1 };

				image[imageIndex] +=
					(*basisFunction)[imageIndex] *
					DCTcoef;
			}
		}
	};

	basisFunctions.forEachEnumerate(calculateImage);

	std::function<void(int&)> scaleAll = [](int& elem)
	{
		elem = //(elem + (quantizationDenominator / 2)) / quantizationDenominator;
		std::min(
			std::max(
				(elem + (quantizationDenominator / 2)) / quantizationDenominator, 
				0
			),
			255
		);
	};
	image.forEach(scaleAll);

	std::array<int, 2> index;
	std::cout << "image:\n";
	for (int d0 = 0; d0 < DCT_N; d0++)
	{
		for (int d1 = 0; d1 < DCT_N; d1++)
		{
			index = { d0, d1 };
			std::cout << std::setw(4) << (int)image[index] << " ";
		}
		std::cout << '\n';
	}

	return image;
}
