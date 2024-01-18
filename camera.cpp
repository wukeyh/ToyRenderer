#include "camera.h"


Camera::Camera(vec3f position, vec3f target, float aspect) : position(position), target(target), aspect(aspect)
{
	forward = (target - position).normalize();
	right = (forward ^ vec3f(0, 1, 0)).normalize();
	up = (right ^ forward).normalize();
}

Camera::~Camera() {}

void Camera::camera_orbit_update()
{
	vec2f orbit_delta = window->mouse_info.orbit_delta;
	float orbit_speed = 0.01f;
	float orbit_x = orbit_delta.x * orbit_speed;
	float orbit_y = orbit_delta.y * orbit_speed;

	vec3f new_target = target - position;
	//new_target = new_target.rotate(up, orbit_x);
	//new_target = new_target.rotate(right, orbit_y);
	target = position + new_target;
}

void Camera::camera_set_transform(Camera& camera, vec3f pos, vec3f target)
{
	camera.position = pos;
	camera.target = target;
	camera.forward = (target - pos).normalize();
	camera.right = (camera.forward ^ vec3f(0, 1, 0)).normalize();
	camera.up = (camera.right ^ camera.forward).normalize();
}


void handle_mouse_events(Camera& camera)
{
	window->mouse_info.orbit_delta = vec2f(0, 0);
}

void handle_keyboard_events(Camera& camera)
{

}

void handle_events(Camera& camera)
{
}
