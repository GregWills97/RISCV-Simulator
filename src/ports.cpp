#include "ports.h"

void SlavePort::bind(MasterPort* _mport) {
    mport = _mport;
    if (mport->getSlave() != this) {
        mport->bind(this);
    }
}

void SlavePort::unbind() {
    mport = NULL;
}

void SlavePort::sendResp(PacketPtr pkt) {
    pkt->popHeader();
    mport->recvResp(pkt);
}

void MasterPort::bind(SlavePort* _sport) {
    sport = _sport;
    if (sport->getMaster() != this) {
        sport->bind(this);
    }
}

void MasterPort::unbind() {
    sport->unbind();
    sport = NULL;
}

void MasterPort::sendReq(PacketPtr pkt) {
    pkt->appendHeader(this);
    sport->recvReq(pkt);
}
