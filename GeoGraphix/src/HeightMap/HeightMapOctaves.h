#pragma once

#include "HeightMap.h"

class HeightMapOctaves : public HeightMap
{
public:
	HeightMapOctaves(int width, int length);

	void AddOctave(HeightMap* octave);
	void RemoveOctave(int index);
	void Generate() override;

private:
	std::vector<HeightMap*> m_Octaves;
};