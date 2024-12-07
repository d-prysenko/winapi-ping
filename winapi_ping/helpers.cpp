#include "helpers.h"


void debug(const char* fmt, ...)
{
#ifdef _DEBUG
	va_list args;
	va_start(args, fmt);

	stderr_colorized(fmt, 3, args);
#endif
}

void error(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	stderr_colorized(fmt, 12, args);
}

void stderr_colorized(const char* fmt, uint16_t text_attr, va_list args)
{
	HANDLE err_handle = GetStdHandle(STD_ERROR_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	uint16_t current_console_attr = 7;

	if (GetConsoleScreenBufferInfo(err_handle, &csbi))
		current_console_attr = csbi.wAttributes;

	SetConsoleTextAttribute(err_handle, text_attr);

	vfprintf(stderr, fmt, args);

	SetConsoleTextAttribute(err_handle, current_console_attr);
}

void print_bytes(const void* data, size_t size)
{
#ifdef _DEBUG
	uint8_t* start = (uint8_t*)data;

	for (int i = 0; i < size; i++)
	{
		debug("%X ", *start);
		start++;
	}

	debug("\n");
#else
	return;
#endif
}

