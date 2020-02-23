/*
 * Created by victoria on 23.02.20.
*/

#pragma once

struct record_s;

int
store_init();

int
store_insert(short rel, unsigned *out_id, struct record_s *record);

int
store_insert_in_place(short rel, unsigned id, struct record_s *record);

typedef int(*store_for_each_callback_t)(unsigned id, struct record_s *record, void *user);

int
store_for_each(short rel, store_for_each_callback_t callback, void *user);

int
store_find_by_id(short rel, unsigned id, struct record_s *out);
