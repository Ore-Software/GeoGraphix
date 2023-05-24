#pragma once
#include <vector>

class HeightMap
{
public:
	HeightMap(int width, int length);

	virtual void Generate() {};

	std::vector<float> map;
};