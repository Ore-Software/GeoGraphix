#pragma once

#include "HeightMap/HeightMap.h"

class Mesh
{
public:
	Mesh(const HeightMap& heightMap);
	~Mesh();

	void Generate(const HeightMap& heightMap);
	void Destroy();
	void Regenerate(const HeightMap& heightMap);

	float* m_Vertices;
	unsigned int* m_Indices;
};