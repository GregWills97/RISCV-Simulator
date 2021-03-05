#ifndef __PORTS_H__
#define __PORTS_H__

#include "packet.h"

class SlavePort {
    private:
        MasterPort* mport;
    public:
        SlavePort() {}
        void bind(MasterPort* _mport);
        void unbind();
        bool isBound() {return (mport != NULL);}
        MasterPort* getMaster() {return mport;}

        virtual void sendResp(PacketPtr pkt);
        virtual void recvReq(PacketPtr pkt) = 0;
        virtual const AddrRange getAddrRange() = 0;
};

class MasterPort {
    private:
        SlavePort* sport;
    public:
        MasterPort() {}
        void bind(SlavePort* _sport);
        void unbind();
        bool isBound() {return(sport != NULL);}
        SlavePort* getSlave() {return sport;}

        virtual void sendReq(PacketPtr pkt);
        virtual void recvResp(PacketPtr pkt) = 0;
        virtual const AddrRange getAddrRange() {return sport->getAddrRange();}
};

#endif //__PORTS_H__
