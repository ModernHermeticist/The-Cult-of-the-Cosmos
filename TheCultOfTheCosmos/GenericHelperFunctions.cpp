#include "pch.h"
#include "main.h"


int string_length(const char s[])
{
	int c = 0;
	while (s[c] != '\0')
		c++;
	return c;
}
