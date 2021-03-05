#include "DRAM.h"
#include <cassert>
#include <cstring>
#include <iostream>
#include <cstdlib>

DRAM::DRAM(System *sys, AddrRange _addrs, Tick respLatency) :
    SimObject(sys),
    addrs(_addrs),
    responseLatency(respLatency),
    port(new MemPort(this)),
    e(new DRAMEvent(this)) {
        size = addrs.second - addrs.first + 1;
        memory = (uint8_t*)std::malloc(size);
    }

void DRAM::process() {
    std::cout << "DRAM is responding to a request on Tick: " << currTick() << std::endl;
    port->sendResp(request);
}

void DRAM::recvReq(PacketPtr pkt) {
    std::cout << "DRAM received a request on Tick: " << currTick() << std::endl;
    if (pkt->isRead()) {
        getDataAtAddr(pkt->getAddr(), pkt->getBuffer(), pkt->getSize());
    } else {
        setDataAtAddr(pkt->getAddr(), pkt->getBuffer(), pkt->getSize());
    }
    request = pkt;
    schedule(e, currTick()+responseLatency);
}

void DRAM::setDataAtAddr(Addr ad, uint8_t* buff, size_t len) {
    assert((ad >= addrs.first) && ((ad+len) <= addrs.second+1));
    Addr offset = ad - addrs.first;
    std::memcpy((memory+offset), buff, len);
}

template<typename T>
void DRAM::writeAtAddr(Addr ad, T val) {
    setDataAtAddr(ad, (uint8_t *)(&val), sizeof(T));
}

void DRAM::writeByteAtAddr(Addr ad, uint8_t val) {
    writeAtAddr<uint8_t>(ad, val);
}

void DRAM::writeHalfwordAtAddr(Addr ad, uint16_t val) {
    writeAtAddr<uint16_t>(ad, val);
}

void DRAM::writeWordAtAddr(Addr ad, uint32_t val) {
    writeAtAddr<uint32_t>(ad, val);
}

void DRAM::writeDoubleWordAtAddr(Addr ad, uint64_t val) {
    writeAtAddr<uint64_t>(ad, val);
}

void DRAM::getDataAtAddr(Addr ad, uint8_t* buff, size_t len) {
    assert((ad >= addrs.first) && ((ad+len) <= addrs.second+1));
    Addr offset = ad - addrs.first;
    std::memcpy(buff, (memory+offset), len);
}

template<typename T>
T DRAM::readAtAddr(Addr ad) {
    T val;
    getDataAtAddr(ad, (uint8_t*)(&val), sizeof(T));
    return val;
}

uint8_t DRAM::readByteAtAddr(Addr ad) {
    return readAtAddr<uint8_t>(ad);
}

uint16_t DRAM::readHalfwordAtAddr(Addr ad) {
    return readAtAddr<uint16_t>(ad);
}

uint32_t DRAM::readWordAtAddr(Addr ad) {
    return readAtAddr<uint32_t>(ad);
}

uint64_t DRAM::readDoubleWordAtAddr(Addr ad) {
    return readAtAddr<uint64_t>(ad);
}
