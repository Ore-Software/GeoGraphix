#include "HeightMapUniform.h"

HeightMapUniform::HeightMapUniform(int width, int length, float height)
	: m_Width(width), m_Length(length), m_Height(height)
{
	Generate();
}

void HeightMapUniform::Generate()
{
	m_Map.resize(m_Width * m_Length, m_Height);
}
