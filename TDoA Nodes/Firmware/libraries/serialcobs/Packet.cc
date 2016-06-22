#include <string.h>

#include "Packet.h"
#include "Crc.h"
#include "COBS.h"

Packet::Packet(uint8_t type, uint16_t address, uint16_t from)
    : _type(type), _address(address), _from(from), _length(0), _crc16(0),
      _encoded_length(0)
{
    // Constructor
}

Packet::~Packet()
{
    // Destructor
}

uint8_t* Packet::getEncodedPacket(unsigned *length)
{
    // Tell child class to populate fields
    this->Pack();
    
    uint8_t tmpdata[32];
    memset(tmpdata, 0, sizeof(tmpdata));
    memset(_encoded_data, 0, sizeof(_encoded_data));

    // Pack data
    uint8_t* dst = tmpdata;
    *dst++ = _type;
    dst += hton(_address, dst);
    dst += hton(_from, dst);
    dst += hton(_length, dst);
    memcpy(dst, _data, _length);
    dst+=_length;

    // Calculate CRC
    _crc16 = Crc::crc_ccitt_bw(tmpdata, dst - tmpdata);
    
    dst += hton(_crc16, dst);

    // COBS
    size_t cobs_len = COBS::encode(tmpdata, dst - tmpdata, _encoded_data);
    _encoded_length = cobs_len;

    *length = _encoded_length;
    return _encoded_data;
}


bool Packet::readFrom(uint8_t* data, unsigned length)
{
    // Sanity check
    if (length < (sizeof(_type) + sizeof(_address) + sizeof(_length) + sizeof(_crc16)))
        return false;

    // Allocate memory
    _encoded_length = length;
    unsigned cobs_overhead = (_encoded_length+2*254)/255;

    // De-COBS, len includes the terminating zero
    size_t decoded_len = COBS::decode(data, length, _encoded_data);

    // Check CRC
    uint8_t* src = _encoded_data + decoded_len - 3; 
    _crc16 = ntoh_u16(src);src += sizeof(_crc16);

    uint16_t packetcrc = Crc::crc_ccitt_bw(_encoded_data, decoded_len-1);
    if (packetcrc != 0)
    {
        return false;
    }

    // unpack
    src = _encoded_data;
    _type = *src++;
    
    _address = ntoh_u16(src);src += sizeof(_address);
    _from = ntoh_u16(src);src += sizeof(_from);
    _length = ntoh_u16(src);src += sizeof(_length);
    
    memcpy(_data, src, _length);

    // Let the child unpack itself
    this->unPack();

    return true;
}

void Packet::log(unsigned level)
{
    // Serial.println("Packet to 0x%.4x, length %d, crc 0x%x and data:\n", 
    //            _address, _length, _crc16);

    // unsigned n=0;
    // for (unsigned j=0;j<(_length/16+(unsigned)1);j++)
    // {
    //     Log::print(level, "  [%.3d] ", n);
    //     for (int k=0;k<16 && n<_length;k++) 
    //         Log::print(level, "0x%.2x ", _data[n++]);
    //     Log::print(level, "\n");
    // }

    // if (!_encoded_data || _encoded_length==0) return;
    // Log::print(level, "  encoded data with length %d:\n", _encoded_length);

    // for (unsigned j=0;j<_encoded_length/16+1;j++)
    // {
    //     Log::print(level, "  [%.3d] ", n);
    //     for (int k=0;k<16 && n<_encoded_length;k++) 
    //         Log::print(level, "0x%.2x ", _encoded_data[n++]);
    //     Log::print(level, "\n");
    // }
}
