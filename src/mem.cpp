#include "mem.h"
#include "cpu.h"

void ReadInstEvent::process() {
    //have memory send instruction port
    std::cout << "Instruction Read" << std::endl;
    device->send_inst();
}

void ReadDataEvent::process() {
    device->send_data();
}

void WriteDataEvent::process() {
    device->write_data();
}

void Memory::send_inst() {
    uint32_t inst;
    //copy data from memory to data variable
    std::memcpy(&inst, &ram[inst_addr], sizeof(inst));
    //send instuction to cpu
    cpu->set_iport(inst);
}

void Memory::send_data() {
    uint32_t data;
    //copy data from memory to data variable
    std::memcpy(&data, &ram[data_addr], sizeof(data));
    //send instuction to cpu
    cpu->set_dport(data);
}

void Memory::write_data() {
    //get data from cpu
    uint32_t data = cpu->get_dport();
    //copy into RAM
    std::memcpy(&ram[data_addr], &data, sizeof(data));
}

void Memory::initialize() {
    //add in instructions
    srand(time(NULL));
    for (int i = 0x400; i < 0xC00; i+=4) {  //fill in temp array with random float
        float x = float(rand());
        float random_float = (float(rand())/float(RAND_MAX)) * x;
        std::memcpy(&ram[i], &random_float, sizeof(float));
    }
}
