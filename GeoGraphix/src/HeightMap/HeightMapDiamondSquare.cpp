#include "HeightMapDiamondSquare.h"

HeightMapDiamondSquare::HeightMapDiamondSquare(int width, int length)
	: HeightMap(width, length), m_MaxInv((float)1/(float)RAND_MAX)
{
	int largestDimension = std::max(width, length);
	m_N = log2(largestDimension) + 1;
	Generate();
}

void HeightMapDiamondSquare::Generate()
{
	int DSlength = (int)pow(2, m_N) + 1; // length/width for diamond square map

	m_HeightValues.resize(DSlength);
	for (int j = 0; j < DSlength; j++)
	{
		m_HeightValues[j].resize(DSlength);
	}

	// Assign four corner values
	m_HeightValues[0][0] = (float)std::rand() * m_MaxInv;
	m_HeightValues[0][DSlength - 1] = (float)std::rand() * m_MaxInv;
	m_HeightValues[DSlength - 1][DSlength - 1] = (float)std::rand() * m_MaxInv;
	m_HeightValues[DSlength - 1][0] = (float)std::rand() * m_MaxInv;

	int offset = (DSlength - 1) / 2;
	DiamondSquare(offset, offset, offset);

	for (int j = 0; j < m_Length; j++)
	{
		for (int i = 0; i < m_Width; i++)
		{
			m_Map[j * m_Width + i] = m_HeightValues[j][i];
		}
	}
}

void HeightMapDiamondSquare::DiamondSquare(int offset, int centerX, int centerY)
{
	DiamondStep(offset, centerX, centerY);
	SquareStep(offset, centerX, centerY);

	if ((offset /= 2) < 1) return;

	DiamondSquare(offset, centerX - offset, centerY - offset); // Bottom-left subsquare
	DiamondSquare(offset, centerX + offset, centerY - offset); // Bottom-right subsquare
	DiamondSquare(offset, centerX - offset, centerY + offset); // Top-left subsquare
	DiamondSquare(offset, centerX + offset, centerY + offset); // Top-right subsquare
}

void HeightMapDiamondSquare::DiamondStep(int offset, int centerX, int centerY)
{
	m_HeightValues[centerY][centerX] = (m_HeightValues[centerY - offset][centerX - offset]	// bottom-left
		+ m_HeightValues[centerY - offset][centerX + offset]								// bottom-right
		+ m_HeightValues[centerY + offset][centerX - offset]								// top-left
		+ m_HeightValues[centerY + offset][centerX + offset]								// top-right
		+ std::rand() * m_MaxInv
	) / 5;
}

void HeightMapDiamondSquare::SquareStep(int offset, int centerX, int centerY)
{
	CalcSquareValues(offset, centerX, centerY - offset); // Bottom
	CalcSquareValues(offset, centerX, centerY + offset); // Top
	CalcSquareValues(offset, centerX - offset, centerY); // Left
	CalcSquareValues(offset, centerX + offset, centerY); // Right
}

void HeightMapDiamondSquare::CalcSquareValues(int offset, int centerX, int centerY)
{
	int DSLength = pow(2, m_N);

	// Random value
	int divisor = 1;
	m_HeightValues[centerY][centerX] += std::rand() * m_MaxInv;

	// Bottom
	if (centerY != 0)
	{
		m_HeightValues[centerY][centerX] += m_HeightValues[centerY - offset][centerX];
		divisor++;
	}

	// Top
	if (centerY != DSLength)
	{
		m_HeightValues[centerY][centerX] += m_HeightValues[centerY + offset][centerX];
		divisor++;
	}

	// Left
	if (centerX != 0)
	{
		m_HeightValues[centerY][centerX] += m_HeightValues[centerY][centerX - offset];
		divisor++;
	}

	// Right
	if (centerX != DSLength)
	{
		m_HeightValues[centerY][centerX] += m_HeightValues[centerY][centerX + offset];
		divisor++;
	}

	m_HeightValues[centerY][centerX] /= divisor;
}
