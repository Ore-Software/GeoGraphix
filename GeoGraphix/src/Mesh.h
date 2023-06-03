#pragma once

#include "HeightMap/HeightMap.h"
#include "external/glm/ext/vector_float3.hpp"
#include "external/glm/geometric.hpp"

class Mesh
{
public:
	Mesh(const HeightMap& heightMap);
	~Mesh();

	void Generate(const HeightMap& heightMap);
	void Destroy();
	void Regenerate(const HeightMap& heightMap);

	float* m_VertexPos;
	unsigned int* m_Indices;
	std::vector<std::vector<std::vector<glm::vec3>>> m_FaceNormals;
	float* m_VertexNormals;
	float* m_Vertices;
};