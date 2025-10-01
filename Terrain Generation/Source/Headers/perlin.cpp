#include <glm/glm.hpp>
#include <iostream>
#include "perlin.h"

//randomly arranged 256 integers
const int permutation[] = { 151,160,137,91,90,15,
	131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
	190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
	88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
	77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
	102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
	135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
	5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
	223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
	129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
	251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
	49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
	138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};


double Perlin::fade(const double& t) {
	return t * t * t * (t * (6 * t - 15) + 10);
}

double Perlin::lerp(double a, double b, double t) {
	return a + t * (b - a);
}

int Perlin::inc(int num) {
	num++;
	if (num > repeat)
		num = num % repeat;

	return num;
}

double Perlin::grad(int hash, double x, double y) {
	int h = hash & 7;
	double u = (h < 4) ? x : y;
	double v = (h < 4) ? y : x;

	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

Perlin::Perlin(int period) {
	repeat = period;

	//generate 512 elements in p to avoid buffer overflow
	for (int i = 0; i < 512; i++) {
		p[i] = permutation[i % 256];
	}
}

double Perlin::perlin(double x, double y) {
	if (repeat > 0) {
		x = fmod(x, repeat);
		y = fmod(y, repeat);
	}

	//extract int [range 0,255] and float
	int xi = (int)x & 255;
	int yi = (int)y & 255;
	double xf = x - (int)x;
	double yf = y - (int)y;

	double u = fade(xf);
	double v = fade(yf);

	//hashing
	int aa, ab, ba, bb;
	aa = p[p[xi] + yi];
	ab = p[p[xi] + inc(yi)];
	ba = p[p[inc(xi)] + yi];
	bb = p[p[inc(xi)] + inc(yi)];

	double x1, x2, res;
	x1 = lerp(grad(aa, xf, yf), grad(ba, xf - 1, yf), u);
	x2 = lerp(grad(ab, xf, yf - 1), grad(bb, xf - 1, yf - 1), u);

	res = lerp(x1, x2, v);

	return (res + 1) / 2.0;
}

double Perlin::octavePerlin(double x, double y, int octaves, double persistence) {
	double total = 0;
	double frequency = 1;
	double amplitude = 1;
	double maxValue = 0;
	for (int i = 0; i < octaves; i++) {
		total += Perlin::perlin(x * frequency, y * frequency) * amplitude;

		maxValue += amplitude;
		amplitude *= persistence;
		frequency *= 2;
	}

	return total / maxValue;
}

double Perlin::octavePerlin(glm::vec2 vec, int octaves, double persistence) {
	return octavePerlin(vec.x, vec.y, octaves, persistence);
}

double Perlin::DW_Perlin(glm::vec2 p, float octaves, float persistence) {
	glm::vec2 q = glm::vec2(
		octavePerlin(p + glm::vec2(0.0, 0.0), octaves, persistence),
		octavePerlin(p + glm::vec2(5.2, 1.3), octaves, persistence)
	);

	glm::vec2 r = glm::vec2(
		octavePerlin(p + 4.0f * q + glm::vec2(1.7, 9.2), octaves, persistence),
		octavePerlin(p + 4.0f * q + glm::vec2(8.3, 2.8), octaves, persistence)
	);

	return octavePerlin(p + 4.0f * r, octaves, persistence);
}

