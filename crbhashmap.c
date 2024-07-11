#include "crbhashmap.h"
#include "clib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xxhash.h"

#define C_RB_HASH_MAP_INIT_CAPACITY	64
#define C_RB_HASH_MAP_LOAD_FACTOR	0.5
#define C_RB_HASH_MAP_NODE_LEN	sizeof(c_rb_hash_node)
#define C_RB_HASH_MIX	0

static void re_hashing(c_rb_hash_map* pmap)
{
	printf("re_hashing\n");

	int newCapacity = pmap->capacity << 1;

	int len = C_RB_HASH_MAP_NODE_LEN * newCapacity;
	c_rb_hash_node* newSlots = (c_rb_hash_node*)malloc(len);
	memset(newSlots, 0x00, len);

	c_rb_hash_node* slots = pmap->slots;
	int capacity = pmap->capacity;

	pmap->slots = newSlots;
	pmap->capacity = newCapacity;

	pmap->count = 0;
	pmap->max_psl = 0;

	for (int i = 0, j = capacity; i < j; i++)
	{
		c_rb_hash_node* pnode = &slots[i];

		if (pnode->inserted)
		{
			c_rb_hash_map_insert(pmap, pnode->key, pnode->value);
		}
	}

	free(slots);
}

c_rb_hash_map* c_rb_hash_map_new(int capacity, int value_free, void* (*addValue)(void))
{
	int len = sizeof(c_rb_hash_map);
	c_rb_hash_map* pmap = (c_rb_hash_map*)malloc(len);
	memset(pmap, 0x00, len);

	c_rb_hash_map_init(pmap, capacity, value_free, addValue);
	return pmap;
}

void c_rb_hash_map_init(c_rb_hash_map* pmap, int capacity, int value_free, void* (*addValue)(void))
{
	pmap->capacity = capacity;
	pmap->value_free = value_free;
	pmap->addValue = addValue;

	if (pmap->capacity < 1)
	{
		pmap->capacity = C_RB_HASH_MAP_INIT_CAPACITY;
	}

	int len = C_RB_HASH_MAP_NODE_LEN * pmap->capacity;
	pmap->slots = (c_rb_hash_node*)malloc(len);
	memset(pmap->slots, 0x00, len);

	pmap->count = 0;
	pmap->max_psl = 0;
}

void c_rb_hash_map_destroy(c_rb_hash_map* pmap)
{
	c_rb_hash_map_clear(pmap);

	if (pmap->slots != NULL)
	{
		free(pmap->slots);
		pmap->slots = NULL;
	}
}

void c_rb_hash_map_move(c_rb_hash_map* dst, c_rb_hash_map* src)
{
	dst->slots = src->slots;
	dst->capacity = src->capacity;
	dst->count = src->count;
	dst->max_psl = src->max_psl;
	dst->value_free = src->value_free;
	dst->addValue = src->addValue;
	dst->old_value = src->old_value;

	c_rb_hash_map_init(src, C_RB_HASH_MAP_INIT_CAPACITY, src->value_free, src->addValue);
}

void c_rb_hash_map_swap(c_rb_hash_map* dst, c_rb_hash_map* src)
{
	char* slots = dst->slots;
	int capacity = dst->capacity;
	int count = dst->count;
	int max_psl = dst->max_psl;
	int value_free = dst->value_free;
	void* (*addValue)(void) = dst->addValue;
	char* old_value = dst->old_value;

	dst->slots = src->slots;
	dst->capacity = src->capacity;
	dst->count = src->count;
	dst->max_psl = src->max_psl;
	dst->value_free = src->value_free;
	dst->addValue = src->addValue;
	dst->old_value = src->old_value;

	src->slots = slots;
	src->capacity = capacity;
	src->count = count;
	src->max_psl = max_psl;
	src->value_free = value_free;
	src->addValue = addValue;
	src->old_value = old_value;
}

void c_rb_hash_iter_init(c_rb_hash_iter* piter, c_rb_hash_map* pmap)
{
	piter->pmap = pmap;
	piter->slot_index = -1;
}

int c_rb_hash_iter_next(c_rb_hash_iter* piter, char** key, void** value)
{
	if (piter->slot_index == piter->pmap->capacity)
	{
		return 0;
	}

	while (++piter->slot_index < piter->pmap->capacity)
	{
		c_rb_hash_node* pnode = &piter->pmap->slots[piter->slot_index];

		if (pnode->inserted)
		{
			c_rb_hash_node* pnode = &piter->pmap->slots[piter->slot_index];
			*key = pnode->key;
			*value = pnode->value;
			return 1;
		}
	}

	return 0;
}

void c_rb_hash_iter_remove(c_rb_hash_iter* piter)
{
	if (piter->slot_index == piter->pmap->capacity)
	{
		return;
	}

	c_rb_hash_node* pnode = &piter->pmap->slots[piter->slot_index];

	if (piter->pmap->value_free)
	{
		free(pnode->value);
		pnode->value = NULL;
	}

	pnode->inserted = 0;
	piter->pmap->count--;
}

