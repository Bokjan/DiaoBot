#pragma once

#include <cstdio>

#ifndef LOG
#define LOG(fmt, args...) do { fprintf(stderr, "[%s, %s, L%d]: " fmt "\n", __FILE__, __FUNCTION__, __LINE__, ##args); fflush(stderr); } while (false);
#endif