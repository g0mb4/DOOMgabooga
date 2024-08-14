#include "oogabooga_helpers.h"

char * ogb_strdup(const char *s){
    assert(s, "String is NULL.");
    const u32 len = strlen(s) + 1;  // + '\0'

    char *s_dup = alloc(get_heap_allocator(), len);
    memcpy(s_dup, s, len - 1);
    s_dup[len - 1] = '\0';
    
    return s_dup;
}