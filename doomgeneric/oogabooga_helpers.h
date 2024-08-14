/*
 *   Compatibility layer
 */

#ifndef OOBABOOGA_HELPERS_H
#define OOBABOOGA_HELPERS_H

#define snprintf(buf, size, ...)                                        \
    do {                                                                \
        string s = sprintf(get_heap_allocator(), __VA_ARGS__);          \
        int count = s.count < ((size) - 1) ? s.count : ((size) - 1);    \
        memcpy((buf), s.data, count);                                   \
        (buf)[count] = '\0';                                            \
        dealloc(get_heap_allocator(), s.data);                          \
    } while(0)      

char * ogb_strdup(const char *s);

#endif // OOBABOOGA_HELPERS_H
