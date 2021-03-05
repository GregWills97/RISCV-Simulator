#ifndef __MEM_H__
#define __MEM_H__

#include <cstdlib>
#include <ctime>
#include <cstring>
#include <iostream>
#include "simobject.h"

class CPU;
class Memory;

class ReadInstEvent : public Event {
    private:
        Memory *device;
    public:
        ReadInstEvent(Memory *sot) : Event(), device(sot) {};
        void process() override;
        virtual const char* description() override {return "Fetch Event Completed";}
};
class ReadDataEvent : public Event {
    private:
        Memory *device;
    public:
        ReadDataEvent(Memory *sot) : Event(), device(sot) {};
        void process() override;
        virtual const char* description() override {return "Read Data Event";}
};

class WriteDataEvent : public Event {
    private:
        Memory *device;
    public:
        WriteDataEvent(Memory *sot) : Event(), device(sot) {};
        void process() override;
        virtual const char* description() override {return "Read Data Event";}
};

class Memory : public SimObject {
    private:
        //ram
        uint8_t *ram;
        //events
        ReadInstEvent *ri;
        ReadDataEvent *rd;
        WriteDataEvent *wd;
        //port addresses
        uint32_t inst_addr, data_addr;
        //cpu pointer
        CPU* cpu;
    public:
        Memory(System *sys) : SimObject(sys), ram(new uint8_t[0x1400]), ri(new ReadInstEvent(this)), rd(new ReadDataEvent(this)), wd(new WriteDataEvent(this)) {};
        void assignCPU(CPU* c) {cpu = c;}
        void setInst_addr(int addr) {inst_addr = addr;}
        void setData_addr(int addr) {data_addr = addr;}
        void schedule_instRead(Tick t) {schedule(ri, t);}
        void schedule_dataRead(Tick t) {schedule(rd, t);}
        void schedule_dataWrite(Tick t) {schedule(wd, t);}
        void send_inst();
        void send_data();
        void write_data();
        virtual void initialize() override;
};

#endif // __MEM_H_
