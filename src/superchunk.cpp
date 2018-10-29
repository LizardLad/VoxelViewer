#include <stdlib.h>

#include <SDL_opengles2.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "include/shader_utils.h"
#include "include/superchunk_size.h"
#include "include/chunk.h"
#include "include/superchunk.h"
#include "include/load.h"

//int32_t SCX = 32;
//int32_t SCY = 2;
//int32_t SCZ = 32;

superchunk::superchunk() {
	c = (chunk **)malloc(sizeof(chunk *) * SCX * SCY * SCZ);
	
	seed = rand();//time(NULL);
	for(int x = 0; x < SCX; x++)
	{
		for(int y = 0; y < SCY; y++)
		{
			for(int z = 0; z < SCZ; z++)
			{
				chunk *temp = new chunk(x - SCX / 2, y - SCY / 2, z - SCZ / 2);
				set_c_entry(x, y, z, temp);
			}
		}
	}

	for(int x = 0; x < SCX; x++)
	{
		for(int y = 0; y < SCY; y++)
		{
			for(int z = 0; z < SCZ; z++)
			{
				if(x > 0)
					get_c_entry(x, y, z)->left = get_c_entry(x - 1, y, z);
				if(x < SCX - 1)
					get_c_entry(x, y, z)->right = get_c_entry(x + 1, y, z);
				if(y > 0)
					get_c_entry(x, y, z)->below = get_c_entry(x, y - 1, z);
				if(y < SCY - 1)
					get_c_entry(x, y, z)->above = get_c_entry(x, y + 1, z);
				if(z > 0)
					get_c_entry(x, y, z)->front = get_c_entry(x, y, z - 1);
				if(z < SCZ - 1)
					get_c_entry(x, y, z)->back = get_c_entry(x, y, z + 1);
			}
		}
	}
}

uint8_t superchunk::get(int x, int y, int z) {
	int cx = (x + CX * (SCX / 2)) / CX;
	int cy = (y + CY * (SCY / 2)) / CY;
	int cz = (z + CZ * (SCZ / 2)) / CZ;

	if(cx < 0 || cx >= SCX || cy < 0 || cy >= SCY || cz <= 0 || cz >= SCZ)
		return 0;

	return get_c_entry(cx, cy, cz)->get(x & (CX - 1), y & (CY - 1), z & (CZ - 1));
}

void superchunk::set(int x, int y, int z, uint8_t type) {
	int cx = (x + CX * (SCX / 2)) / CX;
	int cy = (y + CY * (SCY / 2)) / CY;
	int cz = (z + CZ * (SCZ / 2)) / CZ;

	if(cx < 0 || cx >= SCX || cy < 0 || cy >= SCY || cz <= 0 || cz >= SCZ)
		return;

	get_c_entry(cx, cy, cz)->set(x & (CX - 1), y & (CY - 1), z & (CZ - 1), type);
}

void superchunk::render(const glm::mat4 &pv) {
	float ud = 1.0 / 0.0;
	int ux = -1;
	int uy = -1;
	int uz = -1;

	for(int x = 0; x < SCX; x++) {
		for(int y = 0; y < SCY; y++) {
			for(int z = 0; z < SCZ; z++) {
				glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(get_c_entry(x, y, z)->ax * CX, get_c_entry(x, y, z)->ay * CY, get_c_entry(x, y, z)->az * CZ));
				glm::mat4 mvp = pv * model;

				// Is this chunk on the screen?
				glm::vec4 center = mvp * glm::vec4(CX / 2, CY / 2, CZ / 2, 1);

				float d = glm::length(center);
				center.x /= center.w;
				center.y /= center.w;

				// If it is behind the camera, don't bother drawing it
				if(center.z < -CY / 2)
					continue;

				// If it is outside the screen, don't bother drawing it
				if(fabsf(center.x) > 1 + fabsf(CY * 2 / center.w) || fabsf(center.y) > 1 + fabsf(CY * 2 / center.w))
					continue;

				// If this chunk is not initialized, skip it
				if(!get_c_entry(x, y, z)->initialized) {
					// But if it is the closest to the camera, mark it for initialization
					if(ux < 0 || d < ud) {
						ud = d;
						ux = x;
						uy = y;
						uz = z;
					}
					continue;
				}

				glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));

				get_c_entry(x, y, z)->render();
			}
		}
	}

	if(ux >= 0 && loaded == false) {
		get_c_entry(ux, uy, uz)->noise(seed);
		if(get_c_entry(ux, uy, uz)->left)
			get_c_entry(ux, uy, uz)->left->noise(seed);
		if(get_c_entry(ux, uy, uz)->right)
			get_c_entry(ux, uy, uz)->right->noise(seed);
		if(get_c_entry(ux, uy, uz)->below)
			get_c_entry(ux, uy, uz)->below->noise(seed);
		if(get_c_entry(ux, uy, uz)->above)
			get_c_entry(ux, uy, uz)->above->noise(seed);
		if(get_c_entry(ux, uy, uz)->front)
			get_c_entry(ux, uy, uz)->front->noise(seed);
		if(get_c_entry(ux, uy, uz)->back)
			get_c_entry(ux, uy, uz)->back->noise(seed);
		get_c_entry(ux, uy, uz)->initialized = true;
	}
}

inline chunk* superchunk::get_c_entry(int x, int y, int z)
{
	return c[(x) + (SCX * y) + (z * SCX * SCY)];
}

inline void superchunk::set_c_entry(int x, int y, int z, chunk *chunk_p)
{
	c[(x) + (SCX * y) + (z * SCX * SCY)] = chunk_p;
}

void superchunk::set_loaded(bool loaded_in)
{
	loaded = loaded_in;
}
