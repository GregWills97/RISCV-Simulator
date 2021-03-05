#include "membus.h"
#include <cassert>
#include <iostream>

bool AddressInRange(Addr ad, AddrRange ar) {
    return (ad >= ar.first) && (ad <= ar.second);
}

Membus::Membus(System *sys, Tick forward_time=0):
    SimObject(sys),
    fwd_time(forward_time),
    fwd_evnt(new MembusForwardEvent(this)) {
        memSidePorts.push_back(new MemSidePort(this));
        cpuSidePorts.push_back(new CPUSidePort(this));
}

void Membus::tryToSend() {
    auto it = packetsWaitingForMemPorts.begin();
    while (it != packetsWaitingForMemPorts.end()) {
        PacketPtr pkt = *it;
        MemSidePort* mem_port = getRequestPort(pkt);
        if (!(mem_port->isBusy())) {
            std::cout << "Membus sending packet on Tick: " << currTick() << std::endl;
            mem_port->sendReq(pkt);
            it = packetsWaitingForMemPorts.erase(it);
        } else {
            it++;
        }
    }
}

void Membus::forwardPackets() {
    Tick curr = currTick();
    while(!(packetsWaitingForForward.empty()) && (packetsWaitingForForward.front().first == curr)) {
        fwdQType tmp =  packetsWaitingForForward.front();
        packetsWaitingForForward.pop_front();
        packetsWaitingForMemPorts.push_back(tmp.second);
    }
    if (!(packetsWaitingForForward.empty())) {
        fwdQType tmp = packetsWaitingForForward.front();
        schedule(fwd_evnt, tmp.first);
    }
    tryToSend();
}

void Membus::recvReq(PacketPtr pkt) {
    Tick forwardTick = currTick() + fwd_time;
    packetsWaitingForForward.push_back(fwdQType(forwardTick, pkt));
    if (!fwd_evnt->isScheduled()) {
        schedule(fwd_evnt, forwardTick);
        tryToSend();
    }
}

void Membus::recvResp(PacketPtr pkt) {
    CPUSidePort* resp_port = getResponsePort(pkt);
    resp_port->sendResp(pkt);
    tryToSend();
}

Membus::MemSidePort* Membus::getRequestPort(PacketPtr pkt) {
    for (auto port : memSidePorts) {
        if (AddressInRange(pkt->getAddr(), port->getAddrRange())) {
            return port;
        }
    }
    std::cout << "Couldn't find the destination port for the packet";
    assert(0);
    return NULL;
}

Membus::CPUSidePort* Membus::getResponsePort(PacketPtr pkt) {
    for (auto port : cpuSidePorts) {
        if (port->getMaster() == pkt->getHeaderEnd()) {
            return port;
        }
    }
    std::cout << "Couldn't find the original port for the packet";
    assert(0);
    return NULL;
}


Membus::MemSidePort* Membus::getMemSidePort(size_t index) {
    if (index < memSidePorts.size()) {
        return memSidePorts.at(index);
    }
    std::cout << "Couldn't find valid port at index: " << index << std::endl;
    assert(0);
    return NULL;
}

Membus::CPUSidePort* Membus::getCPUSidePort(size_t index) {
    for (auto port : cpuSidePorts) {
        if (index < cpuSidePorts.size()) {
            return cpuSidePorts.at(index);
        }
    }
    std::cout << "Couldn't find valid port at index: " << index << std::endl;
    assert(0);
    return NULL;
}

Membus::MemSidePort* Membus::getUnboundMemSidePort() {
    if (memSidePorts.back()->isBound()) {
        memSidePorts.push_back(new MemSidePort(this));
    }
    return memSidePorts.back();
}

Membus::CPUSidePort* Membus::getUnboundCPUSidePort() {
    if (cpuSidePorts.back()->isBound()) {
        cpuSidePorts.push_back(new CPUSidePort(this));
    }
    return cpuSidePorts.back();
}
