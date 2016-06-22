#include <stdlib.h>
#include "Crc.h"

namespace Crc
{
    /**
     * Return CRC-8 of the data, using x^8 + x^2 + x + 1 polynomial.  A table-based
     * algorithm would be faster, but for only a few bytes it isn't worth the code
     * size. */
    uint8_t Crc8(const uint8_t *vptr, int len)
    {
        const uint8_t *data = vptr;
        unsigned crc = 0;

        /* Sanity check: */
        if(vptr == NULL)
            return 0;

        int i, j;
        for (j = len; j; j--, data++) 
        {
            crc ^= (*data << 8);
            for(i = 8; i; i--) 
            {
                if (crc & 0x8000)
                    crc ^= (0x1070 << 3);
                crc <<= 1;
            }
        }
        return (uint8_t)(crc >> 8);
    }


    uint16_t Crc16(const uint8_t *data, uint16_t size)
    {
        uint16_t out = 0;
        int bits_read = 0, bit_flag;

        /* Sanity check: */
        if(data == NULL)
            return 0;

        while(size > 0)
        {
            bit_flag = out >> 15;


            /* Get next bit: */
            out <<= 1;
            out |= (*data >> (7 - bits_read)) & 1;


            /* Increment bit counter: */
            bits_read++;
            if(bits_read > 7)
            {
                bits_read = 0;
                data++;
                size--;
            }


            /* Cycle check: */
            if(bit_flag)
                out ^= 0x8005;
        }

        return out;
    }


    uint16_t Crc16bl(const uint8_t *data, uint16_t size)
    {
        uint16_t remainder = 0x0;
        unsigned long address = 0x00;
        
        for (; address < size; ++address) {
            remainder ^= (data[address] << 8);
            for (uint8_t bit = 8; bit > 0; --bit) {
                if (remainder & 0x8000) {
                    remainder = (remainder << 1) ^ 0x8005;
                } else {
                    remainder = (remainder << 1);
                }
            }
        }
        
        return remainder;
    }


    /**
     *      crc_ccitt - recompute the CRC for the data buffer
     *      @crc: previous CRC value
     *      @buffer: data pointer
     *      @len: number of bytes in the buffer
     */
    uint16_t crc_ccitt(uint16_t crc, uint8_t const *buffer, size_t len)
    {
        while (len--)
            crc = crc_ccitt_byte(crc, *buffer++);
        return crc;
    }
    
#define hi8(x) ((x>>8)&0xff)
#define lo8(x) (x&0xff)
    uint16_t crc_ccitt_bw(const uint8_t *data, uint16_t size)
    {
        /* Sanity check: */
        if(data == NULL)
            return 0;

        uint16_t crc = 0x0000;
        for (unsigned i=0;i<size;i++)
        {
            uint8_t d = data[i];

            d ^= lo8 (crc);
            d ^= d << 4;

            crc = ((((uint16_t)d << 8) | hi8 (crc)) ^ (uint8_t)(d >> 4) ^ ((uint16_t)d << 3));
        }

        return crc;
    }
}
