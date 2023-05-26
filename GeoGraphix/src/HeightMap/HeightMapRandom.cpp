#include "HeightMapRandom.h"

HeightMapRandom::HeightMapRandom(int width, int length)
	: HeightMap(width, length)
{
	Generate();
}

void HeightMapRandom::Generate()
{
	// set rng seed using current time
	auto time = std::chrono::system_clock::now();
	auto since_epoch = time.time_since_epoch();
	auto seed = std::chrono::duration_cast<std::chrono::milliseconds>(since_epoch);
	std::srand(seed.count());

	float maxinv = 1 / (float)RAND_MAX;

	for (int i = 0; i < m_Width * m_Length; i++)
	{
		m_Map[i] = (float) std::rand() * maxinv;
	}
}
