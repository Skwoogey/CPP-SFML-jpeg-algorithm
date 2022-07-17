#pragma once

#include <iostream>
#include <cmath>
#include <array>
#include <memory>

#include "DynamicMultiArray.h"
#include "DCT.h"
#include "colours.h"



class JpegCompress
{
private:
	using ImagePatch = DynamicMultiArray<byte, 2>;
	using CoefPatch = DynamicMultiArray<int, 2>;
	using RGBAImageArray = DynamicMultiArray<RGBA, 2>;
	using YCbCrImageArray = DynamicMultiArray<YCbCr, 2>;
	using ImageMap = DynamicMultiArray<ImagePatch, 2>;
	using CoefMap = DynamicMultiArray<CoefPatch, 2>;

	std::array<int, 2> DCT_ARRAY_N = { DCT_N, DCT_N };
	std::array<int, 2> Y_mapTiles;
	std::array<int, 2> C_mapTiles;

	// stage 1
	RGBAImageArray RGBAImage;

	// stage 2
	YCbCrImageArray YCbCrImage;

	// stage 3
	ImageMap Y_channel;
	ImageMap Cb_channel;
	ImageMap Cr_channel;

	// stage 4
	DCT_2D DCT;
	CoefMap Y_DCT;
	CoefMap Cb_DCT;
	CoefMap Cr_DCT;

	// stage 5
	CoefMap quantizedY_DCT;
	CoefMap quantizedCb_DCT;
	CoefMap quantizedCr_DCT;

public:
	JpegCompress(RGBAImageArray& image);

	void compressStage1();
	void compressStage2();
	void compressStage3();
	void compressStage4();

	void compress();

};

