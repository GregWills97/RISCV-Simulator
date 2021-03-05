#ifndef __PACKET_H__
#define __PACKET_H__

#include <deque>
#include <utility>
#include <cinttypes>

typedef size_t Addr;
typedef std::pair<Addr, Addr> AddrRange;

class MasterPort;

class Packet {
    private:
        bool _isRead;
        uint8_t* buffer;
        std::deque<MasterPort *> header;
        Addr dst;
        size_t size;
    public:
        Packet(bool read, Addr _dst, size_t _size) :
            _isRead(read),
            buffer(new uint8_t[_size]),
            dst(_dst),
            size(_size) {}
        ~Packet() {
            header.clear();
            if (buffer) delete[] buffer;
        }
        uint8_t* getBuffer() {return buffer;}
        bool isRead() {return _isRead;}
        size_t getSize() {return size;}
        Addr getAddr() {return dst;}

        void appendHeader(MasterPort* port) {header.push_back(port);}
        void popHeader() {header.pop_back();}
        MasterPort* getHeaderEnd() {return header.back();}
};

typedef Packet* PacketPtr;

#endif // __PACKET_H__
