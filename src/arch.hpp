#pragma once
#include "stdint.h"
#include "stddef.h"
size_t unpackFrom(char* from, size_t size, char* to, int* retok, size_t* memused);