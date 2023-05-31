#pragma once
#include "HeightMap.h"
#include "../external/glm/common.hpp"
#include "../external/glm/ext/vector_float2.hpp"
#include "../external/glm/ext/vector_float3.hpp"
#include "../external/glm/trigonometric.hpp"
#include "../external/glm/gtc/random.hpp"

#define M_PI 3.1415926535897932384626433

class HeightMapPerlin : public HeightMap
{
public:
	HeightMapPerlin(int width, int length);

	void Generate() override;

private:
	float mix(float x, float y, float a);

	float smooth_step(float f);

	glm::vec2 smooth_step(glm::vec2 f2);

	glm::vec2 random2(glm::vec3 seed);

	glm::vec3 random_direction(glm::vec3 seed);

	float noise(glm::vec3 seed);
};