void c_rb_hash_map_insert(c_rb_hash_map* pmap, const char* key, void* value)
{
	if (C_RB_HASH_MAP_LOAD_FACTOR < ((pmap->count * 1.0) / pmap->capacity))
	{
		re_hashing(pmap);
	}

	int hash = XXH64(key, strlen(key), C_RB_HASH_MIX) % pmap->capacity;
	c_rb_hash_node* pnode = &pmap->slots[hash];
	strncpy(pmap->temp, key, C_RB_HASH_NODE_KEY_LEN);
	int psl = 0;

	while (1)
	{
		if (!pnode->inserted)
		{
			strcpy(pnode->key, pmap->temp);
			pnode->value = value;
			pnode->psl = psl;
			pnode->inserted = 1;

			if (psl > pmap->max_psl)
			{
				pmap->max_psl = psl;
			}

			pmap->count++;
			break;
		}
		else if (pnode->psl >= psl)
		{
			if (strcmp(pnode->key, pmap->temp) == 0)
			{
				if (pmap->value_free)
				{
					free(pnode->value);
				}

				pnode->value = value;
				break;
			}
			else
			{
				psl++;
				hash = (hash + 1) % pmap->capacity;
				pnode = &pmap->slots[hash];
				continue;
			}
		}
		else
		{
			strncpy(pmap->old_key, pnode->key, C_RB_HASH_NODE_KEY_LEN);
			pmap->old_value = pnode->value;

			strncpy(pnode->key, pmap->temp, C_RB_HASH_NODE_KEY_LEN);
			pnode->value = value;
			pnode->psl = psl;

			if (psl > pmap->max_psl)
			{
				pmap->max_psl = psl;
			}

			strncpy(pmap->temp, pmap->old_key, C_RB_HASH_NODE_KEY_LEN);
			value = pmap->old_value;
		}

		psl++;
		hash = (hash + 1) % pmap->capacity;
		pnode = &pmap->slots[hash];
	}
}

c_rb_hash_node* c_rb_hash_map_find(c_rb_hash_map* pmap, const char* key)
{
	int hash = XXH64(key, strlen(key), C_RB_HASH_MIX) % pmap->capacity;
	c_rb_hash_node* pnode = &pmap->slots[hash];
	int psl = 0;

	while (pnode->psl >= psl || psl < pmap->max_psl)
	{
		if (pnode->inserted)
		{
			if (strcmp(pnode->key, key) == 0)
			{
				return pnode;
			}
		}

		psl++;
		hash = (hash + 1) % pmap->capacity;
		pnode = &pmap->slots[hash];
	}

	return NULL;
}

int c_rb_hash_map_remove(c_rb_hash_map* pmap, const char* key)
{
	int hash = XXH64(key, strlen(key), C_RB_HASH_MIX) % pmap->capacity;
	c_rb_hash_node* pnode = &pmap->slots[hash];
	int psl = 0;

	while (pnode->psl >= psl || psl < pmap->max_psl)
	{
		if (pnode->inserted)
		{
			if (strcmp(pnode->key, key) == 0)
			{
				if (pmap->value_free)
				{
					free(pnode->value);
					pnode->value = NULL;
				}

				pnode->inserted = 0;

				pmap->count--;
				return 1;
			}
		}

		psl++;
		hash = (hash + 1) % pmap->capacity;
		pnode = &pmap->slots[hash];
	}

	return 0;
}

c_rb_hash_node* c_rb_hash_map_insert_or_find(c_rb_hash_map* pmap, const char* key)
{
	if (C_RB_HASH_MAP_LOAD_FACTOR < ((pmap->count * 1.0) / pmap->capacity))
	{
		re_hashing(pmap);
	}

	int hash = XXH64(key, strlen(key), C_RB_HASH_MIX) % pmap->capacity;
	c_rb_hash_node* pnode = &pmap->slots[hash];
	strncpy(pmap->temp, key, C_RB_HASH_NODE_KEY_LEN);
	int psl = 0;

	c_rb_hash_node* newNode = NULL;
	int changed = 0;
	void* value = NULL;

	while (1)
	{
		if (!pnode->inserted)
		{
			strcpy(pnode->key, pmap->temp);

			if (!changed)
			{
				pnode->value = pmap->addValue();
			}
			else
			{
				pnode->value = value;
			}

			pnode->psl = psl;
			pnode->inserted = 1;

			if (psl > pmap->max_psl)
			{
				pmap->max_psl = psl;
			}

			pmap->count++;

			if (!changed)
			{
				return pnode;
			}

			break;
		}
		else if (pnode->psl >= psl)
		{
			if (strcmp(pnode->key, pmap->temp) == 0)
			{
				return pnode;
			}
			else
			{
				psl++;
				hash = (hash + 1) % pmap->capacity;
				pnode = &pmap->slots[hash];
				continue;
			}
		}
		else
		{

			strncpy(pmap->old_key, pnode->key, C_RB_HASH_NODE_KEY_LEN);
			pmap->old_value = pnode->value;

			strncpy(pnode->key, pmap->temp, C_RB_HASH_NODE_KEY_LEN);

			if (!changed)
			{
				pnode->value = pmap->addValue();
			}
			else
			{
				pnode->value = value;
			}

			pnode->psl = psl;

			if (psl > pmap->max_psl)
			{
				pmap->max_psl = psl;
			}

			if (newNode == NULL)
			{
				newNode = pnode;
			}

			changed = 1;
			strncpy(pmap->temp, pmap->old_key, C_RB_HASH_NODE_KEY_LEN);
			value = pmap->old_value;
		}

		psl++;
		hash = (hash + 1) % pmap->capacity;
		pnode = &pmap->slots[hash];
	}

	return newNode;
}

void c_rb_hash_map_clear(c_rb_hash_map* pmap)
{
	if (pmap->value_free)
	{
		for (int slot_index = 0; slot_index < pmap->capacity; slot_index++)
		{
			c_rb_hash_node* pnode = &pmap->slots[slot_index];

			if (pnode->inserted)
			{
				free(pnode->value);
			}
		}
	}

	memset(pmap->slots, 0x00, C_RB_HASH_MAP_NODE_LEN * pmap->capacity);

	pmap->count = 0;
	pmap->max_psl = 0;
}
