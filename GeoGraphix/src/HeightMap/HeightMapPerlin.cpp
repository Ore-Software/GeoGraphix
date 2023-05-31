#include "HeightMapPerlin.h"
#include "../external/glm/common.hpp"
#include "../external/glm/ext/vector_float2.hpp"
#include "../external/glm/trigonometric.hpp"
#include "../external/glm/geometric.hpp"

HeightMapPerlin::HeightMapPerlin(int width, int length)
	: HeightMap(width, length)
{
	Generate();
}

float HeightMapPerlin::smooth_step(float f)
{
	f = abs(f);
	if (f == 0)
		return 0;
	else if (f >= 1)
		return 1;
	else
	{
		return 6 * pow(f, 5) - 15 * pow(f, 4) + 10 * pow(f, 3);
	}
}

float HeightMapPerlin::mix(float x, float y, float a)
{
	return x * (1 - a) + y * a;
}

glm::vec3 HeightMapPerlin::smooth_step(glm::vec3 f3)
{
	return glm::vec3(smooth_step(f3.x), smooth_step(f3.y), smooth_step(f3.z));
}

glm::vec2 HeightMapPerlin::random2(glm::vec3 seed)
{
	glm::vec2 S = glm::vec2(glm::dot(seed, glm::vec3(127.1, 311.7, 783.089)),
		glm::dot(seed, glm::vec3(269.5, 183.3, 173.542)));
	return glm::fract(glm::vec2(glm::sin(S.x) * 43758.5453123, glm::sin(S.y) * 43758.5453123));
}

glm::vec3 HeightMapPerlin::random_direction(glm::vec3 seed)
{
	// use random2 and spherical coords
	glm::vec2 angles = random2(seed);
	return glm::vec3(
		glm::sin(M_PI * angles.x) * glm::cos(2 * M_PI * angles.y),
		glm::sin(M_PI * angles.x) * glm::sin(2 * M_PI * angles.y),
		glm::cos(M_PI * angles.x)
	);
}

float HeightMapPerlin::noise(glm::vec3 seed)
{
	glm::vec3 int_part = glm::floor(seed);
	glm::vec3 frac_part = glm::fract(seed);

	glm::vec3 u = smooth_step(frac_part); // use improved smooth step

	float x0y0z0 = glm::dot(random_direction(int_part + glm::vec3(0.0, 0.0, 0.0)), frac_part - glm::vec3(0.0, 0.0, 0.0));
	float x1y0z0 = glm::dot(random_direction(int_part + glm::vec3(1.0, 0.0, 0.0)), frac_part - glm::vec3(1.0, 0.0, 0.0));

	float x0y1z0 = glm::dot(random_direction(int_part + glm::vec3(0.0, 1.0, 0.0)), frac_part - glm::vec3(0.0, 1.0, 0.0));
	float x1y1z0 = glm::dot(random_direction(int_part + glm::vec3(1.0, 1.0, 0.0)), frac_part - glm::vec3(1.0, 1.0, 0.0));

	float x0y0z1 = glm::dot(random_direction(int_part + glm::vec3(0.0, 0.0, 1.0)), frac_part - glm::vec3(0.0, 0.0, 1.0));
	float x1y0z1 = glm::dot(random_direction(int_part + glm::vec3(1.0, 0.0, 1.0)), frac_part - glm::vec3(1.0, 0.0, 1.0));

	float x0y1z1 = glm::dot(random_direction(int_part + glm::vec3(0.0, 1.0, 1.0)), frac_part - glm::vec3(0.0, 1.0, 1.0));
	float x1y1z1 = glm::dot(random_direction(int_part + glm::vec3(1.0, 1.0, 1.0)), frac_part - glm::vec3(1.0, 1.0, 1.0));


	return
		mix(
			mix(
				mix(
					x0y0z0,
					x1y0z0,
					u.x),
				mix(
					x0y1z0,
					x1y1z0,
					u.x),
				u.y),
			mix(
				mix(
					x0y0z1,
					x1y0z1,
					u.x),
				mix(x0y1z1,
					x1y1z1,
					u.x),
				u.y),
			u.z);
}

void HeightMapPerlin::Generate()
{
	for (int i = 0; i < m_Length; i++)
	{
		for (int j = 0; j < m_Width; j++)
		{
			int index = i * m_Width + j;
			float height = noise(glm::vec3(j, i, 0.5));
			m_Map[index] = height + 0.5f;
		}
	}
}