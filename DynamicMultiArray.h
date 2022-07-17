#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include <assert.h>
#include <functional>

template<class T, size_t N> class DynamicMultiArray
{
private:
	std::unique_ptr<T[]> array;
	std::array<int, N> size;
	std::array<int, N> strides;
	int totalUnits;

public:
	DynamicMultiArray() {};

	DynamicMultiArray(std::array<int, N>& dims)
	{
		resize(dims);
	};

	DynamicMultiArray(const DynamicMultiArray<T, N>& obj) :
		size(obj.size),
		strides(obj.strides),
		totalUnits(obj.totalUnits)
	{
		//std::cout << "DynamicMultiArray copy\n";
		array = std::make_unique<T[]>(totalUnits);
		copyFromMemory((const T*)(obj.array.get()));
	}

	DynamicMultiArray(DynamicMultiArray<T, N>&& obj) :
		size(obj.size),
		strides(obj.strides),
		totalUnits(obj.totalUnits)
	{
		//std::cout << "DynamicMultiArray move\n";
		array.reset(obj.array.release());
	}

	void resize(std::array<int, N>& dims)
	{
		size = dims;

		strides[N - 1] = 1;
		//std::cout << "strides backwards\n";
		//std::cout << strides[N - 1] << '\n';
		for (int i = N - 2; i >= 0; i--)
		{
			strides[i] = strides[i + 1] * size[i + 1];
			//std::cout << strides[i] << '\n';
		}

		totalUnits = strides[0] * size[0];
		//std::cout << "size: " << totalUnits << '\n';

		array = std::make_unique<T[]>(totalUnits);
	}

	void copyFromMemory(const T* src, int dstIndex = 0, int copySize = -1)
	{
		if (copySize < 0)
			copySize = totalUnits;
		assert(dstIndex + copySize <= totalUnits);

		std::memcpy(array.get(), src, copySize * sizeof(T));
	}

	T* get()
	{
		return array.get();
	}

	T& operator[](std::array<int, N>& index)
	{
		int offset = 0;
		for (int i = 0; i < N; i++)
		{
			assert(index[i] < size[i]);

			offset += strides[i] * index[i];
		}
		
		//std::cout << offset << '\n';

		return *(array.get() + offset);
	}

	const std::array<int, N>& getSize()
	{
		return size;
	}

	void forEach(std::function<void(T&)>& lambda)
	{
		for (int i = 0; i < totalUnits; i++)
		{
			lambda(static_cast<T&>(array[i]));
		}
	}

	void forEachEnumerate(std::function<void(T&, std::array<int, N>)>& lambda)
	{
		std::array<int, N> index;
		for (int i = 0; i < totalUnits; i++)
		{
			int indexRem = i;
			for (int d = 0; d < N; d++)
			{
				index[d] = indexRem / strides[d];
				indexRem %= strides[d];
			}
			lambda(array[i], index);
		}
	}
};

