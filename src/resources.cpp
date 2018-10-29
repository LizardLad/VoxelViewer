#include <time.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_opengles2.h>

#include <glm/glm.hpp>

#include "include/global_vars.h"
#include "include/shader_utils.h"
#include "include/textures.h"
#include "include/superchunk.h"
#include "include/vectors.h"
#include "include/load.h"
#include "include/chunk.h"
int32_t CHUNKSLOTS = 0;
int32_t SCX = 0, SCY = 0, SCZ = 0, CX = 0, CY = 0, CZ = 0;

int init_resources()
{
	/* Seed random number generator */
	srand(time(NULL));
	
	/* Create shaders */

	program = create_program("shader/vertex.glsl", "shader/fragment.glsl");

	if(program == 0)
		return 0;

	attribute_coord = get_attrib(program, "coord");
	uniform_mvp = get_uniform(program, "mvp");

	if(attribute_coord == -1 || uniform_mvp == -1)
		return 0;

	/* Create and upload the texture */

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textures.width, textures.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textures.pixel_data);
	glGenerateMipmap(GL_TEXTURE_2D);

	//From file get dimentions
	get_world_dimentions_from_file(&SCX, &SCY, &SCZ, &CX, &CY, &CZ, "world.bin");

	CHUNKSLOTS = SCX * SCY * SCZ;

	chunk_slot = (chunk **)malloc(sizeof(chunk *) * CHUNKSLOTS);
	memset((void *)chunk_slot, 0, sizeof(chunk *) * CHUNKSLOTS);

	/* Create the world */

	world = new superchunk;

	load_world();
	
	position = glm::vec3(0, CY + 1, 0);
	angle = glm::vec3(0, -0.5, 0);
	update_vectors();

	/* Create a VBO for the cursor */

	glGenBuffers(1, &cursor_vbo);

	/* OpenGL settings that do not change while running this program */

	glUseProgram(program);
	glUniform1i(uniform_texture, 0);
	glClearColor(0.6, 0.8, 1.0, 0.0);
	glEnable(GL_CULL_FACE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Use GL_NEAREST_MIPMAP_LINEAR if you want to use mipmaps
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glPolygonOffset(1, 1);

	glEnableVertexAttribArray(attribute_coord);

	return 1;
}

void free_resources()
{
	free(chunk_slot);
	glDeleteProgram(program);
}
