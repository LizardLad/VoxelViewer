#ifndef CHUNK_H
#define CHUNK_H

#include <SDL_opengles2.h>

#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>

#include "shader_utils.h"
#include "superchunk_size.h"
#include "global_vars.h"

// Size of one chunk in blocks
#define CX 16
#define CY 32
#define CZ 16

// Sea level
#define SEALEVEL 4

// Number of VBO slots for chunks
#define CHUNKSLOTS (SCX * SCY * SCZ)

struct byte4 {
	uint8_t x, y, z, w;
	byte4() {}
	byte4(uint8_t x, uint8_t y, uint8_t z, uint8_t w): x(x), y(y), z(z), w(w) {}
};

static const int transparent[16] = {2, 0, 0, 0, 1, 0, 0, 0, 3, 4, 0, 0, 0, 0, 0, 0}; 
static const char *blocknames[16] = {
	"air", "dirt", "topsoil", "grass", "leaves", "wood", "stone", "sand",
	"water", "glass", "brick", "ore", "woodrings", "white", "black", "x-y"
};

static struct chunk *chunk_slot[CHUNKSLOTS] = {0};

class chunk {
	public:
		uint8_t blk[CX][CY][CZ];
		chunk *left, *right, *below, *above, *front, *back;
		int slot;
		GLuint vbo;
		int elements;
		time_t lastused;
		bool changed;
		bool noised;
		bool initialized;
		int ax;
		int ay;
		int az;
		
		chunk(int x, int y, int z); //Constructor function
		
		uint8_t get(int x, int y, int z);
		void set(int x, int y, int z, uint8_t type);
		void render();
		void update();
		void noise(int seed);
		
	private:
		float noise3d_abs(float x, float y, float z, int seed, int octaves, float persistence);
		float noise2d(float x, float y, int seed, int octaves, float persistence);
		bool isblocked(int x1, int y1, int z1, int x2, int y2, int z2);
};

#endif
