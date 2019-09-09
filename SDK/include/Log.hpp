#pragma once

#include <cstdio>

const char * __GET_CURRENT_TIME_STRING(void);

#ifndef LOG
#define LOG(fmt, args...) do { fprintf(stderr, "[%s %s, %s, L%d]: " fmt "\n", __GET_CURRENT_TIME_STRING(), __FILE__, __FUNCTION__, __LINE__, ##args); fflush(stderr); } while (false);
#endif