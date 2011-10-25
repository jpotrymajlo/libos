
#include "types.h" /* size_t */


void *memcpy(void *dst_ptr, const void *src_ptr, UINT32 count)
{
	void *ret_val = dst_ptr;
	const char *src = (const char *)src_ptr;
	char *dst = (char *)dst_ptr;

	for(; count != 0; count--)
		*dst++ = *src++;
	return ret_val;
}

int memcmp(UINT8* s1,UINT8* s2,int n)
{
    while (*s1++ == *s2++ && n--);
    return n;

}
void *memset(void *dst, UBYTE val, UINT32 count)
{
	unsigned char *temp = (unsigned char *)dst;

	for( ; count != 0; count--)
		*temp++ = val;
	return dst;
}

void *memsetw(void *dst, UWORD val, UINT32 count)
{
	unsigned short *temp = (unsigned short *)dst;

	for( ; count != 0; count--)
		*temp++ = val;
	return dst;
}


