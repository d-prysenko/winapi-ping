#pragma once
#include <cstdint>
#include <cstdio>
#include <Windows.h>


void debug(const char* fmt, ...);
void error(const char* fmt, ...);
void stderr_colorized(const char* fmt, uint16_t text_attr, va_list args);

void print_bytes(const void* data, size_t size);
