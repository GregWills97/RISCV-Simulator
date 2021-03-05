#ifndef __MEMBUS_H__
#define __MEMBUS_H__

#include "simobject.h"
#include "ports.h"
#include <vector>
#include <deque>

class Membus : public SimObject {
    private:
        class MembusForwardEvent : public Event{
            private:
                Membus* owner;
            public:
                MembusForwardEvent(Membus* _owner) : Event(), owner(_owner) {}
                void process() override {owner->forwardPackets();}
                const char* description() override {return "Membus Forward Event";}
        };

        class MemSidePort : public MasterPort {
            private:
                Membus* owner;
                PacketPtr activeRequest;
            public:
                MemSidePort(Membus* _owner) : MasterPort(), owner(_owner) {}
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
        class CPUSidePort : public SlavePort {
            private:
                Membus* owner;
            public:
                CPUSidePort(Membus* _owner) : SlavePort(), owner(_owner) {}
                virtual void recvReq(PacketPtr pkt) override {owner->recvReq(pkt);}
                const AddrRange getAddrRange() override {return AddrRange(0,0);}
        };

        typedef std::pair<Tick, PacketPtr> fwdQType;
        
        Tick fwd_time;
        MembusForwardEvent* fwd_evnt;
        std::vector<MemSidePort*> memSidePorts;
        std::vector<CPUSidePort*> cpuSidePorts;
        std::deque<PacketPtr> packetsWaitingForMemPorts;
        std::deque<fwdQType> packetsWaitingForForward;

    public:
        Membus(System *sys, Tick forward_time);
        virtual void initialize() override {}
        void tryToSend();
        void forwardPackets();
        void recvReq(PacketPtr pkt);
        void recvResp(PacketPtr pkt);
        MemSidePort* getRequestPort(PacketPtr pkt);
        CPUSidePort* getResponsePort(PacketPtr pkt);

        MemSidePort* getMemSidePort(size_t index);
        CPUSidePort* getCPUSidePort(size_t index);
        MemSidePort* getUnboundMemSidePort();
        CPUSidePort* getUnboundCPUSidePort();
};


#endif // __MEMBUS_H__
