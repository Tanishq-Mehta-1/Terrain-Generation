#ifndef EROSION_UNIFORMS_H
#define EROSION_UNIFORMS_H

struct Erosion_Uniforms {
	 float dt; //timestep
	 float K_rain = 0.01; //rain_amount
	 float flux_pipe_cross_section = 1.0f;
	 float flux_pipe_length = 1.0f;
	 float K_gravity = 9.8f;
	 float K_capacity = 1.0f; //sediment capacity constant
	 float K_dissolving = 1.0f; //sediment erosion constant
	 float K_deposition = 1.0f; //sediment deposition constant
	 float K_evaporation = 0.1f;
};


#endif