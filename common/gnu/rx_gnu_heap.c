

/****************************************************************************
*
*  common\gnu\rx_gnu_heap.c
*
*  Copyright (c) 2020-2024 ENSACO Solutions doo
*  Copyright (c) 2018-2019 Dusan Ciric
*
*
*  This file is part of {rx-platform}
*
*
*  {rx-platform} is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  {rx-platform} is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with {rx-platform}. It is also available in any {rx-platform} console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*
****************************************************************************/


#include "pch.h"

#include "../rx_common.h"




#define CHUNK_NUMBER 20
#define INITIAL_CHUNK_SIZE 0x100

#define MAX_CHUNKSIZE (8<<(CHUNK_NUMBER-1))

#ifndef RX_STARTUP_HEAP_SIZE
#define RX_STARTUP_HEAP_SIZE (1024*1024)
#endif


#define HEAP_NEXT_BUFFER_CLAIMING ((uint8_t*)1)

size_t heap_alloc = 8 * 1024 * 1024;
size_t heap_trigger = 0;
size_t heap_bucket_size = 0;
uint8_t* next_buffer = NULL;
size_t next_buffer_size = 0;
uint8_t* current_buffer = NULL;
size_t current_buffer_size = 0;
size_t next_free_index = 0;
slim_lock_t  heap_lock;
size_t total_heap_bytes = 0;
size_t used_heap_bytes = 0;


size_t initial_list_size = sizeof(uint8_t*) * INITIAL_CHUNK_SIZE;


void claim_from_memory(size_t size)
{
	if (next_buffer && next_buffer_size && next_buffer != HEAP_NEXT_BUFFER_CLAIMING)
	{
		current_buffer = next_buffer;
		current_buffer_size = next_buffer_size;
		next_free_index = 0;
		next_buffer = NULL;
		next_buffer_size = 0;
	}
	else
	{
		size_t total_to_commit = size;
		current_buffer_size = total_to_commit;
		current_buffer = (uint8_t*)rx_allocate_os_memory(total_to_commit);
		memzero(current_buffer, total_to_commit);// claim it in memory
		next_free_index = 0;
		total_heap_bytes += total_to_commit;
	}
}

uint8_t* alloc_from_buffer(size_t size)
{
	if (current_buffer)
	{
		if (next_free_index + size < current_buffer_size)
		{
			uint8_t* ret = &current_buffer[next_free_index];
			next_free_index += size;
			used_heap_bytes += size;
			return ret;
		}
		else
		{
			claim_from_memory(heap_alloc);
			return alloc_from_buffer(size);
		}
	}
	else
	{
		claim_from_memory(RX_STARTUP_HEAP_SIZE);
		return alloc_from_buffer(size);
	}
}


struct my_heap_chunk
{
	size_t size;
	size_t next_index;
	size_t occupied;
	size_t free;
	size_t max_buckets;
	size_t g_allocated[CHUNK_NUMBER];
	uint64_t** array;
};

struct my_heap_chunk g_chunks[CHUNK_NUMBER * 8];

RX_COMMON_API size_t rx_heap_house_keeping()
{
	uint8_t* temp_buffer;
	size_t to_alloc = 0;
	size_t total = 0;
	size_t used;
	size_t trigger;
	rx_slim_lock_aquire(&heap_lock);

	if (next_buffer == NULL)
	{
		used = used_heap_bytes;
		total = total_heap_bytes;
		trigger = heap_trigger;
	}

	rx_slim_lock_release(&heap_lock);

	// division outside of lock!!!!
	if (total && ((used << 10) / total) > trigger)
	{
		rx_slim_lock_aquire(&heap_lock);
		if (next_buffer == NULL)
		{
			to_alloc = heap_alloc;
			next_buffer = HEAP_NEXT_BUFFER_CLAIMING;
			next_buffer_size = 0;
		}
		rx_slim_lock_release(&heap_lock);

		if (to_alloc)
		{
			temp_buffer = (uint8_t*)rx_allocate_os_memory(to_alloc);
			memzero(temp_buffer, to_alloc);// claim it in memory

			rx_slim_lock_aquire(&heap_lock);

			if (next_buffer == NULL || next_buffer == HEAP_NEXT_BUFFER_CLAIMING)
			{
				next_buffer = temp_buffer;
				next_buffer_size = to_alloc;
				total_heap_bytes += to_alloc;
			}

			rx_slim_lock_release(&heap_lock);
		}
	}


	return to_alloc;
}

RX_COMMON_API int rx_heap_status(struct rx_platform_heap_status_t* buffer, size_t* total, size_t* used, size_t* trigger, size_t* alloc)
{
	rx_slim_lock_aquire(&heap_lock);
	if (buffer)
	{
		for (int i = 0; i < CHUNK_NUMBER; i++)
		{
			buffer[i].bucket_size = ((size_t)8 << i);
			buffer[i].free = g_chunks[i].free;
			buffer[i].occuupied = g_chunks[i].occupied;
			buffer[i].buckets_capacity = g_chunks[i].size;
			buffer[i].bucket_max_used = g_chunks[i].max_buckets;
		}
		buffer[CHUNK_NUMBER].bucket_size = 0;
		buffer[CHUNK_NUMBER].free = 0;
		buffer[CHUNK_NUMBER].occuupied = 0;
	}
	*total = total_heap_bytes;
	*used = used_heap_bytes;
	*trigger = heap_trigger;
	*alloc = heap_alloc;
	rx_slim_lock_release(&heap_lock);

	return RX_OK;
}

