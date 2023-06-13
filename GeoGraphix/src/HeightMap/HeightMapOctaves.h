#pragma once

#include "HeightMap.h"

class HeightMapOctaves : public HeightMap
{
public:
	HeightMapOctaves(int width, int length);

	void AddOctave(std::pair<bool, HeightMap*> octave);
	void RemoveOctave(int index);
	void Reset(int width, int length);
	void Generate() override;

	std::vector<std::pair<bool, HeightMap*>> m_Octaves;
};