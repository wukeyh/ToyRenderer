#pragma once

#include "platform/win32.h"

class Camera
{
public:
	Camera(vec3f position, vec3f target, float aspect);
	~Camera();

	void camera_orbit_update();
	void camera_set_transform(Camera& camera, vec3f pos, vec3f target);

	vec3f get_position() const { return position; }
	vec3f get_target() const { return target; }
	vec3f get_forward() const { return forward; }

private:

	vec3f position;			// eye position in world space 
	vec3f target;
	vec3f forward;
	vec3f up;
	vec3f right;
	float aspect;
};


void handle_events(Camera& camera);