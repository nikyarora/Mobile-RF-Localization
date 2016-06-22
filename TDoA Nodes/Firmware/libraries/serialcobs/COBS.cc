#include "COBS.h"

namespace COBS
{


#define COBSFinishBlock(X) (*code_ptr = (X), code_ptr = dst++, code = 0x01)
    
    /*
     * encode byte stuffs "length" bytes of
     * data at the location pointed to by "ptr",
     * writing the output to the location pointed
     * to by "dst".
     */
    uint32_t encode(const unsigned char *ptr,
                    unsigned long length, unsigned char *dst)
    {
        const unsigned char *end = ptr + length;
        unsigned char *code_ptr = dst++;
        unsigned char code = 0x01;
        unsigned char *start = dst;
        
        while (ptr < end)
        {
            if (*ptr == 0)
                COBSFinishBlock(code);
            else
            {
                *dst++ = *ptr;
                code++;
                if (code == 0xFF)
                    COBSFinishBlock(code);
            }
            ptr++;
        }
        *dst++ = 0;
        COBSFinishBlock(code);
        return dst-start;
    }

    /*
     * decode decodes "length" bytes of
     * data at the location pointed to by "ptr",
     * writing the output to the location pointed
     * to by "dst". dst will be length-2 long.
     */
    uint32_t decode(const unsigned char *ptr,
                    unsigned long length, unsigned char *dst)
    {
        const unsigned char *end = ptr + length;
        unsigned char *start = dst;
        while (ptr < end)
        {
            int i, code = *ptr++;
            for (i=1; i<code && ptr<end; i++)
                *dst++ = *ptr++;
            if (code < 0xFF)
                *dst++ = 0;
        }
        return dst-start-1;
    }


}

