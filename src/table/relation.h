/*
 * Created by victoria on 23.02.20.
*/

#pragma once

#include "attribute.h"

struct relation_s
{
	struct attribute_s *attributes;
	unsigned count;
	unsigned size;
};
