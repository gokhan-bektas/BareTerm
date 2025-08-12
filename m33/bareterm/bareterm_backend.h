// bareterm_backend.h
#ifndef bareterm_BACKEND_H
#define bareterm_BACKEND_H

#include <stddef.h>


// You define this function in the platform layer
void bareterm_backend_write(const char *buf, size_t len);

// Convenience macro for string literals
#define bareterm_backend_puts(str) bareterm_backend_write(str, sizeof(str) - 1)

#endif // bareterm_BACKEND_H