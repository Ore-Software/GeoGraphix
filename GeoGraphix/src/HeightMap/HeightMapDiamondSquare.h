#pragma once
#include "HeightMap.h"

class HeightMapDiamondSquare : public HeightMap
{
public:
	HeightMapDiamondSquare(int n);

	void Generate() override;
private:
	void DiamondSquare(int n, int x0, int x1, int y0, int y1);
	void DiamondStep(int n, int x0, int x1, int y0, int y1);
	void SquareStep(int n, int x0, int x1, int y0, int y1);

	int m_StartingN;
	int m_DSLength; // 2^n + 1 (length and width are equal for DS algorithm)
	std::vector<std::vector<float>> m_NoiseValues;
	float m_MaxInv;
};