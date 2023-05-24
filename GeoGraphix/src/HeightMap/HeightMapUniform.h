#pragma once
#include <vector>

class HeightMapUniform
{
public:
	HeightMapUniform(int width, int length, float height);

	void Generate();

	std::vector<float> m_Map;
	int m_Width, m_Length;
	float m_Height;
};