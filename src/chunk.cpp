#include <stdlib.h>

#include <SDL_opengles2.h>

#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>

#include "include/chunk.h"
#include "include/shader_utils.h"
#include "include/superchunk_size.h"
#include "include/global_vars.h"

//chunk *chunk_slot[CHUNKSLOTS] = {0};
chunk **chunk_slot; 

chunk::chunk(int x, int y, int z): ax(x), ay(y), az(z) {
	blk = (uint8_t *)malloc(sizeof(uint8_t) * CX * CY * CZ);
	memset(blk, 0, sizeof(uint8_t) * CX * CY * CZ);
	left = right = below = above = front = back = 0;
	lastused = now;
	slot = 0;
	changed = true;
	initialized = false;
	noised = false;
}

uint8_t chunk::get(int x, int y, int z) {
	if(x < 0)
		return left ? left->get_blk_entry(x + CX, y, z) : 0;
	if(x >= CX)
		return right ? right->get_blk_entry(x - CX, y, z) : 0;
	if(y < 0)
		return below ? below->get_blk_entry(x, y + CY, z) : 0;
	if(y >= CY)
		return above ? above->get_blk_entry(x, y - CY, z) : 0;
	if(z < 0)
		return front ? front->get_blk_entry(x, y, z + CZ) : 0;
	if(z >= CZ)
		return back ? back->get_blk_entry(x, y, z - CZ) : 0;
	return get_blk_entry(x, y, z);
}

bool chunk::isblocked(int x1, int y1, int z1, int x2, int y2, int z2) {
	// Invisible blocks are always "blocked"
	if(!get_blk_entry(x1, y1, z1))
		return true;
	
	// Leaves do not block any other block, including themselves
	if(transparent[get(x2, y2, z2)] == 1)
			return false;

	// Non-transparent blocks always block line of sight
	if(!transparent[get(x2, y2, z2)])
		return true;

	// Otherwise, LOS is only blocked by blocks if the same transparency type
	return transparent[get(x2, y2, z2)] == transparent[get_blk_entry(x1, y1, z1)];
}

void chunk::set(int x, int y, int z, uint8_t type) {
	// If coordinates are outside this chunk, find the right one.
	if(x < 0) {
		if(left)
			left->set(x + CX, y, z, type);
		return;
	}
	if(x >= CX) {
		if(right)
			right->set(x - CX, y, z, type);
		return;
	}
	if(y < 0) {
		if(below)
			below->set(x, y + CY, z, type);
		return;
	}
	if(y >= CY) {
		if(above)
			above->set(x, y - CY, z, type);
		return;
	}
	if(z < 0) {
		if(front)
			front->set(x, y, z + CZ, type);
		return;
	}
	if(z >= CZ) {
		if(back)
			back->set(x, y, z - CZ, type);
		return;
	}

	// Change the block
	set_blk_entry(x, y, z, type);
	changed = true;

	// When updating blocks at the edge of this chunk,
	// visibility of blocks in the neighbouring chunk might change.
	if(x == 0 && left)
		left->changed = true;
	if(x == CX - 1 && right)
		right->changed = true;
	if(y == 0 && below)
		below->changed = true;
	if(y == CY - 1 && above)
		above->changed = true;
	if(z == 0 && front)
		front->changed = true;
	if(z == CZ - 1 && back)
		back->changed = true;
}

float chunk::noise2d(float x, float y, int seed, int octaves, float persistence) {
	float sum = 0;
	float strength = 1.0;
	float scale = 0.0000000025;

	for(int i = 0; i < octaves; i++) {
		sum += strength * glm::simplex(glm::vec2(x, y) * (scale * ((float)seed)));
		scale *= 2.0;
		strength *= persistence;
	}

	return sum;
}

float chunk::noise3d_abs(float x, float y, float z, int seed, int octaves, float persistence) {
	float sum = 0;
	float strength = 1.0;
	//float scale = 0.0000000025;
	float scale = 0.0L;

	for(int i = 0; i < octaves; i++) {
		sum += strength * fabs(glm::simplex(glm::vec3(x, y, z) * (scale * ((float)seed))));
		scale *= 2.0;
		strength *= persistence;
	}

	return sum;
}

