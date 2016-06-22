#ifndef PACKET_H
#define PACKET_H

#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>
#include <string.h>

#define PACKET_TYPE_NONE     0x00
#define PACKET_TYPE_SETNETID 0x01
#define PACKET_TYPE_IDQUERY  0x02
#define PACKET_TYPE_IDREPLY  0x03
#define PACKET_TYPE_COMMAND_STATUS  0x04
#define PACKET_TYPE_DISPLAY_HEX 0x08
#define PACKET_TYPE_SETMEM   0x10
#define PACKET_TYPE_CRCMEM   0x11
#define PACKET_TYPE_CRCMEM_READ 0x12
#define PACKET_TYPE_TESTFLASH 0x20
#define PACKET_TYPE_RESET    0x21
#define PACKET_TYPE_ERASEMEM 0x30
#define PACKET_TYPE_CLOCKMASTER 0x40
#define PACKET_TYPE_PLAYBACK_ADDRESS 0x50
#define PACKET_TYPE_PLAYBACK_STOPAT  0x51
#define PACKET_TYPE_PLAYBACK_STORE 0x52
#define PACKET_TYPE_PLAYBACK_CLEAR 0x53

// Motorcontroller
#define PACKET_TYPE_RUNMOTOR 0x60
#define PACKET_TYPE_MOTOR_POSITIONS 0x61
#define PACKET_TYPE_MOTOR_POSITIONS_REPONSE 0x62
#define PACKET_TYPE_MOTOR_POWER 0x63

// Lidar
#define PACKET_TYPE_MEASUREMENT_START 0x70
#define PACKET_TYPE_MEASUREMENT_STOP 0x71
#define PACKET_TYPE_MEASUREMENT 0x72
#define PACKET_TYPE_MEASUREMENT_ERROR 0x73

#define PACKET_TYPE_MIRROR_ANGLE  0x80
// LPS
#define PACKET_TYPE_RANGE         0x90
#define PACKET_TYPE_ACCELEROMETER 0x91

// Bedlevel controller
#define PACKET_TYPE_BEDLEVELCTRL  0xb1
#define PACKET_TYPE_BEDLEVELSTATUS 0xb2

#define PACKET_MASTER_ADDRESS 0x0001
#define PACKET_BROADCAST_ADDRESS 0xffff


#define PACKET_BYTEORDER_LSB

class Packet
{
public:
    Packet(uint8_t type=PACKET_TYPE_NONE, uint16_t address=0, uint16_t from = 0x1234);
    virtual ~Packet();

    Packet(const Packet &s) : _type(s._type), _address(s._address), _from(s._from), _length(s._length), 
                              _crc16(s._crc16), _encoded_length(s._encoded_length)
    {
        if (s._data)
        {
            memcpy(_data, s._data, _length);
        }
        if (s._encoded_data)
        {
            memcpy(_encoded_data, s._encoded_data, _encoded_length);
        }
    }

    bool readFrom(uint8_t* data, unsigned length);
    uint8_t* getEncodedPacket(unsigned *length);

    static const char* typeToTxt(int type) 
    {
        switch(type)
        {
        case PACKET_TYPE_NONE: return "none";break;
        case PACKET_TYPE_SETNETID: return "SetNetID";break;
        case PACKET_TYPE_IDQUERY: return "IDQuery";break;
        case PACKET_TYPE_IDREPLY: return "IDQueryReply";break;
        case PACKET_TYPE_COMMAND_STATUS: return "CommandStatus";break;
        case PACKET_TYPE_SETMEM: return "SetMem";break;
        case PACKET_TYPE_TESTFLASH: return "TestFlash";break;
        case PACKET_TYPE_PLAYBACK_ADDRESS: return "PlaybackAddress";break;
        case PACKET_TYPE_PLAYBACK_STOPAT: return "PlaybackStopAt";break;
        default: return "unknown";break;
        }
    }

    uint8_t type() const {return _type;} 
    uint16_t address() const {return _address;}
    uint16_t setFrom(uint16_t f) {_from = f;}

    virtual void log(unsigned level);

    uint32_t hton(uint32_t d, uint8_t *p)
    {
#ifdef PACKET_BYTEORDER_LSB
        *(p++) =  d      & 0x000000ff;
        *(p++) = (d>>8)  & 0x000000ff;
        *(p++) = (d>>16) & 0x000000ff;
        *(p++) = (d>>24) & 0x000000ff;
#else
        *(p++) = (d>>24) & 0x000000ff;
        *(p++) = (d>>16) & 0x000000ff;
        *(p++) = (d>>8)  & 0x000000ff;
        *(p++) =  d      & 0x000000ff;
#endif
        return sizeof(d);
    }

    uint32_t hton(uint16_t d, uint8_t *ip)
    {
        uint8_t *p=ip; 
#ifdef PACKET_BYTEORDER_LSB
        *(p++) =  d      & 0x000000ff;
        *(p++) = (d>>8)  & 0x000000ff;
#else
        *(p++) = (d>>8)  & 0x000000ff;
        *(p++) =  d      & 0x000000ff;
#endif
        return sizeof(d);
    }

    uint32_t hton(int16_t d, uint8_t *ip)
    {
        uint8_t *p=ip; 
#ifdef PACKET_BYTEORDER_LSB
        *(p++) =  d      & 0x00ff;
        *(p++) = (d>>8)  & 0x00ff;
#else
        *(p++) = (d>>8)  & 0x00ff;
        *(p++) =  d      & 0x00ff;
#endif
        return sizeof(d);
    }

    inline uint32_t hton(uint8_t d, uint8_t *p)
    {
        *p =  d;
        return 1;
    }

    uint32_t ntoh_u32(uint8_t *p)
    {
        uint32_t r = 0;
#ifdef PACKET_BYTEORDER_LSB
        r  =  *(p++) & 0x000000ff;
        r |= ((uint32_t)*(p++) & 0x000000ff) << 8;
        r |= ((uint32_t)*(p++) & 0x000000ff) << 16;
        r |= ((uint32_t)*(p++) & 0x000000ff) << 24;
#else
        r  =  *(p++) & 0x000000ff;
        r |= ((uint32_t)*(p++) & 0x000000ff) << 8;
        r |= ((uint32_t)*(p++) & 0x000000ff) << 16;
        r |= ((uint32_t)*(p++) & 0x000000ff) << 24;
#endif
        return r;
    }

    uint16_t ntoh_u16(uint8_t *p)
    {
        uint16_t r = 0;
#ifdef PACKET_BYTEORDER_LSB
        r  =  *(p++) & 0x000000ff;
        r |= (*(p++) & 0x000000ff) << 8;
#else
        r  =  *(p++) & 0x000000ff;
        r |= (*(p++) & 0x000000ff) << 8;
#endif
        return r;
    }

    int16_t ntoh_i16(uint8_t *p)
    {
        int16_t r = 0;
#ifdef PACKET_BYTEORDER_LSB
        r  =  *(p++) & 0x000000ff;
        r |= (*(p++) & 0x000000ff) << 8;
#else
        r  =  *(p++) & 0x000000ff;
        r |= (*(p++) & 0x000000ff) << 8;
#endif
        return r;
    }

    uint8_t ntoh_u8(uint8_t *p)
    {
        return *p;
    }


protected:
    // Child classes needs to implement these
    virtual void Pack() {assert(0);}; 
    virtual void unPack() {};

    uint8_t  _type;
    uint16_t _address;
    uint16_t _from;
    uint16_t _length;
    uint8_t  _data[32];
    uint16_t _crc16;
private:
    uint8_t  _encoded_data[32];
    unsigned _encoded_length;
};

#endif // PACKET_H
