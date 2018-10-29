#ifndef SUPERCHUNK_H
#define SUPERCHUNK_H

#include <SDL_opengles2.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>

#include "shader_utils.h"
#include "superchunk_size.h"
#include "chunk.h"

class superchunk
{
	public:
		//chunk *c[SCX][SCY][SCZ]; //TODO
		chunk **c; //TODO
		int seed;
		superchunk();
		uint8_t get(int x, int y, int z);
		void set(int x, int y, int z, uint8_t type);
		void render(const glm::mat4 &pv);
		chunk *get_c_entry(int x, int y, int z);
		void set_loaded(bool loaded_in);
	private:
		bool loaded;
		void set_c_entry(int x, int y, int z, chunk *chunk_p);
};

extern superchunk *world;

#endif
