#ifndef SAVE_H
#define SAVE_H

int save_world();
void block_save_for_chunk(int chunk_x, int chunk_y, int chunk_z, FILE *voxel_world_save_file);

#endif
