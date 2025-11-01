#ifndef EROSION_UNIFORMS_H
#define EROSION_UNIFORMS_H

struct Erosion_Uniforms {
	float p_inertia;      // resistance to direction change
	float p_min_slope;     // minimal slope threshold
	float p_capacity;     // sediment capacity multiplier
	float p_deposition;      // deposition rate
	float p_erosion;      // erosion rate
	float p_gravity;      // gravity factor
	float p_evaporation;   // evaporation per step
	float p_radius;      // erosion radius
	int   p_max_iteration;       // maximum iterations per droplet
};


#endif