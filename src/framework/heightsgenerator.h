#pragma once

#include <cstdlib>
#include <ctime>
#include <noise/noise.h>
#include <algorithm>
#include <iostream>
#include <vector>

const double pi = 3.14159265357;

using namespace noise;

class HeightsGenerator {
	private:
		const float AMPLITUDE = 1;
		const int OCTAVES = 3;
		float ROUGHNESS = 0.3f;
		int seed;

		float minX, maxX; // min/max x value for the vertices
		float minZ, maxZ; // min/max z value for the vertices

		// The dimensions of the noise map
		float noiseWidth;
		float noiseHeight;

		std::vector<std::vector<float>> noises;
		std::vector<std::vector<float>> heights;

		module::Perlin perlinModule;
	public:
		HeightsGenerator(float noiseWidth, float noiseHeight, float minX, float maxX, float miZ, float maxZ)
			: noiseWidth(noiseWidth), noiseHeight(noiseHeight), minX(minX), maxX(maxX), minZ(minZ), maxZ(maxZ)
		{
			perlinModule.SetFrequency(3);

			this->noiseWidth = 10;
			this->noiseHeight = 10;

			noises.resize(this->noiseWidth + 1);
			heights.resize(this->noiseWidth + 1);
			for(int i = 0; i < this->noiseWidth; i++) {
				noises[i].resize(this->noiseHeight + 1);
				heights[i].resize(this->noiseHeight + 1);
			}
			
			build();
		}

		float generateHeight(int x, int z) {
			float total = 0;
			/*
			float d = (float)pow(2, OCTAVES - 1);
			for(int i = 0; i < OCTAVES; i++) {
				float freq = (float)(pow(2, i) / d);
				float amp = (float)pow(ROUGHNESS, i) * AMPLITUDE;
				total += getInterpolatedNoise(x * freq, z * freq) * amp;
			} */

			total = getInterpolatedNoise(x, z);

			//total = getInterpolatedNoise(x / 4.0f, z / 4.0f) * AMPLITUDE;
			//total += getInterpolatedNoise(x / 2.0f, z / 2.0f) * AMPLITUDE / 3.0f;
			//total += getInterpolatedNoise(x, z) * AMPLITUDE / 9.0f;
				 
			return total;
		}

		float getSmoothNoise(int x, int z) {
			float corners = getNoise(x-1, z-1) + getNoise(x+1, z-1) + getNoise(x - 1, z + 1) + getNoise(x + 1, z + 1) / 16.0f;
			float sides = getNoise(x - 1, z) + getNoise(x + 1, z) + getNoise(x, z - 1) + getNoise(x, z + 1) / 8.0f;

			float center = getNoise(x, z) / 4.0f;

			return corners + sides + center;
		}


		/*float getInterpolatedNoise(float x, float z) {
			int intX = (int)x;
			int intZ = (int)z;
			float fracX = x - intX;
			float fracZ = z - intZ;

			float v1 = getSmoothNoise(intX, intZ);
			float v2 = getSmoothNoise(intX + 1, intZ);
			float v3 = getSmoothNoise(intX, intZ + 1);
			float v4 = getSmoothNoise(intX + 1, intZ + 1);
			float i1 = interpolate(v1, v2, fracX);
			float i2 = interpolate(v1, v2, fracX);

			return interpolate(i1, i2, fracZ);
		}*/

		float getInterpolatedNoise(int x, int z) {
			return getHeight(x, z);
		}

		float getHeight(int x, int z) {
			if(x < 0 || x >= noiseWidth || z < 0 || z >= noiseHeight) {
				return 0;
			} else {
				return heights.at(x).at(z);
			}
		}

		float interpolate(float a, float b, float blend) {
			double theta = blend * pi;
			float f = (float)(1.0f - cos(theta)) * 0.5f;
			return a * (1.0f - f) + b * f;
		}

		float getNoise(int x, int z) {
			return perlinModule.GetValue(x, 0, z);
		}

		float calcNoiseMapValue(float x, float z) {
			double xExtent = maxX - minX + 1.1234f;
			double zExtent = maxZ - minZ + 3.234f;
			double xDelta = xExtent / (double)noiseWidth; // 
			double zDelta = zExtent / (double)noiseHeight;
			double xCurrent = minX;
			double zCurrent = minZ;

			xCurrent += xDelta * x;
			zCurrent += zDelta * z;

			double swVal, seVal, nwVal, neVal;
			swVal = perlinModule.GetValue(xCurrent,			  0, zCurrent);
			seVal = perlinModule.GetValue(xCurrent + xExtent, 0, zCurrent);
			nwVal = perlinModule.GetValue(xCurrent,			  0, zCurrent + zExtent);
			neVal = perlinModule.GetValue(xCurrent + xExtent, 0, zCurrent + zExtent);

			double xBlend = 1.0 - ((xCurrent - minX) / xExtent);
			double zBlend = 1.0 - ((zCurrent - minZ) / zExtent);
			
			double z0 = linearInterp(swVal, seVal, xBlend);
			double z1 = linearInterp(nwVal, neVal, xBlend);
			float finalValue = (float)linearInterp(z0, z1, zBlend);

			return finalValue;
		}
	private:
		void build() {
			for(int z = 0; z < noiseHeight; z++) {
				for(int x = 0; x < noiseHeight; x++) {
					//noises.at(x).at(z) = perlinModule.GetValue(x + 0.5f, 0, z + 0.5f);
				}
			}

			double xExtent = maxX - minX + 0.3f;
			double zExtent = maxZ - minZ + 0.3f;
			double xDelta = xExtent / (double)noiseWidth; // 
			double zDelta = zExtent / (double)noiseHeight;
			double xCurrent = minX;
			double zCurrent = minZ;

			for(int z = 0; z < noiseHeight; z++) {
				xCurrent = minX;
				for(int x = 0; x < noiseWidth; x++) {
					double swVal, seVal, nwVal, neVal;
					swVal = perlinModule.GetValue(xCurrent,			  0, zCurrent);
					seVal = perlinModule.GetValue(xCurrent + xExtent, 0, zCurrent);
					nwVal = perlinModule.GetValue(xCurrent,			  0, zCurrent + zExtent);
					neVal = perlinModule.GetValue(xCurrent + xExtent, 0, zCurrent + zExtent);
					double xBlend = 1.0 - ((xCurrent - minX) / xExtent);
					double zBlend = 1.0 - ((zCurrent - minZ) / zExtent);
					double z0 = linearInterp(swVal, seVal, xBlend);
					double z1 = linearInterp(nwVal, neVal, xBlend);
					float finalValue = (float)linearInterp(z0, z1, zBlend);

					heights[x][z] = finalValue;

					xCurrent += xDelta;
				}

				zCurrent += zDelta;
			}
		}

		double linearInterp(double n0, double n1, double a) {
			return ((1.0 - a) * n0) + (a * n1);
		}
};
