#pragma once
#include "HeightMap.h"

class HeightMapUniform : public HeightMap
{
public:
	HeightMapUniform(int width, int length, float height);

	void Generate() override;

	float m_Height;
};