void chunk::noise(int seed) {
	if(noised)
		return;
	else
		noised = true;

	for(int x = 0; x < CX; x++) {
		for(int z = 0; z < CZ; z++) {
			// Land height
			float n = noise2d((x + ax * CX) / 256.0, (z + az * CZ) / 256.0, seed, 5, 0.8) * 4;
			int h = n * 2;
			int y = 0;

			// Land blocks
			for(y = 0; y < CY; y++) {
				// Are we above "ground" level?
				if(y + ay * CY >= h) {
					// If we are not yet up to sea level, fill with water blocks
					if(y + ay * CY < SEALEVEL) {
						set_blk_entry(x, y, z, 8);
						continue;
					// Otherwise, we are in the air
					} else {
						// A tree!
						if(get(x, y - 1, z) == 3 && (rand() & 0xff) == 0) {
							// Trunk
							h = (rand() & 0x3) + 3;
							for(int i = 0; i < h; i++)
								set(x, y + i, z, 5);

							// Leaves
							for(int ix = -3; ix <= 3; ix++) { 
								for(int iy = -3; iy <= 3; iy++) { 
									for(int iz = -3; iz <= 3; iz++) { 
										if(ix * ix + iy * iy + iz * iz < 8 + (rand() & 1) && !get(x + ix, y + h + iy, z + iz))
											set(x + ix, y + h + iy, z + iz, 4);
									}
								}
							}
						}
						break;
					}
				}

				// Random value used to determine land type
				float r = this->noise3d_abs((x + ax * CX) / 16.0, (y + ay * CY) / 16.0, (z + az * CZ) / 16.0, -seed, 2, 1);

				// Sand layer
				if(n + r * 5 < 4)
					set(x, y, z, 7);
				// Dirt layer, but use grass blocks for the top
				else if(n + r * 5 < 8)
					set(x, y, z, ((h < SEALEVEL || y + ay * CY < h - 1) ? 1 : 3));
				// Rock layer
				else if(r < 1.25)
					set(x, y, z, 6);
				// Sometimes, ores!
				else
					set(x, y, z, 11);
			}
		}
	}
	changed = true;
}

