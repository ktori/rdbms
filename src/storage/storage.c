/*
 * Created by victoria on 23.02.20.
*/

#include "storage.h"

#include "table/record.h"

#include <malloc.h>
#include <string.h>
#include <table/relation.h>

typedef struct store_entry_s
{
	unsigned id;
	record_value_t values;
	unsigned count;
} *store_entry_t;

typedef struct rel_store_s
{
	short rel;

	store_entry_t *array;
	unsigned count;
	unsigned size;
	unsigned last_id;
} *rel_store_t;

struct internal_store_s
{
	rel_store_t	*relations;
	unsigned count;
	unsigned size;
};

static struct internal_store_s store = {0};

int
store_init()
{
	store.count = 0;
	store.size = 8;
	store.relations = malloc(store.size * sizeof(rel_store_t));

	return 0;
}

static unsigned
store_get(short rel)
{
	rel_store_t *i = store.relations, *end = store.relations + store.count, c;

	for (; i < end; ++i)
	{
		if ((*i)->rel == rel)
			return i - store.relations;
	}

	if (store.count == store.size)
	{
		store.size *= 2;
		store.relations = realloc(store.relations, store.size * sizeof(rel_store_t));
	}

	c = calloc(1, sizeof(struct rel_store_s));
	c->rel = rel;
	c->count = 0;
	c->last_id = 0;
	c->size = 4;
	c->array = calloc(c->size, sizeof(store_entry_t));

	store.relations[store.count] = c;
	return store.count++;
}

int
store_insert(short rel, unsigned *out_id, record_t record)
{
	rel_store_t rel_store = store.relations[store_get(rel)];

	*out_id = ++rel_store->last_id;

	if (store_insert_in_place(rel, *out_id, record))
		return 1;

	return 0;
}

int
store_for_each(short rel, store_for_each_callback_t callback, void *user)
{
	unsigned i;
	rel_store_t rel_store = store.relations[store_get(rel)];
	relation_t relation = rel_get(rel);

	store_entry_t entry;
	struct record_s record = {0};
	record.def = relation->record_def;

	for (i = 0; i < rel_store->count; ++i)
	{
		entry = rel_store->array[i];
		record.values = entry->values;
		if (callback(entry->id, &record, user))
			break;
	}

	return 0;
}

int
store_insert_in_place(short rel, unsigned id, record_t record)
{
	unsigned rel_store_id = store_get(rel), i;
	rel_store_t rel_store = store.relations[rel_store_id];
	store_entry_t entry;
	relation_t relation = rel_get(rel);

	if (rel_store->count == rel_store->size)
	{
		rel_store->size *= 2;
		rel_store->array = realloc(rel_store->array, rel_store->size * sizeof(store_entry_t));
	}

	entry = calloc(1, sizeof(struct rel_store_s));
	entry->id = id;
	if (rel_store->last_id < id)
		rel_store->last_id = id;
	entry->values = malloc(relation->record_def->attributes_count * sizeof(struct record_value_s));
	entry->count = relation->record_def->attributes_count;
	for (i = 0; i < relation->record_def->attributes_count; ++i)
	{
		entry->values[i].data_size = record->values[i].data_size;
		entry->values[i].data = malloc(entry->values[i].data_size);
		memcpy(entry->values[i].data, record->values[i].data, record->values[i].data_size);
	}
	rel_store->array[rel_store->count] = entry;
	rel_store->count++;

	return 0;
}

int
store_find_by_id(short rel, unsigned id, struct record_s *out)
{
	unsigned i;
	rel_store_t rel_store = store.relations[store_get(rel)];

	for (i = 0; i < rel_store->count; ++i)
	{
		if (rel_store->array[i]->id == id)
		{
			out->values = rel_store->array[i]->values;
			out->def = rel_get(rel)->record_def;
			return 0;
		}
	}

	return 1;
}

void
store_shutdown()
{
	rel_store_t *i = store.relations, *end = i + store.count;
	store_entry_t *j, *e_end;
	record_value_t k, k_end;

	for (; i < end; ++i)
	{
		for ((j = (*i)->array), (e_end = (*i)->array + (*i)->count); j < e_end; ++j)
		{
			for ((k = (*j)->values), (k_end = k + (*j)->count); k < k_end; ++k)
			{
				free(k->data);
			}
			free((*j)->values);
			free(*j);
		}
		free((*i)->array);
		free(*i);
	}

	free(store.relations);
}
