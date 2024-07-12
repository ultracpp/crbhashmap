# crbhashmap

crbhashmap is a lightweight hash map implementation in C language. This project utilizes the Robin Hood hashing algorithm to support efficient key-value storage and retrieval. Robin Hood hashing minimizes key movement during insertion using linear probing, thereby enhancing search performance.

## Features

- **Dynamic Resizing**: Automatically expands capacity when the load factor exceeds a predefined threshold.
- **Hashing Algorithm**: Uses the XXH64 hash function for key hashing.
- **Iterators**: Supports iterators for easy traversal of stored elements.
- **Memory Management**: Provides options for managing values stored in the hash map.

## Usage

1. **Prerequisites**:
   - C compiler (e.g., GCC)
   - Standard C libraries (`stdio.h`, `stdlib.h`, `string.h`)

2. **Installation**:
   - Clone the repository: `git clone https://github.com/yourusername/c-rb-hash-map.git`
   - Navigate to the directory: `cd c-rb-hash-map`

3. **Integration**:
   - Include `"crbhashmap.h"` in your source files.
   - Link with `"crbhashmap.c"` during compilation.

4. **Example**:
   ```c
	#include "crbhashmap.h"
	#include <stdio.h>

	void* Create_int()
	{
		return INT_TO_POINTER(0);
	}

	int main()
	{
		// Example usage
		int test_count = 100;

		c_rb_hash_map cmap = {};
		c_rb_hash_map_init(&cmap, -1, 0, &Create_int);

		for (int i = 0, j = test_count; i < j; i++)
		{
			char* key = (char*)malloc(8);
			sprintf(key, "%d", i);

			c_rb_hash_map_insert(&cmap, key, INT_TO_POINTER(i));
			free(key);
		}

		printf("====insert, size:%d\n", cmap.count);

		c_rb_hash_iter citer;
		c_rb_hash_iter_init(&citer, &cmap);
		char* __key = NULL;
		void* __value = NULL;

		while (c_rb_hash_iter_next(&citer, &__key, &__value))
		{
			int val = POINTER_TO_INT(__value);
			printf("%s, %d\n", __key, val);
		}

		printf("====iterate, size:%d\n", cmap.count);

		for (int i = 0, j = test_count; i < j; i++)
		{
			char* key = (char*)malloc(8);
			sprintf(key, "%d", i + 2);

			c_rb_hash_node* pnode = c_rb_hash_map_insert_or_find(&cmap, key);
			printf("%s, %d\n", key, POINTER_TO_INT(pnode->value));

			free(key);
		}

		printf("====insert_or_find, size:%d\n", cmap.count);

		c_rb_hash_iter_init(&citer, &cmap);

		while (c_rb_hash_iter_next(&citer, &__key, &__value))
		{
			int val = POINTER_TO_INT(__value);
			printf("%s, %d\n", __key, val);
		}

		printf("====iterate, size:%d\n", cmap.count);

		char* key = (char*)malloc(8);

		for (int i = 0, j = test_count + 4; i < j; i++)
		{
			sprintf(key, "%d", i);

			c_rb_hash_node* pnode = c_rb_hash_map_find(&cmap, key);

			if (pnode != NULL)
			{
				int val = POINTER_TO_INT(pnode->value);
				printf("contains, %s=%d\n", key, val);
			}
			else
			{
				printf("====not found, %s\n", key);
			}

			c_rb_hash_map_remove(&cmap, key);
		}

		free(key);

		printf("====remove size:%d\n", cmap.count);

		c_rb_hash_map cmap2 = {};
		c_rb_hash_map_move(&cmap2, &cmap);
		printf("====move\n");

		c_rb_hash_map_insert(&cmap, "test", INT_TO_POINTER(111));

		c_rb_hash_iter_init(&citer, &cmap);

		while (c_rb_hash_iter_next(&citer, &__key, &__value))
		{
			int val = POINTER_TO_INT(__value);
			printf("cmap:%s, %d\n", __key, val);
		}

		c_rb_hash_map_destroy(&cmap);

		c_rb_hash_iter_init(&citer, &cmap2);

		while (c_rb_hash_iter_next(&citer, &__key, &__value))
		{
			int val = POINTER_TO_INT(__value);
			printf("cmap2:%s, %d\n", __key, val);
		}

		c_rb_hash_map_destroy(&cmap2);

		printf("====clear\n");

		return 0;
	}

## License
This project is licensed under the Apache License, Version 2.0. See the LICENSE file for details.
