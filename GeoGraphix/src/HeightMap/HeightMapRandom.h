#pragma once
#include "HeightMap.h"
#include <chrono>

class HeightMapRandom : public HeightMap
{
public:
	HeightMapRandom(int width, int length);

	void Generate() override;
};