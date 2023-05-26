#include "HeightMapUniform.h"

HeightMapUniform::HeightMapUniform(int width, int length, float height)
	: HeightMap(width, length), m_Height(height)
{
	Generate();
}

void HeightMapUniform::Generate()
{
	m_Map.resize(m_Width * m_Length, m_Height);
}
