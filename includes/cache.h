#ifndef __CACHE_H__
#define __CACHE_H__

#include "simobject.h"
#include "ports.h"
#include <utility>
#include <vector>

enum associativity_type {none, two_way, four_way, full};

class Cache : public SimObject {
    private: 
        AddrRange addrs;
        size_t size;
        size_t line_size;
        Tick responseLatency;
        uint8_t* memory;    //actual cache data

        //struct to keep all of line data
        struct lineData {
            std::vector<uint32_t> tag;
            std::vector<bool> valid_bits;
            std::vector<bool> dirty_bits;
            std::vector<bool> mru_bits; //tracks lru (Bit-PLRU)
        };
        std::vector<lineData*> maps;
        
        associativity_type assoc;

        class CPUPort : public SlavePort {
            private:
                Cache* owner;
            public:
                CPUPort(Cache* _owner) : SlavePort(), owner(_owner) {}
                virtual void recvReq(PacketPtr pkt) override {owner->recvReq(pkt);}
                const AddrRange getAddrRange() override {return owner->getAddrRange();}
        };
        class MemPort : public MasterPort {
            private:
                Cache* owner;
                PacketPtr activeRequest;
            public:
                MemPort(Cache* _owner) : MasterPort(), owner(_owner) {}
                bool isBusy() {return (activeRequest != NULL);}
                void recvResp(PacketPtr pkt) override {
                    activeRequest = NULL;
                    owner->recvResp(pkt);
                }
                void sendReq(PacketPtr pkt) override {
                    activeRequest = pkt;
                    MasterPort::sendReq(pkt);
                }
        };
        class CacheEvent : public Event {
            private:
                Cache* owner;
            public:
                CacheEvent(Cache* _owner) : Event(), owner(_owner) {}
                void process() override {owner->process();}
                const char* description() override {return "Cache Response Event";}
        };

        MemPort* mem_port;
        CPUPort* cpu_port;
        CacheEvent* e;
        PacketPtr request;

        //for testing puruposes
        int hit_num, miss_num;

    public:
        Cache(System *sys, AddrRange _addrs, size_t _size, size_t _lines, associativity_type a, Tick respLatency);

        virtual void initialize() override {}
        void setDataAtAddr(Addr, uint8_t*, size_t);

        bool isHit(Addr ad);
        bool lineDirty(Addr ad);

        /*template<typename T>
        void writeAtAddr(Addr, T);

        void writeByteAtAddr(Addr, uint8_t);
        void writeHalfwordAtAddr(Addr, uint16_t);
        void writeWordAtAddr(Addr, uint32_t);
        void writeDoubleWordAtAddr(Addr, uint64_t);*/

        void getDataAtAddr(Addr, uint8_t*, size_t);

        /*template<typename T>
        T readAtAddr(Addr);
        uint8_t  readByteAtAddr(Addr);
        uint16_t readHalfwordAtAddr(Addr);
        uint32_t readWordAtAddr(Addr);
        uint64_t readDoubleWorkdAtAddr(Addr);*/

        void process();
        void recvReq(PacketPtr pkt);
        void recvResp(PacketPtr pkt);

        SlavePort* getCPUPort() {return cpu_port;}
        MasterPort* getMemPort() {return mem_port;}
        const AddrRange getAddrRange() {return addrs;}

        //for testing
        int getNumHits() {return hit_num;}
        int getNumMisses() {return miss_num;}
};

#endif //__CACHE_H__
