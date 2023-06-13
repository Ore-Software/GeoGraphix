#pragma once

#include "HeightMap.h"

class HeightMapOctaves : public HeightMap
{
public:
	HeightMapOctaves(int width, int length);

	void AddOctave(HeightMap* octave);
	void RemoveOctave(int index);
	void Reset();
	void Generate() override;

	std::vector<HeightMap*> m_Octaves;
};