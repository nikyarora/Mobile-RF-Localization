#ifndef ACCELEROMETERPACKET_H
#define ACCELEROMETERPACKET_H

#include "Packet.h"
#include "Crc.h"

class AccelerometerPacket : public Packet
{
public:
    AccelerometerPacket(int16_t x=0.0, int16_t y=0.0, int16_t z=0.0, uint16_t from=0xbed1) : Packet(PACKET_TYPE_ACCELEROMETER, 0x0001, from), 
        _x(x), _y(y), _z(z)
    {
        // Constructor
    }

    AccelerometerPacket(Packet &p) : Packet(p)
    {
        this->unPack();
    }

    void x(int16_t x) {_x = x;}
    void y(int16_t y) {_y = y;}
    void z(int16_t z) {_z = z;}

    int16_t x() {return _x;}
    int16_t y() {return _y;}
    int16_t z() {return _z;}

protected:
    virtual void Pack(){
        _length = 6;
        uint8_t *p = _data;
        p += hton(_x, p);
        p += hton(_y, p);
        p += hton(_z, p);
    }

    virtual void unPack(){
        _x = ntoh_i16(_data);
        _y = ntoh_i16(_data+2);
        _z = ntoh_i16(_data+4);
    }

private:
    int16_t _x;
    int16_t _y;
    int16_t _z;
};

#endif // ACCELEROMETERPACKET_H
