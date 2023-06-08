#pragma once
#include "HeightMap.h"

class HeightMapDiamondSquare : public HeightMap
{
public:
	HeightMapDiamondSquare(int width, int length);

	void Generate() override;
private:
	void DiamondSquare(int n, int centerX, int centerY);
	void DiamondStep(int offset, int centerX, int centerY);
	void SquareStep(int offset, int centerX, int centerY);
	void CalcSquareValues(int offset, int centerX, int centerY);

	int m_N; // n where 2^n + 1 is length and width of generated map
	std::vector<std::vector<float>> m_HeightValues;
	float m_MaxInv; // (1 / RAND_MAX) used for calculating float between 0 and 1
};