#ifndef __DRAM_H__
#define __DRAM_H__

#include "simobject.h"
#include "ports.h"

class DRAM : public SimObject {
    private:
        AddrRange addrs;
        size_t size;
        Tick responseLatency;
        uint8_t* memory;
        
        class MemPort : public SlavePort {
            private:
                DRAM* owner;
            public:
                MemPort(DRAM* _owner) : SlavePort(), owner(_owner) {}
                virtual void recvReq(PacketPtr pkt) override {owner->recvReq(pkt);}
                const AddrRange getAddrRange() override {return owner->getAddrRange();}
        };

        MemPort* port;

        class DRAMEvent : public Event {
            private:
                DRAM* owner;
            public:
                DRAMEvent(DRAM* _owner) : Event(), owner(_owner) {}
                void process() override {owner->process();}
                const char* description() override {return "DRAM Response Event";}
        };

        DRAMEvent* e;
        PacketPtr request;

    public:
        DRAM(System *sys, AddrRange _addrs, Tick respLatency);

        virtual void initialize() override {}
        void setDataAtAddr(Addr, uint8_t*, size_t);

        template<typename T>
        void writeAtAddr(Addr, T);

        void writeByteAtAddr(Addr, uint8_t);
        void writeHalfwordAtAddr(Addr, uint16_t);
        void writeWordAtAddr(Addr, uint32_t);
        void writeDoubleWordAtAddr(Addr, uint64_t);

        void getDataAtAddr(Addr, uint8_t*, size_t);

        template<typename T>
        T readAtAddr(Addr);
        uint8_t readByteAtAddr(Addr);
        uint16_t readHalfwordAtAddr(Addr);
        uint32_t readWordAtAddr(Addr);
        uint64_t readDoubleWordAtAddr(Addr);

        void process();
        void recvReq(PacketPtr pkt);

        SlavePort* getPort() {return port;}
        const AddrRange getAddrRange() {return addrs;}
};

#endif // __DRAM_H__
