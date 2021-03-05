#include <iostream>

#include "cpu.h"
//#include "memrequester.h"
//#include "DRAM.h"
//#include "membus.h"
//#include "cache.h"

int main() {
    System *sys = new System();
    CPU *p0 = new CPU(sys, "p0", 10);
    //memrequester* dev0 = new memrequester(sys, "dev0", 0, 1023);
    //Cache* cash = new Cache(sys, AddrRange(0, 4096), 256, 32, none, 3);
    //DRAM* ram = new DRAM(sys, AddrRange(0, 4096), 100);
    //Membus* bus = new Membus(sys, 0);


    //dev0->getPort()->bind(cash->getCPUPort());
    //cash->getMemPort()->bind(bus->getUnboundCPUSidePort());
    //ram->getPort()->bind(bus->getUnboundMemSidePort());

    //for (auto i = ram->getAddrRange().first; i < ram->getAddrRange().second; i+=4) {
    //    //float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    //    uint32_t val = *(uint32_t *)(&i);
    //    ram->writeWordAtAddr(i, val);
    //}

    //dev0->initialize();
    p0->initialize();
    sys->runSim(200);


    //std::cout << std::endl;
    //std::cout << std::endl;
    //std::cout << "Totall Time in ticks: " << sys->currTick() << std::endl;
    //std::cout << "Total Reads: " << cash->getNumHits() + cash->getNumMisses() << std::endl;
    //std::cout << "Total Hits: " << cash->getNumHits() << " Percentage: " << ((double)cash->getNumHits()/(double)(cash->getNumHits()+cash->getNumMisses()))*100 << "%" << std::endl;
    //std::cout << "Total Misses: " << cash->getNumMisses() << " Percentage: " << ((double)cash->getNumMisses()/(double)(cash->getNumHits()+cash->getNumMisses()))*100 << "%" << std::endl;
    return 0;
}
