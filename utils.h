
#pragma function(memset)
void* memset(void *destination, int value, size_t count)
{
    char *bytes = (char*)destination;
    while(count--)
    {
        
        *bytes++ = (char)value;
    }
    return destination;
}

#pragma function(memcpy)
inline void* memcpy(void* destination, void* src, size_t count)
{
    for (size_t i = 0; i < count; i++)
    {
        ((char*)destination)[i] = ((char*)src)[i];
    }
    return destination;
}

