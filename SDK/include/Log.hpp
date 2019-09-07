#pragma once

#include <cstdio>

#ifndef LOG
#define LOG(fmt, args...) fprintf(stderr, "[%s, %s, L%d]: " fmt "\n", __FILE__, __FUNCTION__, __LINE__, ##args)
#endif