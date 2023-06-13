#include "HeightMapOctaves.h"

#include <iostream>

HeightMapOctaves::HeightMapOctaves(int width, int length)
	: HeightMap(width, length)
{
}

void HeightMapOctaves::AddOctave(std::pair<bool, HeightMap*> octave)
{
	m_Octaves.push_back(octave);
	Generate();
}

void HeightMapOctaves::RemoveOctave(int index)
{
	if (index < m_Octaves.size() && index > -1)
	{
		m_Octaves.erase(m_Octaves.begin() + index);
		Generate();
	}
	else
		std::cerr << "Attempted to remove invalid octave index." << std::endl;
}

void HeightMapOctaves::Reset()
{
	m_Octaves.clear();
}

void HeightMapOctaves::Generate()
{
	m_Map.clear();
	m_Map.resize(m_Width * m_Length);
	for (int k = 0; k < m_Octaves.size(); k++)
	{
		if (m_Octaves[k].first)
		{
			for (int j = 0; j < m_Length; j++)
			{
				for (int i = 0; i < m_Width; i++)
				{
					m_Map[j * m_Width + i] += m_Octaves[k].second->m_Map[j * m_Width + i] * pow(2, -k);
				}
			}
		}
	}
}
