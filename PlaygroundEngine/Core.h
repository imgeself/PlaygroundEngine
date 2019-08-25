#pragma once

#include <stdint.h>
#include <stdio.h>
#include <assert.h>

#define PG_ASSERT(x, string) { if(!(x)) { printf("Assertion Failed: %s\n", string); __debugbreak(); } }