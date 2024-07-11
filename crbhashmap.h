/*
 * C RB Hash Map - Hash Map Implementation in C Language
 * Copyright (c) 2024 Eungsuk Jeon
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __C_RB_HASH_MAP_H
#define __C_RB_HASH_MAP_H

#define C_RB_HASH_NODE_KEY_LEN	32

#ifdef __cplusplus
extern "C"
{
#endif

	typedef struct _tag_c_rb_hash_node
	{
		char key[C_RB_HASH_NODE_KEY_LEN];
		void* value;
		int psl;
		int inserted;
	} c_rb_hash_node;

	typedef struct _tag_c_rb_hash_map
	{
		c_rb_hash_node* slots;
		int capacity;
		int count;
		int max_psl;
		int value_free;
		void* (*addValue)(void);
		char temp[C_RB_HASH_NODE_KEY_LEN];
		char old_key[C_RB_HASH_NODE_KEY_LEN];
		void* old_value;
	} c_rb_hash_map;

	typedef struct _tag_c_rb_hash_iter
	{
		c_rb_hash_map* pmap;
		int slot_index;
	} c_rb_hash_iter;

	c_rb_hash_map* c_rb_hash_map_new(int capacity, int value_free, void* (*addValue)(void));
	void c_rb_hash_map_init(c_rb_hash_map* pmap, int capacity, int value_free, void* (*addValue)(void));
	void c_rb_hash_map_destroy(c_rb_hash_map* pmap);
	void c_rb_hash_map_move(c_rb_hash_map* dst, c_rb_hash_map* src);
	void c_rb_hash_map_swap(c_rb_hash_map* dst, c_rb_hash_map* src);

	void c_rb_hash_iter_init(c_rb_hash_iter* piter, c_rb_hash_map* pmap);
	int c_rb_hash_iter_next(c_rb_hash_iter* piter, char** key, void** value);
	void c_rb_hash_iter_remove(c_rb_hash_iter* piter);

	void c_rb_hash_map_insert(c_rb_hash_map* pmap, const char* key, void* value);
	c_rb_hash_node* c_rb_hash_map_find(c_rb_hash_map* pmap, const char* key);
	int c_rb_hash_map_remove(c_rb_hash_map* pmap, const char* key);
	c_rb_hash_node* c_rb_hash_map_insert_or_find(c_rb_hash_map* pmap, const char* key);
	void c_rb_hash_map_clear(c_rb_hash_map* pmap);

#ifdef __cplusplus
}
#endif
#endif
