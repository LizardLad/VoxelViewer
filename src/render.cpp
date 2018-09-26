#include <SDL.h>
#include <SDL_opengles2.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>

#include "include/global_vars.h"
#include "include/shader_utils.h"
#include "include/superchunk.h"

void render() {
	glm::mat4 view = glm::lookAt(position, position + lookat, up);
	glm::mat4 projection = glm::perspective(45.0f, 1.0f*ww/wh, 0.01f, 1000.0f);

	glm::mat4 mvp = projection * view;

	glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_POLYGON_OFFSET_FILL);

	/* Then draw chunks */

	world->render(mvp);

	/* At which voxel are we looking? */
	/* Very naive ray casting algorithm to find out which block we are looking at */

	glm::vec3 testpos = position;
	glm::vec3 prevpos = position;

	for(int i = 0; i < 100; i++) {
		/* Advance from our currect position to the direction we are looking at, in small steps */

		prevpos = testpos;
		testpos += lookat * 0.1f;

		mx = floorf(testpos.x);
		my = floorf(testpos.y);
		mz = floorf(testpos.z);

		/* If we find a block that is not air, we are done */

		if(world->get(mx, my, mz))
			break;
	}

	/* Find out which face of the block we are looking at */

	int px = floorf(prevpos.x);
	int py = floorf(prevpos.y);
	int pz = floorf(prevpos.z);

	if(px > mx)
		face = 0;
	else if(px < mx)
		face = 3;
	else if(py > my)
		face = 1;
	else if(py < my)
		face = 4;
	else if(pz > mz)
		face = 2;
	else if(pz < mz)
		face = 5;

	/* If we are looking at air, move the cursor out of sight */

	if(!world->get(mx, my, mz))
		mx = my = mz = 99999;

	float bx = mx;
	float by = my;
	float bz = mz;

	/* Render a box around the block we are pointing at */

	float box[24][4] = {
		{bx + 0, by + 0, bz + 0, 14},
		{bx + 1, by + 0, bz + 0, 14},
		{bx + 0, by + 1, bz + 0, 14},
		{bx + 1, by + 1, bz + 0, 14},
		{bx + 0, by + 0, bz + 1, 14},
		{bx + 1, by + 0, bz + 1, 14},
		{bx + 0, by + 1, bz + 1, 14},
		{bx + 1, by + 1, bz + 1, 14},

		{bx + 0, by + 0, bz + 0, 14},
		{bx + 0, by + 1, bz + 0, 14},
		{bx + 1, by + 0, bz + 0, 14},
		{bx + 1, by + 1, bz + 0, 14},
		{bx + 0, by + 0, bz + 1, 14},
		{bx + 0, by + 1, bz + 1, 14},
		{bx + 1, by + 0, bz + 1, 14},
		{bx + 1, by + 1, bz + 1, 14},

		{bx + 0, by + 0, bz + 0, 14},
		{bx + 0, by + 0, bz + 1, 14},
		{bx + 1, by + 0, bz + 0, 14},
		{bx + 1, by + 0, bz + 1, 14},
		{bx + 0, by + 1, bz + 0, 14},
		{bx + 0, by + 1, bz + 1, 14},
		{bx + 1, by + 1, bz + 0, 14},
		{bx + 1, by + 1, bz + 1, 14},
	};

	glDisable(GL_POLYGON_OFFSET_FILL);
	glDisable(GL_CULL_FACE);
	glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
	glBindBuffer(GL_ARRAY_BUFFER, cursor_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_LINES, 0, 24);

	/* Draw a cross in the center of the screen */

	float cross[4][4] = {
		{-0.05, 0, 0, 13},
		{+0.05, 0, 0, 13},
		{0, -0.05, 0, 13},
		{0, +0.05, 0, 13},
	};

	glDisable(GL_DEPTH_TEST);
	glm::mat4 one(1);
	glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(one));
	glBufferData(GL_ARRAY_BUFFER, sizeof cross, cross, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_LINES, 0, 4);

	/* And we are done */
}
