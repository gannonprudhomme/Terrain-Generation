#pragma once

#include <noise/noise.h>

using namespace noise;

class PerlinHeightsGenerator {
	public:
		module::Perlin perlinModule;
	public:
		PerlinHeightsGenerator() {
			
		}

		float getHeight(int x, int z) {
			return perlinModule.GetValue(x + 0.5f, 0, z + 0.5f);
		}
};