#ifndef PERLIN
#define PERLIN

#include <glm/glm.hpp>

class Perlin {

public:

	//periodicity,how often the noise repeats itself
	int repeat;

	Perlin();
	Perlin(int period);
	double perlin(double x, double y);
	double octavePerlin(double x, double y, int octaves, double persistence);
	double octavePerlin(glm::vec2 vec, int octaves, double persistence);
	double DW_Perlin(glm::vec2 p, float octaves, float persistence);
	double static fade(const double& t);
	double static lerp(double a, double b, double t);

private:
	int p[512];

	int inc(int num);
	double grad(int hash, double x, double y);

};

#endif // !PERLIN
