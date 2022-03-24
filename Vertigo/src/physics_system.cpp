// internal
#include "physics_system.hpp"
#include "world_init.hpp"

// standard
#include <math.h> 
#define PI 3.14159265

bool in_between(vec3 a, vec3 b1, vec3 b2){
	float x0 = min(b1[0], b2[0]);
	float x1 = max(b1[0], b2[0]);
	float y0 = min(b1[1], b2[1]);
	float y1 = max(b1[1], b2[1]);
	float z0 = min(b1[2], b2[2]);
	float z1 = max(b1[2], b2[2]);
	if (a[0] > x0 && a[0] < x1 && a[1] > y0 && a[1] < y1 && a[2] > z0 && a[2] < z1){
		return true;
	}
	return false;
}

// This is a SUPER APPROXIMATE check
bool collides(mat4 model1, mat4 model2)
{	
	vec4 o1 = vec4(-0.5f, -0.5f, -0.5f, 1.f);
	vec4 o2 = vec4(0.5f, 0.5f, 0.5f, 1.f);
	vec4 coord0 = model1[3]; // model1 * o0;
	vec4 coord1 = model2[3] - vec4(0.5f, 0.5f, 0.5f, 0);
	vec4 coord2 = model2[3] + vec4(0.5f, 0.5f, 0.5f, 0);
	printf("FIRE: \t  %.4f || %.4f || %.4f || %.4f\n", coord0[0], coord0[1], coord0[2], coord0[3]);
	printf("CORNER1: \t %.4f || %.4f || %.4f || %.4f\n", coord1[0], coord1[1], coord1[2], coord1[3]);
	printf("CORNER2: \t %.4f || %.4f || %.4f || %.4f\n", coord2[0], coord2[1], coord2[2], coord2[3]);
	printf("\n");
	if (in_between(coord0, coord1, coord2)){
		return true;
	}
	return false;
}

void PhysicsSystem::oscillate()
{
	auto& oscillate_registry = registry.oscillations;
	for (uint i = 0; i < oscillate_registry.size(); i++)
	{
		Oscillate& oscillate = oscillate_registry.components[i];
		Entity& e = oscillate_registry.entities[i];
		Motion& motion = registry.motions.get(e);
		oscillate.phase += 2 * PI / oscillate.steps;
		oscillate.phase = fmod(oscillate.phase, 2 * PI);
		motion.position = oscillate.center + oscillate.amplitude * vec3(sin(oscillate.phase));
		
	}
}

void PhysicsSystem::step(float elapsed_ms)
{
	// Used for oscillation
	PhysicsSystem::oscillate();

	// Move bug based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.
	auto& motion_registry = registry.motions;
	for (uint i = 0; i < motion_registry.size(); i++)
	{
		Motion& motion = motion_registry.components[i];
		Entity entity = motion_registry.entities[i];
		float step_seconds = elapsed_ms / 1000.f;

		if (motion.interpolate){
			if (elapsed_ms > motion.remaining_time){
				motion.position = motion.destination;
				motion.remaining_time = 0;
			}
			else{
				motion.position = motion.position + (motion.destination - motion.position) * (elapsed_ms / motion.remaining_time);
				if (motion.move_z) {
					motion.position.z = 2*sin((PI*motion.remaining_time)/500.f);
				}
				motion.remaining_time -= elapsed_ms;
			}
		}
		else{
			motion.velocity = motion.velocity + motion.acceleration * step_seconds;
			motion.position = motion.position + motion.velocity * step_seconds;
		}
		
	}

	// Check for collisions between only fire and animated tiles right now
	ComponentContainer<Motion>& motion_container = registry.motions;
	for (uint i = 0; i < registry.fire.components.size(); i++)
	{
		Entity entity_i = registry.fire.entities[i];
		Fire fire = registry.fire.components[i];

		if (!fire.active || !fire.inUse){
			continue;
		}
		Object object_i = registry.objects.get(entity_i);
		Motion motion_i = motion_container.get(entity_i);
		vec3 position = motion_i.position;

		if (position.z < 0){
			position = position - (position.z / motion_i.velocity.z * motion_i.velocity);
		}

		mat4 trans1 = translate(mat4(1.0f), position);
		mat4 model1 = object_i.model;

		mat4 model2;

		for (uint j = 0; j < registry.objects.components.size(); j++)
		{
			Entity entity_j = registry.objects.entities[j];
			Object object_j = registry.objects.components[j];
			if (registry.fire.has(entity_j)){
				continue;
			}
			model2 = object_j.model;

			if (collides(trans1 * model1, model2)){
				// Create a collisions event
				// Will always be fire first
				registry.collisions.emplace_with_duplicates(entity_i, entity_j);
			}
		}
	}

	// you may need the following quantities to compute wall positions
	(float)window_width_px; (float)window_height_px;
}