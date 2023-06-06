#include "HeightMapDiamondSquare.h"

HeightMapDiamondSquare::HeightMapDiamondSquare(int n)
	: m_StartingN(n), m_DSLength((int)pow(2, n) + 1), HeightMap(m_DSLength, m_DSLength), m_MaxInv((float)1/(float)RAND_MAX)
{
	Generate();
}

void HeightMapDiamondSquare::Generate()
{
	m_NoiseValues.resize(m_DSLength);
	for (int j = 0; j < m_DSLength; j++)
	{
		m_NoiseValues[j].resize(m_DSLength);
	}

	// Assign four corner values
	m_NoiseValues[0][0] = (float)std::rand() * m_MaxInv;
	m_NoiseValues[0][m_DSLength - 1] = (float)std::rand() * m_MaxInv;
	m_NoiseValues[m_DSLength - 1][m_DSLength - 1] = (float)std::rand() * m_MaxInv;
	m_NoiseValues[m_DSLength - 1][0] = (float)std::rand() * m_MaxInv;

	DiamondSquare(m_StartingN, 0, m_DSLength - 1, 0, m_DSLength - 1);

}

void HeightMapDiamondSquare::DiamondSquare(int n, int x0, int x1, int y0, int y1)
{
	int offset = pow(2, n - 1);

	DiamondStep(n, x0, x1, y0, y1);
	SquareStep(n, x0, x1, y0, y1);

	// Recurse
	if (n == 1)
		return;
	DiamondSquare(n - 1, x0, x0 + offset, y0, y0 + offset); // Bottom-left
	DiamondSquare(n - 1, x0 + offset, x1, y0, y0 + offset); // Bottom-right
	DiamondSquare(n - 1, x0, x0 + offset, y0 + offset, y1); // Top-left
	DiamondSquare(n - 1, x0 + offset, x1, y0 + offset, y1); // Top-right
}

void HeightMapDiamondSquare::DiamondStep(int n, int x0, int x1, int y0, int y1)
{
	// Calculate center point for diamond step
	m_NoiseValues[(y1 + y0) / 2][(x1 + x0) / 2] = (m_NoiseValues[y0][x0] + m_NoiseValues[y0][x1] + m_NoiseValues[y1][x1] + m_NoiseValues[y1][x0] + std::rand() * m_MaxInv) / 5;
}

void HeightMapDiamondSquare::SquareStep(int n, int x0, int x1, int y0, int y1)
{

	// Calculate points for square step
	int offset = pow(2, n - 1);
	int horizMidpoint = x0 + offset;
	int vertMidpoint = y0 + offset;

	// Bottom
	int avgDivisor = 4;
	m_NoiseValues[y0][horizMidpoint] += m_NoiseValues[vertMidpoint][horizMidpoint];
	m_NoiseValues[y0][horizMidpoint] += m_NoiseValues[y0][x0];
	m_NoiseValues[y0][horizMidpoint] += m_NoiseValues[y0][x1];
	m_NoiseValues[y0][horizMidpoint] += std::rand() * m_MaxInv;

	if (y0 != 0)
	{
		m_NoiseValues[y0][horizMidpoint] += m_NoiseValues[y0 - offset][horizMidpoint];
		avgDivisor++;
	}
	m_NoiseValues[y0][horizMidpoint] /= avgDivisor;
	
	// Top
	avgDivisor = 4;
	m_NoiseValues[y1][horizMidpoint] += m_NoiseValues[vertMidpoint][horizMidpoint];
	m_NoiseValues[y1][horizMidpoint] += m_NoiseValues[y1][x0];
	m_NoiseValues[y1][horizMidpoint] += m_NoiseValues[y1][x1];
	m_NoiseValues[y1][horizMidpoint] += std::rand() * m_MaxInv;

	if (y1 != pow(2, m_StartingN))
	{
		m_NoiseValues[y1][horizMidpoint] += m_NoiseValues[y1 + offset][horizMidpoint];
			avgDivisor++;
	}
	m_NoiseValues[y1][horizMidpoint] /= avgDivisor;

	// Left
	avgDivisor = 4;
	m_NoiseValues[vertMidpoint][x0] += m_NoiseValues[vertMidpoint][horizMidpoint];
	m_NoiseValues[vertMidpoint][x0] += m_NoiseValues[y0][x0];
	m_NoiseValues[vertMidpoint][x0] += m_NoiseValues[y1][x0];
	m_NoiseValues[vertMidpoint][x0] += std::rand() * m_MaxInv;

	if (x0 != 0)
	{
		m_NoiseValues[vertMidpoint][x0] += m_NoiseValues[vertMidpoint][x0 - offset];
		avgDivisor++;
	}
	m_NoiseValues[vertMidpoint][x0] /= avgDivisor;

	// Right
	avgDivisor = 4;
	m_NoiseValues[vertMidpoint][x1] += m_NoiseValues[vertMidpoint][horizMidpoint];
	m_NoiseValues[vertMidpoint][x1] += m_NoiseValues[y0][x1];
	m_NoiseValues[vertMidpoint][x1] += m_NoiseValues[y1][x1];
	m_NoiseValues[vertMidpoint][x1] += std::rand() * m_MaxInv;

	if (x0 != pow(2, m_StartingN))
	{
		m_NoiseValues[vertMidpoint][x1] += m_NoiseValues[vertMidpoint][x0 + offset];
		avgDivisor++;
	}
	m_NoiseValues[vertMidpoint][x1] /= avgDivisor;
}