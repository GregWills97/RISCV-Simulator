#include "instruction.h"

void Instruction::decode() {
    int op_mask = 6;
    result = 0;
    op_mask = (1 << op_mask) - 1;
    opcode = raw_binary & op_mask;

    unsigned bit20, bit10_1, bit11, bit19_12, bit4_1, bit10_5, bit12, bit4_0, bit11_5;

    switch(opcode) {

        case 0x37 :     //LUI
            rd = (((1 << 5) - 1) & (raw_binary >> 7));
            imm20b = (((1 << 20) - 1) & (raw_binary >> 12));
            break;

        case 0x17 :     //AUIPC
            rd = (((1 << 5) - 1) & (raw_binary >> 7));
            imm20b = (((1 << 20) - 1) & (raw_binary >> 12));
            break;

        case 0x6F :     //JAL
            rd = (((1 << 5) - 1) & (raw_binary >> 7));
            bit20 = (raw_binary & (1<<31)) != 0;
            bit10_1 = (((1 << 10) - 1) & (raw_binary >> 21));
            bit11 = (raw_binary & (1<<20)) != 0;
            bit19_12 = (((1 << 8) - 1) & (raw_binary >> 12)); 
            imm20b = (bit20 << 8) | bit19_12;
            imm20b = (imm20b << 1) | bit11;
            imm20b = (imm20b << 9) | bit10_1;
            imm20b = imm20b << 1;       //add on last bit 0
            break;

        case 0x67 :     //JALR
            rd = (((1 << 5) - 1) & (raw_binary >> 7));
            funct3 = 0;
            rs1 = (((1 << 5) - 1) & (raw_binary >> 15));
            imm12b = (((1 << 12) - 1) & (raw_binary >> 20));
            break;

        case 0x63 :     //BEQ, BNE, BLT, BGE, BLTU, BGEU 
            bit11 = (raw_binary & (1 << 7)) != 0;
            bit4_1 = (((1 << 4) - 1) & (raw_binary >> 8));
            funct3 = (((1 << 3) - 1) & (raw_binary >> 12));
            rs1 = (((1 << 5) - 1) & (raw_binary >> 15));
            rs2 = (((1 << 5) - 1) & (raw_binary >> 20));
            bit10_5 = (((1 << 6) - 1) & (raw_binary >> 25));
            bit12 = (raw_binary & (1<<31)) != 0;
            imm12b = (bit12 << 1) | bit11;
            imm12b = (imm12b << 6) | bit10_5;
            imm12b = (imm12b << 4) | bit4_1;
            //add on last zero
            imm12b = imm12b << 1;
            break;

        case 0x3 :      //LB, LH, LW, LBU, LHU
            rd = (((1 << 5) - 1) & (raw_binary >> 7));
            funct3 = (((1 << 3) - 1) & (raw_binary >> 12));
            rs1 = (((1 << 5) - 1) & (raw_binary >> 15));
            imm12b = (((1 << 12) - 1) & (raw_binary >> 20));
            break;

        case 0x23 :     //SB, SH, SW
            bit4_0 = (((1 << 5) - 1) & (raw_binary >> 7));
            funct3 = (((1 << 3) - 1) & (raw_binary >> 12));
            rs1 = (((1 << 5) - 1) & (raw_binary >> 15));
            rs2 = (((1 << 5) - 1) & (raw_binary >> 20));
            bit11_5 = (((1 << 7) - 1) & (raw_binary >> 25));
            imm12b = (bit11_5 << 5) | bit4_0;
            break;

        case 0x13 :     //ADDI, SLTI, SLTIU, XORI, ORI, ANDI, SLLI, SRLI, SRAI
            rd = (((1 << 5) - 1) & (raw_binary >> 7));
            funct3 = (((1 << 3) - 1) & (raw_binary >> 12));
            rs1 = (((1 << 5) - 1) & (raw_binary >> 15));
            shamt = (((1 << 5) - 1) & (raw_binary >> 20));
            funct7 = (((1 << 7) - 1) & (raw_binary >> 25));
            imm12b = (((1 << 12) - 1) & (raw_binary >> 20));
            if (imm12b > 2048) {    //this immediate is signed so we must flip
                imm12b *= -1;
            }
            break;

        case 0x33 :     //ADD, SUB, SLL, SLT, SLTU, XOR, SRL, SRA, OR, AND, all RV32M 
            rd = (((1 << 5) - 1) & (raw_binary >> 7));
            funct3 = (((1 << 3) - 1) & (raw_binary >> 12));
            rs1 = (((1 << 5) - 1) & (raw_binary >> 15));
            rs2 = (((1 << 5) - 1) & (raw_binary >> 20));
            funct7 = (((1 << 7) - 1) & (raw_binary >> 25));
            break;

        case 0x7 :      //FLW
            rd = (((1 << 5) - 1) & (raw_binary >> 7));
            funct3 = (((1 << 3) - 1) & (raw_binary >> 12));
            rs1 = (((1 << 5) - 1) & (raw_binary >> 15));
            imm12b = (((1 << 12) - 1) & (raw_binary >> 20));
            break;

        case 0x27 :     //FSW
            bit4_0 = (((1 << 5) - 1) & (raw_binary >> 7));
            funct3 = (((1 << 3) - 1) & (raw_binary >> 12));
            rs1 = (((1 << 5) - 1) & (raw_binary >> 15));
            rs2 = (((1 << 5) - 1) & (raw_binary >> 20));
            bit11_5 = (((1 << 7) - 1) & (raw_binary >> 25));
            imm12b = (bit11_5 << 5) | bit4_0;
            break;

        case 0x43 :     //FMADD.S
            rd = (((1 << 5) - 1) & (raw_binary >> 7));
            funct3 = (((1 << 3) - 1) & (raw_binary >> 12));
            rs1 = (((1 << 5) - 1) & (raw_binary >> 15));
            rs2 = (((1 << 5) - 1) & (raw_binary >> 20));
            funct7 = (((1 << 7) - 1) & (raw_binary >> 25));
            break;

        case 0x47 :     //FMSUB.S
            rd = (((1 << 5) - 1) & (raw_binary >> 7));
            funct3 = (((1 << 3) - 1) & (raw_binary >> 12));
            rs1 = (((1 << 5) - 1) & (raw_binary >> 15));
            rs2 = (((1 << 5) - 1) & (raw_binary >> 20));
            funct7 = (((1 << 7) - 1) & (raw_binary >> 25));
            break;

        case 0x4B :     //FNMSUB.S
            rd = (((1 << 5) - 1) & (raw_binary >> 7));
            funct3 = (((1 << 3) - 1) & (raw_binary >> 12));
            rs1 = (((1 << 5) - 1) & (raw_binary >> 15));
            rs2 = (((1 << 5) - 1) & (raw_binary >> 20));
            funct7 = (((1 << 7) - 1) & (raw_binary >> 25));
            break;

        case 0x4F :     //FNMADD.S
            rd = (((1 << 5) - 1) & (raw_binary >> 7));
            funct3 = (((1 << 3) - 1) & (raw_binary >> 12));
            rs1 = (((1 << 5) - 1) & (raw_binary >> 15));
            rs2 = (((1 << 5) - 1) & (raw_binary >> 20));
            funct7 = (((1 << 7) - 1) & (raw_binary >> 25));
            break;

        case 0x53 :     //Rest of RV32F instructions
            rd = (((1 << 5) - 1) & (raw_binary >> 7));
            funct3 = (((1 << 3) - 1) & (raw_binary >> 12));
            rs1 = (((1 << 5) - 1) & (raw_binary >> 15));
            rs2 = (((1 << 5) - 1) & (raw_binary >> 20));
            funct7 = (((1 << 7) - 1) & (raw_binary >> 25));
            break;
    }
}

