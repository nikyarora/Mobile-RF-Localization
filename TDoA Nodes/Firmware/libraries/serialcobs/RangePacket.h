#ifndef RANGEPACKET_H
#define RANGEPACKET_H

#include "Packet.h"
#include "Crc.h"

class RangePacket : public Packet
{
public:
    RangePacket( ) : Packet(PACKET_TYPE_RANGE, 0x0001, 0x0195), 
                     _anchorid(0), _distinmm(0), _power(0)
    {
        // Constructor
    }

    RangePacket(Packet &p) : Packet(p)
    {
        this->unPack();
    }

    void anchorid(uint16_t id) {_anchorid = id;}
    void distinmm(uint32_t d)  {_distinmm = d;}
    void power(float d)  {_power = floor(-d*10);}

protected:
    virtual void Pack(){
        _length = 8;
        uint8_t *p = _data;
        p += hton(_anchorid, p);
        p += hton(_distinmm, p);
        p += hton(_power, p);
    }

    virtual void unPack(){
        _anchorid = ntoh_u16(_data);
        _distinmm = ntoh_u32(_data+2);
        _power = ntoh_i16(_data+6);
    }

private:
    uint16_t _anchorid;
    uint32_t _distinmm;
    int16_t _power;
};

#endif // RANGEPACKET_H