void chunk::update() {
	byte4 vertex[CX * CY * CZ * 18];
	int i = 0;
	int merged = 0;
	bool vis = false;;

	// View from negative x
	for(int x = CX - 1; x >= 0; x--) {
		for(int y = 0; y < CY; y++) {
			for(int z = 0; z < CZ; z++) {
				// Line of sight blocked?
				if(isblocked(x, y, z, x - 1, y, z)) {
					vis = false;
					continue;
				}

				uint8_t top = get_blk_entry(x, y, z);
				uint8_t bottom = get_blk_entry(x, y, z);
				uint8_t side = get_blk_entry(x, y, z);

				// Grass block has dirt sides and bottom
				if(top == 3) {
					bottom = 1;
					side = 2;
				// Wood blocks have rings on top and bottom
				} else if(top == 5) {
					top = bottom = 12;
				}

				// Same block as previous one? Extend it.
				if(vis && z != 0 && get_blk_entry(x, y, z) == get_blk_entry(x, y, z - 1))
				{
					vertex[i - 5] = byte4(x, y, z + 1, side);
					vertex[i - 2] = byte4(x, y, z + 1, side);
					vertex[i - 1] = byte4(x, y + 1, z + 1, side);
					merged++;
				// Otherwise, add a new quad.
				}
				else
				{
					vertex[i++] = byte4(x, y, z, side);
					vertex[i++] = byte4(x, y, z + 1, side);
					vertex[i++] = byte4(x, y + 1, z, side);
					vertex[i++] = byte4(x, y + 1, z, side);
					vertex[i++] = byte4(x, y, z + 1, side);
					vertex[i++] = byte4(x, y + 1, z + 1, side);
				}
				
				vis = true;
			}
		}
	}

	// View from positive x
	for(int x = 0; x < CX; x++) {
		for(int y = 0; y < CY; y++) {
			for(int z = 0; z < CZ; z++) {
				if(isblocked(x, y, z, x + 1, y, z)) {
					vis = false;
					continue;
				}

				uint8_t top = get_blk_entry(x, y, z);
				uint8_t bottom = get_blk_entry(x, y, z);
				uint8_t side = get_blk_entry(x, y, z);

				if(top == 3) {
					bottom = 1;
					side = 2;
				} else if(top == 5) {
					top = bottom = 12;
				}

				if(vis && z != 0 && get_blk_entry(x, y, z) == get_blk_entry(x, y, z - 1)) {
					vertex[i - 4] = byte4(x + 1, y, z + 1, side);
					vertex[i - 2] = byte4(x + 1, y + 1, z + 1, side);
					vertex[i - 1] = byte4(x + 1, y, z + 1, side);
					merged++;
				} else {
					vertex[i++] = byte4(x + 1, y, z, side);
					vertex[i++] = byte4(x + 1, y + 1, z, side);
					vertex[i++] = byte4(x + 1, y, z + 1, side);
					vertex[i++] = byte4(x + 1, y + 1, z, side);
					vertex[i++] = byte4(x + 1, y + 1, z + 1, side);
					vertex[i++] = byte4(x + 1, y, z + 1, side);
				}
				vis = true;
			}
		}
	}

	// View from negative y
	for(int x = 0; x < CX; x++) {
		for(int y = CY - 1; y >= 0; y--) {
			for(int z = 0; z < CZ; z++) {
				if(isblocked(x, y, z, x, y - 1, z)) {
					vis = false;
					continue;
				}

				uint8_t top = get_blk_entry(x, y, z);
				uint8_t bottom = get_blk_entry(x, y, z);

				if(top == 3) {
					bottom = 1;
				} else if(top == 5) {
					top = bottom = 12;
				}

				if(vis && z != 0 && get_blk_entry(x, y, z) == get_blk_entry(x, y, z - 1)) {
					vertex[i - 4] = byte4(x, y, z + 1, bottom + 128);
					vertex[i - 2] = byte4(x + 1, y, z + 1, bottom + 128);
					vertex[i - 1] = byte4(x, y, z + 1, bottom + 128);
					merged++;
				} else {
					vertex[i++] = byte4(x, y, z, bottom + 128);
					vertex[i++] = byte4(x + 1, y, z, bottom + 128);
					vertex[i++] = byte4(x, y, z + 1, bottom + 128);
					vertex[i++] = byte4(x + 1, y, z, bottom + 128);
					vertex[i++] = byte4(x + 1, y, z + 1, bottom + 128);
					vertex[i++] = byte4(x, y, z + 1, bottom + 128);
				}
				vis = true;
			}
		}
	}

	// View from positive y
	for(int x = 0; x < CX; x++) {
		for(int y = 0; y < CY; y++) {
			for(int z = 0; z < CZ; z++) {
				if(isblocked(x, y, z, x, y + 1, z)) {
					vis = false;
					continue;
				}

				uint8_t top = get_blk_entry(x, y, z);
				uint8_t bottom = get_blk_entry(x, y, z);

				if(top == 3) {
					bottom = 1;
				} else if(top == 5) {
					top = bottom = 12;
				}

				if(vis && z != 0 && get_blk_entry(x, y, z) == get_blk_entry(x, y, z - 1)) {
					vertex[i - 5] = byte4(x, y + 1, z + 1, top + 128);
					vertex[i - 2] = byte4(x, y + 1, z + 1, top + 128);
					vertex[i - 1] = byte4(x + 1, y + 1, z + 1, top + 128);
					merged++;
				} else {
					vertex[i++] = byte4(x, y + 1, z, top + 128);
					vertex[i++] = byte4(x, y + 1, z + 1, top + 128);
					vertex[i++] = byte4(x + 1, y + 1, z, top + 128);
					vertex[i++] = byte4(x + 1, y + 1, z, top + 128);
					vertex[i++] = byte4(x, y + 1, z + 1, top + 128);
					vertex[i++] = byte4(x + 1, y + 1, z + 1, top + 128);
				}
				vis = true;
			}
		}
	}

	// View from negative z
	for(int x = 0; x < CX; x++) {
		for(int z = CZ - 1; z >= 0; z--) {
			for(int y = 0; y < CY; y++) {
				if(isblocked(x, y, z, x, y, z - 1)) {
					vis = false;
					continue;
				}

				uint8_t top = get_blk_entry(x, y, z);
				uint8_t bottom = get_blk_entry(x, y, z);
				uint8_t side = get_blk_entry(x, y, z);

				if(top == 3) {
					bottom = 1;
					side = 2;
				} else if(top == 5) {
					top = bottom = 12;
				}

				if(vis && y != 0 && get_blk_entry(x, y, z) == get_blk_entry(x, y - 1, z)) {
					vertex[i - 5] = byte4(x, y + 1, z, side);
					vertex[i - 3] = byte4(x, y + 1, z, side);
					vertex[i - 2] = byte4(x + 1, y + 1, z, side);
					merged++;
				} else {
					vertex[i++] = byte4(x, y, z, side);
					vertex[i++] = byte4(x, y + 1, z, side);
					vertex[i++] = byte4(x + 1, y, z, side);
					vertex[i++] = byte4(x, y + 1, z, side);
					vertex[i++] = byte4(x + 1, y + 1, z, side);
					vertex[i++] = byte4(x + 1, y, z, side);
				}
				vis = true;
			}
		}
	}

	// View from positive z
	for(int x = 0; x < CX; x++) {
		for(int z = 0; z < CZ; z++) {
			for(int y = 0; y < CY; y++) {
				if(isblocked(x, y, z, x, y, z + 1)) {
					vis = false;
					continue;
				}

				uint8_t top = get_blk_entry(x, y, z);
				uint8_t bottom = get_blk_entry(x, y, z);
				uint8_t side = get_blk_entry(x, y, z);

				if(top == 3) {
					bottom = 1;
					side = 2;
				} else if(top == 5) {
					top = bottom = 12;
				}

				if(vis && y != 0 && get_blk_entry(x, y, z) == get_blk_entry(x, y - 1, z)) {
					vertex[i - 4] = byte4(x, y + 1, z + 1, side);
					vertex[i - 3] = byte4(x, y + 1, z + 1, side);
					vertex[i - 1] = byte4(x + 1, y + 1, z + 1, side);
					merged++;
				} else {
					vertex[i++] = byte4(x, y, z + 1, side);
					vertex[i++] = byte4(x + 1, y, z + 1, side);
					vertex[i++] = byte4(x, y + 1, z + 1, side);
					vertex[i++] = byte4(x, y + 1, z + 1, side);
					vertex[i++] = byte4(x + 1, y, z + 1, side);
					vertex[i++] = byte4(x + 1, y + 1, z + 1, side);
				}
				vis = true;
			}
		}
	}

	changed = false;
	elements = i;

	// If this chunk is empty, no need to allocate a chunk slot.
	if(!elements)
		return;

	// If we don't have an active slot, find one
	if(chunk_slot[slot] != this) {
		int lru = 0;
		for(int i = 0; i < CHUNKSLOTS; i++) {
			// If there is an empty slot, use it
			if(!chunk_slot[i]) {
				lru = i;
				break;
			}
			// Otherwise try to find the least recently used slot
			if(chunk_slot[i]->lastused < chunk_slot[lru]->lastused)
				lru = i;
		}

		// If the slot is empty, create a new VBO
		if(!chunk_slot[lru]) {
			glGenBuffers(1, &vbo);
		// Otherwise, steal it from the previous slot owner
		} else {
			vbo = chunk_slot[lru]->vbo;
			chunk_slot[lru]->changed = true;
		}

		slot = lru;
		chunk_slot[slot] = this;
	}

	// Upload vertices

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, i * sizeof *vertex, vertex, GL_STATIC_DRAW);
}

void chunk::render() {
	if(changed)
		update();

	lastused = now;

	if(!elements)
		return;

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(attribute_coord, 4, GL_BYTE, GL_FALSE, 0, 0);
	glDrawArrays(GL_TRIANGLES, 0, elements);
}

inline uint8_t chunk::get_blk_entry(int x, int y, int z)
{
	return blk[(x) + (CX * y) + (z * CX * CY)];
}

inline void chunk::set_blk_entry(int x, int y, int z, uint8_t type)
{
	blk[(x) + (CX * y) + (z * CX * CY)] = type;
}


