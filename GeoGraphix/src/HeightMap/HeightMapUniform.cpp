#include "HeightMapUniform.h"

HeightMapUniform::HeightMapUniform(int width, int length, float height)
	: HeightMap(width, length), m_Height(height)
{
	Generate();
}

void HeightMapUniform::Generate()
{
	for(int i = 0; i < m_Width * m_Length; i++)
		m_Map[i] = m_Height;
}