void Instruction::execute() {

    int32_t opcode = getOpcode();
	switch(opcode) {
	
	        /*case 0x37 :     //LUI
	            {
	                //set result upper20bits to immediate value
	                curr_inst->setresult(curr_inst->getimm20b() << 12);
	                if (!this->next()->isStalled()) {
	                    this->unstall();
	                    schedule(this->next()->getEvent(), currTick()+10);
	                    this->next()->writeReg(this->getReg());
	                } else {
	                    this->stall();
	                    schedule(ee, currTick()+10);
	                }
	            }
	            break;
	
	        case 0x17 :     //AUIPC
	            {
	                //set upper 20 bits of offset
	                int32_t imm_offset = 0;
	                imm_offset = curr_inst->getimm20b() << 12;
	                //add offset to PC
	                curr_inst->setresult(imm_offset + this->getCPU()->getPC());
	                this->unstall();
	                schedule(this->next()->getEvent(), currTick()+10);
	                this->next()->writeReg(this->getReg());
	            }
	            break;
	
	        case 0x6F :     //JAL
	            {
	                int32_t imm_offset = curr_inst->getimm20b();
	                //store pc in rd
	                curr_inst->setrd(this->getCPU()->getPC()+4);
	                this->getCPU()->setOffset(imm_offset);
	                this->unstall();
	                schedule(this->next()->getEvent(), currTick()+10);
	                this->next()->writeReg(this->getReg());
	            }
	            break;
	
	        case 0x67 :     //JALR
	            {
	                int32_t imm_offset = 0;
	                imm_offset = curr_inst->getimm12b();
	                //store pc in rd
	                curr_inst->setrd(this->getCPU()->getPC()+4);
	                //set offset to rs1 + imm
	                this->getCPU()->setOffset(this->getCPU()->get_regi(curr_inst->getrs1()) + imm_offset);
	                this->unstall();
	                schedule(this->next()->getEvent(), currTick()+10);
	                this->next()->writeReg(this->getReg());
	            }
	            break;
	
	        case 0x63 :     //Branch instruction
	            {
	                int32_t arg1 = this->getCPU()->get_regi(curr_inst->getrs1());
	                int32_t arg2 = this->getCPU()->get_regi(curr_inst->getrs2());
	                uint32_t uarg1 = this->getCPU()->get_regi(curr_inst->getrs1());
	                uint32_t uarg2 = this->getCPU()->get_regi(curr_inst->getrs2());
	                int32_t offset = curr_inst->getimm12b();
	                switch(curr_inst->getfunct3()) {
	                    
	                        case 0 :        //BEQ
	                            if (arg1 == arg2) {
	                                this->getCPU()->setOffset(offset);
	                                this->getCPU()->wasTaken();
	                            } else{
	                                this->getCPU()->setOffset(0);
	                                this->getCPU()->wasNotTaken();
	                            }
	                            break;
	                        case 1 :        //BNE
	                            if (arg1 != arg2) {
	                                this->getCPU()->setOffset(offset);
	                                this->getCPU()->wasTaken();
	                            } else{
	                                this->getCPU()->setOffset(0);
	                                this->getCPU()->wasNotTaken();
	                            }
	                            break;
	                        case 4 :        //BLT
	                            if (arg1 < arg2) {
	                                this->getCPU()->setOffset(offset);
	                                this->getCPU()->wasTaken();
	                            } else{
	                                this->getCPU()->setOffset(0);
	                                this->getCPU()->wasNotTaken();
	                            }
	                            break;
	                        case 5 :        //BGT
	                            if (arg1 > arg2) {
	                                this->getCPU()->setOffset(offset);
	                                this->getCPU()->wasTaken();
	                            } else{
	                                this->getCPU()->setOffset(0);
	                                this->getCPU()->wasNotTaken();
	                            }
	                            break;
	                        case 6 :        //BLTU
	                            if (uarg1 < uarg2) {
	                                this->getCPU()->setOffset(offset);
	                                this->getCPU()->wasTaken();
	                            } else{
	                                this->getCPU()->setOffset(0);
	                                this->getCPU()->wasNotTaken();
	                            }
	                            break;
	                        case 7 :        //BGEU
	                            if (uarg1 >= uarg2) {
	                                this->getCPU()->setOffset(offset);
	                                this->getCPU()->wasTaken();
	                            } else{
	                                this->getCPU()->setOffset(0);
	                                this->getCPU()->wasNotTaken();
	                            }
	                            break;
	                        default :
	                            this->getCPU()->setOffset(0);
	                            break;
	                    }
	            }
	            break;
	
	        case 0x3 :      //load instructions
	            {
	                switch(curr_inst->getfunct3()) {
	                    case 2 :    //LW
	                        //set address
	                        this->getCPU()->getMem()->setData_addr(curr_inst->getrs1() + curr_inst->getimm12b());
	                        //wait for ram latency
	                        this->getCPU()->getMem()->schedule_dataRead(currTick()+20);
	                        //set stall
	                        this->stall();
	                        break;
	                }
	            }
	            break;
	
	        case 0x23 :     //store instructions
	            {
	                switch(curr_inst->getfunct3()) {
	                    case 2 :    //SW
	                        //set address and data
	                        this->getCPU()->getMem()->setData_addr(curr_inst->getrs1() + curr_inst->getimm12b());
	                        this->getCPU()->set_dport(this->getCPU()->get_regi(curr_inst->getrs2()));
	                        //wait for ram latency
	                        this->getCPU()->getMem()->schedule_dataWrite(currTick()+20);
	                        this->stall();
	                        break;
	                }
	            }*/
	
	        case 0x13 :     //immediate instructions
	            {
	                int32_t reg1 = owner->getIntReg(getrs1());
	                int32_t imm  = getimm12b();
	                switch(getfunct3()) {
	                    case 0 :    //ADDI
	                        setresult(reg1 + imm);
	                        break;
	                    case 1 :    //SLLI
	                        setresult(reg1 << imm);
	                        break;
	                }
	            }
	
	        /*case 0x33 :     //rtype instructions
	            {
	                int32_t reg1 = this->getCPU()->get_regi(curr_inst->getrs1());
	                int32_t reg2 = this->getCPU()->get_regi(curr_inst->getrs2());
	                uint32_t ureg1 = this->getCPU()->get_regi(curr_inst->getrs1());
	                uint32_t ureg2 = this->getCPU()->get_regi(curr_inst->getrs2());
	                switch(curr_inst->getfunct3()) {
	                    case 0 :    
	                        if (curr_inst->getfunct7() == 0) {  //ADD
	                            curr_inst->setresult(reg1 + reg2);
	                        } else {      //SUB
	                            curr_inst->setresult(reg1 - reg2);
	                        }
	                        break;
	                    case 1 :    //SLL
	                        curr_inst->setresult(reg1 << reg2);
	                        break;
	                    case 2 :    //SLT
	                        if (reg1 < reg2) {
	                            curr_inst->setresult(1);
	                        } else {
	                            curr_inst->setresult(0);
	                        }
	                        break;
	                    case 3 :    //SLTU
	                        if (ureg1 < ureg2) {
	                            curr_inst->setresult(1);
	                        } else {
	                            curr_inst->setresult(0);
	                        }
	                        break;
	                    case 4 :    //XOR
	                        curr_inst->setresult(reg1 ^ reg2);
	                        break;
	                    case 5 :    //SRL
	                        curr_inst->setresult(reg1 >> reg2);
	                        break;
	                    case 6 :    //OR
	                        curr_inst->setresult(reg1 | reg2);
	                        break;
	                    case 7 :    //AND
	                        curr_inst->setresult(reg1 & reg2);
	                        break;
	                }
	                this->unstall();
	                schedule(this->next()->getEvent(), currTick()+10);
	                this->next()->writeReg(this->getReg());
	            }*/
	        default:
	            break;
	    }
}

void Instruction::writeBack() {
    //write to destination register with result from execute stage
    std::cout << "Writing to register " << getrd() << " with a result of " << getResult() << std::endl;
    owner->setIntReg(getrd(), getResult());
}
