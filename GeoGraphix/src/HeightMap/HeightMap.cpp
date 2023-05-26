#include "HeightMap.h"

HeightMap::HeightMap(int width, int length)
	: m_Width(width), m_Length(length)
{
	m_Map.resize(width * length);
}
