#include "JpegCompress.h"

#include <cmath>
#include <algorithm>
#include <array>
#include <iostream>
#include <functional>
#include <memory>
#include <iomanip>
#include <assert.h>

#include "DynamicMultiArray.h"
#include "DCT.h"

JpegCompress::JpegCompress(RGBAImageArray& image) :
	RGBAImage(image),
	YCbCrImage((std::array<int, 2>&)image.getSize())
{
	Y_mapTiles = {
		(image.getSize()[0] + DCT_N - 1) / DCT_N,
		(image.getSize()[1] + DCT_N - 1) / DCT_N
	};

	C_mapTiles = {
		(image.getSize()[0] + (DCT_N * 2) - 1) / (DCT_N * 2),
		(image.getSize()[1] + (DCT_N * 2) - 1) / (DCT_N * 2)
	};
	
	std::function<void(ImagePatch&)> setImagePatchSize =
		[this](ImagePatch& patch)
	{
		patch.resize(DCT_ARRAY_N);
	};

	Y_channel.resize(Y_mapTiles);
	Y_channel.forEach(setImagePatchSize);

	Cb_channel.resize(C_mapTiles);
	Cb_channel.forEach(setImagePatchSize);

	Cr_channel.resize(C_mapTiles);
	Cr_channel.forEach(setImagePatchSize);

	std::function<void(CoefPatch&)> setCoefPatchSize =
		[this](CoefPatch& patch)
	{
		patch.resize(DCT_ARRAY_N);
	};

	Y_DCT.resize(Y_mapTiles);
	Y_DCT.forEach(setCoefPatchSize);

	Cb_DCT.resize(C_mapTiles);
	Cb_DCT.forEach(setCoefPatchSize);

	Cr_DCT.resize(C_mapTiles);
	Cr_DCT.forEach(setCoefPatchSize);

	quantizedY_DCT.resize(Y_mapTiles);
	quantizedY_DCT.forEach(setCoefPatchSize);

	quantizedCb_DCT.resize(C_mapTiles);
	quantizedCb_DCT.forEach(setCoefPatchSize);

	quantizedCr_DCT.resize(C_mapTiles);
	quantizedCr_DCT.forEach(setCoefPatchSize);
}

void JpegCompress::compressStage1()
{

}

void JpegCompress::compressStage2()
{
}

void JpegCompress::compressStage3()
{
}

void JpegCompress::compressStage4()
{
}

void JpegCompress::compress()
{
	compressStage1();
	compressStage2();
	compressStage3();
	compressStage4();
}
