#ifndef LOAD_H
#define LOAD_H

#include <stdint.h>

int get_world_dimentions_from_file(int32_t *SCX, int32_t *SCY, int32_t *SCZ, int32_t *CX, int32_t *CY, int *CZ, const char *file_name);
int load_world();
void chunk_load(int chunk_x, int chunk_y, int chunk_z, FILE *voxel_world_save_file);

#endif
