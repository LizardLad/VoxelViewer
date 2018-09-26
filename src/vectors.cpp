#include <glm/glm.hpp>

#include "include/global_vars.h"

// Calculate the forward, right and lookat vectors from the angle vector
void update_vectors()
{
	forward.x = sinf(angle.x);
	forward.y = 0;
	forward.z = cosf(angle.x);

	right.x = -cosf(angle.x);
	right.y = 0;
	right.z = sinf(angle.x);

	lookat.x = sinf(angle.x) * cosf(angle.y);
	lookat.y = sinf(angle.y);
	lookat.z = cosf(angle.x) * cosf(angle.y);

	up = glm::cross(right, lookat);
}
