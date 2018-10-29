#include <stdio.h>
#include <stdint.h>

#include "include/superchunk_size.h"
#include "include/superchunk.h"
#include "include/chunk.h"
#include "include/save.h"

void file_name_insert(char *dest, const char *src, int n)
{
	while(n--)
	{
		*dest++ = *src++;
	}
}

int save_world()
{
	FILE *voxel_world_save_file;
	voxel_world_save_file = fopen("world.bin", "wb");
	//I can completely zero the file
	if (!voxel_world_save_file)
	{
		printf("[ERROR] Unable to open file!\n");
		return 1;
	}
	
	struct save_file_info_t voxel_world_info = { 0 };
	struct chunk_save_data_t chunk_save_data = { 0 };
	file_name_insert(voxel_world_info.file_type, "VOXELWORLD", 10);
	voxel_world_info.file_type_version = FILE_VERSION;
	voxel_world_info.superchunk_x_dimention = SCX;
	voxel_world_info.superchunk_y_dimention = SCY;
	voxel_world_info.superchunk_z_dimention = SCZ;
	voxel_world_info.chunk_x_dimention = CX;
	voxel_world_info.chunk_y_dimention = CY;
	voxel_world_info.chunk_z_dimention = CZ;

	//Write voxel_world_info to file
	fwrite(&voxel_world_info, sizeof(struct save_file_info_t), 1, voxel_world_save_file);

	//Have to get chunk and block data through the superchunk aka the world
	//and get the chunk name ready
	file_name_insert(chunk_save_data.name, "CHNK", 4);
	for(int x = 0; x < SCX; x++)
	{
		for(int y = 0; y < SCY; y++)
		{
			for(int z = 0; z < SCZ; z++)
			{
				//Record chunk and write it to file
				chunk_save_data.ax = x;
				chunk_save_data.ay = y;
				chunk_save_data.az = z;
				//Now write to file
				fwrite(&chunk_save_data, sizeof(struct chunk_save_data_t), 1, voxel_world_save_file);
				//Now ge the block data from the chunk
				block_save_for_chunk(x, y, z, voxel_world_save_file);
			}
		}
	}
	fclose(voxel_world_save_file);
	return 0;
}

void block_save_for_chunk(int chunk_x, int chunk_y, int chunk_z, FILE *voxel_world_save_file)
{
	struct block_save_data_t block_save_data = { 0 };
	
	file_name_insert(block_save_data.name, "BLOCK", 5);

	for(int x = 0; x < CX; x++)
	{
		for(int y = 0; y < CY; y++)
		{
			for(int z = 0; z < CZ; z++)
			{

				block_save_data.type = world->get_c_entry(chunk_x, chunk_y, chunk_z)->get(x, y, z);
				fwrite(&block_save_data, sizeof(struct block_save_data_t), 1, voxel_world_save_file);

			}
		}
	}
}
