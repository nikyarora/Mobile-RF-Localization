#ifndef COBS_H
#define COBS_H

#include <stdlib.h>
#include <stdint.h>

namespace COBS
{

/*
 * encode byte stuffs "length" bytes of
 * data at the location pointed to by "ptr",
 * writing the output to the location pointed
 * to by "dst".
 */
    uint32_t encode(const unsigned char *ptr, unsigned long length, unsigned char *dst);
 
/*
 * decode decodes "length" bytes of
 * data at the location pointed to by "ptr",
 * writing the output to the location pointed
 * to by "dst". dst will be length-2 long.
 */
    uint32_t decode(const unsigned char *ptr, unsigned long length, unsigned char *dst);
}
#endif
