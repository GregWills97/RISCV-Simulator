#include "simobject.h"
#include "ports.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>

class memrequester : public SimObject {
    private:
        class RequestEvent : public Event {
            private:
                memrequester* owner;
            public:
                RequestEvent(memrequester* _owner) : Event(), owner(_owner) {}
                void process() override {owner->process();}
                const char* description() override {return "MemRequester Clock Event";}
        };

        class RequestPort : public MasterPort{
            private:
                memrequester* owner;
                PacketPtr activeRequest;
            public:
                RequestPort(memrequester* _owner) : MasterPort(), owner(_owner) {}
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

        RequestEvent* e;
        RequestPort* port;
        Tick clk_tick;
        size_t currAddr;
        size_t endAddr;
        FILE* fp;
        int pc;

    public:
        memrequester(System *sys, const char* name, size_t start, size_t end) :
            SimObject(sys, name),
            e(new RequestEvent(this)),
            port(new RequestPort(this)),
            clk_tick(10),
            currAddr(start),
            endAddr(end) {}
        ~memrequester();
        virtual void initialize() override {
            schedule(e, currTick());
            fp = fopen("/home/greg/Documents/C++/Comp_arch/Homework_5/addresses.txt", "r");
            if (fp == NULL){
                std::cout << "Couldn't open file" << std::endl;
            }
        }

        void process() {
            char* line = NULL;
            size_t len = 0;
            ssize_t read;
            /*if (!(port->isBusy())) {
                read = getline(&line, &len, fp);
                currAddr = std::strtol(line, NULL, 16);
                if (pc % 5 == 0) {
                    std::cout << "Creating memory write request to Addr: " << currAddr << " for 4 bytes on Tick: " << currTick() << std::endl;
                    PacketPtr write_packet = new Packet(false, currAddr, 4);
                    uint32_t num = 25;
                    if (currAddr % 4 == 0) {
                        std::memcpy(write_packet->getBuffer(), (uint8_t *)(&num), sizeof(num));
                        port->sendReq(write_packet);
                    }
                } else {
                    if (currAddr % 4 == 0) {
                        std::cout << "Creating memory read request to Addr: " << currAddr << " for 4 bytes on Tick: " << currTick() << std::endl;
                        port->sendReq(new Packet(true, currAddr, 4));
                    }
                }
            }
            if (read > -1) {
                schedule(e, currTick()+clk_tick);
            }*/

            if (!(port->isBusy())) {
                if (pc == 0) {
                    PacketPtr write_packet = new Packet(false, 0, 4);
                    uint32_t num = 25;
                    std::memcpy(write_packet->getBuffer(), (uint8_t *)(&num), sizeof(num));
                    std::cout << "Creating memory write request to Addr: 0 for 4 bytes on Tick: " << currTick() << std::endl;
                    port->sendReq(write_packet);
                } else if (pc == 1) {
                    std::cout << "Creating memory read request to Addr: 0 for 4 bytes on Tick: " << currTick() << std::endl;
                    port->sendReq(new Packet(true, 0, 4));
                } else if (pc == 2) {
                    std::cout << "Creating memory read request to Addr: 3588 for 4 bytes on Tick: " << currTick() << std::endl;
                    port->sendReq(new Packet(true, 3588, 4));
                } else if (pc == 3) {
                    std::cout << "Creating memory read request to Addr: 0 for 4 bytes on Tick: " << currTick() << std::endl;
                    port->sendReq(new Packet(true, 0, 4));
                } else if (pc == 4) {
                    std::cout << "Creating memory read request to Addr: 4 for 4 bytes on Tick: " << currTick() << std::endl;
                    port->sendReq(new Packet(true, 4, 4));
                }
                pc++;
            }
            if (pc < 5) {
                schedule(e, currTick()+clk_tick);
            }
        }

        void recvResp(PacketPtr pkt) {
            std::cout << getName() << " received packet response from memory on Tick: " << currTick() << std::endl;
            if (pkt->isRead()) {
                std::cout << getName() << " read: " << *(uint32_t *)(pkt->getBuffer()) << std::endl;
            } else {
                std::cout << getName() << " wrote: " << *(uint32_t *)(pkt->getBuffer()) << std::endl;
            }
            delete(pkt);
        }

        MasterPort* getPort() {return port;}
};
