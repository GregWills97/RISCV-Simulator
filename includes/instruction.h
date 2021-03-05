#ifndef __INSTRUCTION_H__
#define __INSTRUCTION_H__

#include <string>
#include <vector>
#include "cpu.h"


class CPU;

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
        void setresult(int32_t data) {result = data;}
};

#endif //__INSTRUCTION_H__
