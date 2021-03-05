#ifndef __CPU_H_
#define __CPU_H_

#include "simobject.h"
#include "ports.h"
#include <iostream>

class Instruction;
class CPU;

class Pipeline : public SimObject {
    protected:
        CPU* owner;                   //pointer to top level cpu
        Instruction* pipelineReg;   //instruction register
        Pipeline* next;      //pointer to next pipeline element

    public:
        Pipeline(System *sys) : SimObject(sys), pipelineReg(NULL) {};
        virtual void recvInst(Instruction* inst) = 0;
        void sendInst(Instruction* inst) {
            if (!(next == NULL)) {
                next->recvInst(pipelineReg);
            }
            pipelineReg = NULL;
        }
        void setNext(Pipeline* _next) {next = _next;}
        bool isBusy() {return (pipelineReg != NULL);}
};

class Fetch : public Pipeline {
    private:
        class FetchEvent : public Event{
            private: 
                Fetch *device;
            public:
                FetchEvent(Fetch *sot) : Event(), device(sot) {};
                void process() {device->process();}
                virtual const char* description() override {return "Fetch Event";}
        };

        Tick clk_tick;
        FetchEvent *fe;

    public:
        Fetch(System *sys, Tick clk_period) : Pipeline(sys), clk_tick(clk_period), fe(new FetchEvent(this)) {};
        virtual void initialize() override {}
        void process();
        void recvInst(Instruction* inst) override;
};

class Decode : public Pipeline {
    private:
        class DecodeEvent : public Event{
            private: 
                Decode *device;
            public:
                DecodeEvent(Decode *sot) : Event(), device(sot) {};
                void process() {device->process();}
                virtual const char* description() override {return "Decode Event";}
        };

        Tick clk_tick;
        DecodeEvent *de;

    public:
        Decode(System *sys, Tick clk_period) : Pipeline(sys), clk_tick(clk_period), de(new DecodeEvent(this)) {};
        virtual void initialize() override {}
        void process();
        void recvInst(Instruction* inst) override;
};

class Execute : public Pipeline {
    private:
        class ExecuteEvent : public Event{
            private: 
                Execute *device;
            public:
                ExecuteEvent(Execute *sot) : Event(), device(sot) {};
                void process() {device->process();}
                virtual const char* description() override {return "Execute Event";}
        };

        Tick clk_tick;
        ExecuteEvent *ee;

    public:
        Execute(System *sys, Tick clk_period) : Pipeline(sys), clk_tick(clk_period), ee(new ExecuteEvent(this)) {};
        virtual void initialize() override {}
        void process();
        void recvInst(Instruction* inst) override;
};

class Writeback : public Pipeline {
    private:
        class WritebackEvent : public Event{
            private: 
                Writeback *device;
            public:
                WritebackEvent(Writeback *sot) : Event(), device(sot) {};
                void process() {device->process();}
                virtual const char* description() override {return "Writeback Event";}
        };

        Tick clk_tick;
        WritebackEvent *we;

    public:
        Writeback(System *sys, Tick clk_period) : Pipeline(sys), clk_tick(clk_period), we(new WritebackEvent(this)) {};
        virtual void initialize() override {}
        void process();
        void recvInst(Instruction* inst) override;
};

class CPU : public SimObject {
    private:
        class ClkEvent : public Event {
            private:
                CPU *device;
            public:
                ClkEvent(CPU *sot) : Event(), device(sot) {};
                void process() {device->process();}
                virtual const char* description() override {return "CPU Clock Event";}
        };

        /*class InstructionPort : public MasterPort {
            private:
                CPU *owner;
                PacketPtr activeRequest;
            public:
                InstructionPort(CPU* _owner) : MasterPort(), owner(_owner) {}
                bool isBusy() {return (activeRequest != NULL);}
                void recvResp(PacketPtr pkt) override {
                    activeRequest = NULL;
                    owner->recvRespInst(pkt);
                }
                void sendReq(PacketPtr pkt) override {
                    activeRequest = pkt;
                    MasterPort::sendReq(pkt);
                }
        };

        class DataPort : public MasterPort {
            private:
                CPU *owner;
                PacketPtr activeRequest;
            public:
                DataPort(CPU* _owner) : MasterPort(), owner(_owner) {}
                bool isBusy() {return (activeRequest != NULL);}
                void recvResp(PacketPtr pkt) override {
                    activeRequest = NULL;
                    owner->recvRespData(pkt);
                }
                void sendReq(PacketPtr pkt) override {
                    activeRequest = pkt;
                    MasterPort::sendReq(pkt);
                }
        };*/

        ClkEvent *clk;
        //InstructionPort* i_port;
        //DataPort* d_port;
        Tick clk_tick;
        //Pipeline elements
        Fetch *f;
        Decode *d;
        Execute *e;
        Writeback *w;
        //Register banks
        uint32_t pc;
        uint32_t integer_bank[32];
        float floating_bank[32];
        //branch predictor
        enum bp {SNT, WNT, WT, ST};
        bp branch_predictor;

    public:
        CPU(System *sys, const char* _name, Tick clk_period) : 
            SimObject(sys, _name), 
            clk(new ClkEvent(this)),
            f(new Fetch(sys, clk_period)),
            d(new Decode(sys, clk_period)),
            e(new Execute(sys, clk_period)),
            w(new Writeback(sys, clk_period)),
            //i_port(new MasterPort(this)),
            //d_port(new MasterPort(this)),
            clk_tick(clk_period),
            pc(0),
            branch_predictor(WT) {
			for (int i = 0; i < 32; i++) {
				integer_bank[i] = 0;
			}
		}

        virtual void initialize() override;
        void process();

        Tick getClkPeriod() {return clk_tick;}
        void recvRespInst(PacketPtr pkt);
        void recvRespData(PacketPtr pkt);

        uint32_t getIntReg(int32_t reg) {return integer_bank[reg];}
        void setIntReg(int32_t reg, int32_t data) {integer_bank[reg] = data;}

        /*bp getBP() {return branch_predictor;}
        void wasTaken();
        void wasNotTaken();
        void setOffset(int off) {offset = off;}
        void setPC() {pc = pc + offset;}
        uint32_t getPC() {return pc;}
        float get_regf(unsigned reg) {return floating_bank[reg];}
        void set_regf(unsigned reg, float data) {floating_bank[reg] = data;}*/
};

class Instruction {
	private:
        uint32_t raw_binary;
        int32_t opcode, rd, funct3, rs1, rs2, funct7, shamt, imm12b, imm20b, result = 0;
        CPU* owner;

    public:
        Instruction(uint32_t data, CPU* _owner) : raw_binary(data), owner(_owner) {};
        void decode();
        void execute();
        void writeBack();
        uint32_t getData() {return raw_binary;}
        int32_t getOpcode() {return opcode;}
        int32_t getrd() {return rd;}
        void setrd(int32_t data) {rd = data;}
        int32_t getfunct3() {return funct3;}
        int32_t getrs1() {return rs1;}
        int32_t getrs2() {return rs2;}
        int32_t getfunct7() {return funct7;}
        int32_t getimm12b() {return imm12b;}
        int32_t getimm20b() {return imm20b;}
        int32_t getResult() {return result;}
};
#endif //__CPU_H__
