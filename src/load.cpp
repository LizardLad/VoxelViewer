#include <stdio.h>
#include <stdint.h>

#include "include/superchunk_size.h"
#include "include/superchunk.h"
#include "include/chunk.h"
#include "include/save.h"
#include "include/load.h"
#include "include/render.h"

int get_world_dimentions_from_file(int32_t *SCX, int32_t *SCY, int32_t *SCZ, int32_t *CX, int32_t *CY, int *CZ, const char *file_name)
{
	FILE *voxel_world_save_file;
	voxel_world_save_file = fopen(file_name, "rb");
	if(!voxel_world_save_file)
	{
		printf("[ERROR] Unable to open file to read!\n[ERROR] Error occured trying to read world dimentions from file!\n");
		*SCX = 32;
		*SCY = 2;
		*SCZ = 32;
		*CX = 16;
		*CY = 32;
		*CZ = 16;
		return 1;
	}

	struct save_file_info_t voxel_world_info = { 0 };
	fread(&voxel_world_info, sizeof(struct save_file_info_t), 1, voxel_world_save_file);
	if(strncmp(voxel_world_info.file_type, "VOXELWORLD", 10))
	{
		printf("[ERROR] Not a voxel world file\n");
		return 2;
	}
	if(voxel_world_info.file_type_version != FILE_VERSION)
	{
		printf("[ERROR] File version incorrect\n");
		return 3;
	}

	*SCX = voxel_world_info.superchunk_x_dimention;
	*SCY = voxel_world_info.superchunk_y_dimention;
	*SCZ = voxel_world_info.superchunk_z_dimention;
	*CX = voxel_world_info.chunk_x_dimention;
	*CY = voxel_world_info.chunk_y_dimention;
	*CZ = voxel_world_info.chunk_z_dimention;
	return 0;
}

int load_world()
{
	FILE *voxel_world_save_file;
	voxel_world_save_file = fopen("world.bin", "rb");
	if(!voxel_world_save_file)
	{
		printf("[ERROR] Unable to open file to read!\n[ERROR] Error occured trying to read world dimentions from file!\n");
		return 1;
	}

	//Still read the file info to check.
	//Just a safe guard
	struct save_file_info_t voxel_world_info = { 0 };
	fread(&voxel_world_info, sizeof(struct save_file_info_t), 1, voxel_world_save_file);
	if(strncmp(voxel_world_info.file_type, "VOXELWORLD", 10))
	{
		printf("[ERROR] Not a voxel world file\n");
		return 2;
	}
	if(voxel_world_info.file_type_version != FILE_VERSION)
	{
		printf("[ERROR] File version incorrect\n");
		return 3;
	}

	if(voxel_world_info.superchunk_x_dimention != SCX || \
	voxel_world_info.superchunk_y_dimention != SCY || \
	voxel_world_info.superchunk_z_dimention != SCZ || \
	voxel_world_info.chunk_x_dimention != CX || \
	voxel_world_info.chunk_y_dimention != CY || \
	voxel_world_info.chunk_z_dimention != CZ) //File has changed since last read it must have been modified
	{
		printf("[ERROR] File changed since last read!\n[ERROR] Please restart the program!\n");
		return 4;
	}
	//The file now looks ok to work with
	struct chunk_save_data_t chunk_save_data = { 0 };
	for(int x = 0; x < SCX; x++)
	{
		for(int y = 0; y < SCY; y++)
		{
			for(int z = 0; z < SCZ; z++)
			{
				fread(&chunk_save_data, sizeof(struct chunk_save_data_t), 1, voxel_world_save_file);
				if(strncmp(chunk_save_data.name, "CHNK", 4))
				{
					//Not a chunk
					printf("[ERROR] Expected chunk encountered something else!\n[ERROR] Please check file integrity\n!");
					return 5;
				}
				chunk_load(x, y, z, voxel_world_save_file);
			}
		}
	}
	fclose(voxel_world_save_file);
	world->set_loaded(true);
	render();
	printf("[INFO] Loaded!\n");
	return 0;
}
						

			


void chunk_load(int chunk_x, int chunk_y, int chunk_z, FILE *voxel_world_save_file)
{
	struct block_save_data_t block_save_data = { 0 };

	for(int x = 0; x < CX; x++)
	{
		for(int y = 0; y < CY; y++)
		{
			for(int z = 0; z < CZ; z++)
			{
				fread(&block_save_data, sizeof(struct block_save_data_t), 1, voxel_world_save_file);
				if(strncmp(block_save_data.name, "BLOCK", 5)) //Encountered data isn't a block
				{
					//Error
					printf("[ERROR] Data encountered isn't a block when expecting block!\n");
					exit(1);
					return;
				}
				world->get_c_entry(chunk_x, chunk_y, chunk_z)->set(x, y, z, block_save_data.type);
			}
		}
	}
	world->get_c_entry(chunk_x, chunk_y, chunk_z)->changed = true;
	world->get_c_entry(chunk_x, chunk_y, chunk_z)->noised = true;
	world->get_c_entry(chunk_x, chunk_y, chunk_z)->initialized = true;
}