int initialize_chunk_list()
{
	return RX_OK;
}

int rx_init_heap(size_t initial_heap, size_t alloc_size, size_t trigger, size_t bucket_size)
{
	if (current_buffer == NULL)
	{
		////////////////////////////////////
		rx_slim_lock_create(&heap_lock);
	}
	rx_slim_lock_aquire(&heap_lock);
	if (current_buffer == NULL)
	{
		if (bucket_size == 0)
			bucket_size = INITIAL_CHUNK_SIZE;
		for (int i = 0; i < CHUNK_NUMBER; i++)
		{
			g_chunks[i].array = (uint64_t**)alloc_from_buffer(sizeof(uint64_t*) * bucket_size);
			g_chunks[i].size = bucket_size;
			g_chunks[i].next_index = 0;
			g_chunks[i].occupied = 0;
			g_chunks[i].free = 0;
			g_chunks[i].max_buckets = 0;

		}
	}
	if (initial_heap)
	{// this is real init so alloc to initial size
		if (alloc_size)
			heap_alloc = alloc_size;
		if (trigger)
			heap_trigger = trigger;
		if (bucket_size > INITIAL_CHUNK_SIZE)
		{
			for (int i = 0; i < CHUNK_NUMBER; i++)
			{
				struct my_heap_chunk* my_chunk = &g_chunks[i];
				if (my_chunk->size < bucket_size)
				{
					void* old_data = my_chunk->array;
					size_t new_size = bucket_size;
					my_chunk->array = (uint64_t**)alloc_from_buffer(new_size * sizeof(uint64_t*));
					memcpy(my_chunk->array, old_data, my_chunk->size * sizeof(uint64_t*));
					my_chunk->size = new_size;
				}

			}
		}

		if (initial_heap > total_heap_bytes)
		{
			next_buffer_size = initial_heap - total_heap_bytes;
			next_buffer = (uint8_t*)rx_allocate_os_memory(next_buffer_size);
			memzero(next_buffer, next_buffer_size);// claim it in memory
			total_heap_bytes += next_buffer_size;
		}

	}
	rx_slim_lock_release(&heap_lock);

	return RX_OK;
}
RX_COMMON_API void* rx_heap_alloc(size_t size)
{
	if (current_buffer == NULL)
	{// initialize us
		rx_init_heap(0, 0, 0, 0);
	}

	uint64_t* p = NULL;

	if (size < MAX_CHUNKSIZE)
	{
		uint64_t chunknum = 0;
		size_t tmp = size >> 3;// start with 8 bytes
		//RX_ASSERT(tmp != 0);
		while (tmp != 0 && chunknum != CHUNK_NUMBER)
		{
			tmp = tmp >> 1;
			chunknum++;
		}

		size_t bucket_size = ((size_t)8 << chunknum);
		rx_slim_lock_aquire(&heap_lock);

		struct my_heap_chunk* my_chunk = &g_chunks[chunknum];


		my_chunk->occupied += 1;

		if (my_chunk->next_index != 0)
		{// we have it
			p = my_chunk->array[--my_chunk->next_index];
			p++;// return address just one byte upper from us

			my_chunk->free -= 1;
		}
		else
		{// we don't have it jet

			p = (uint64_t*)alloc_from_buffer(bucket_size + sizeof(uint64_t));
			*p = chunknum;
			p++;// return address just one byte upper from us
		}
		rx_slim_lock_release(&heap_lock);

	}
	else
	{
		p = (uint64_t*)rx_allocate_os_memory(size + sizeof(uint64_t));
		*p = 0xff;
		p++;// return address just one byte upper from us
	}

	return p;
}
RX_COMMON_API int rx_heap_free(void* ptr)
{
	if (ptr == NULL)
		return 1;

	uint64_t* mem = (uint64_t*)ptr;
	mem--;

	uint64_t chunk = *mem;

	if (chunk != 0xff)
	{// return it to bucket

		rx_slim_lock_aquire(&heap_lock);

		struct my_heap_chunk* my_chunk = &g_chunks[chunk];

		my_chunk->array[my_chunk->next_index++] = mem;
		if (my_chunk->next_index == my_chunk->size)
		{// we should do realloc becouse we are out of limit
			void* old_data = my_chunk->array;
			size_t new_size = ((my_chunk->size) << 1);
			my_chunk->array = (uint64_t**)alloc_from_buffer(new_size * sizeof(uint64_t*));
			memcpy(my_chunk->array, old_data, my_chunk->size * sizeof(uint64_t*));
			my_chunk->size = new_size;
		}
		if (my_chunk->max_buckets < my_chunk->next_index)
			my_chunk->max_buckets = my_chunk->next_index;
		my_chunk->free += 1;

		my_chunk->occupied -= 1;

		rx_slim_lock_release(&heap_lock);
	}
	else
	{// this are not we do simple release
		rx_deallocate_os_memory(mem, 0);
	}
	return 1;
}


