#include "cache.h"
#include <cassert>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <cmath>

Cache::Cache(System *sys, AddrRange _addrs, size_t _size, size_t _lines, associativity_type a, Tick respLatency) :
    SimObject(sys),
    addrs(_addrs),
    size(_size),
    line_size(_lines),
    assoc(a),
    responseLatency(respLatency),
    mem_port(new MemPort(this)),
    cpu_port(new CPUPort(this)),
    e(new CacheEvent(this)) {

    miss_num = 0;
    hit_num = 0;

    //if lines does not divide into cache size exit
    assert((size % line_size) == 0);
    memory = (uint8_t*)std::malloc(size);

    uint32_t num_sets;

    if (a == none) {    //if no associativity
        num_sets = size / line_size;
        //initializes table
        for (int i = 0; i < num_sets; i++) {
            lineData* table = new lineData();
            table->tag.push_back(0); 
            table->valid_bits.push_back(false); 
            table->dirty_bits.push_back(false); 
            //mru bits not needed
            maps.push_back(table);
        }
    }/* else if (a == two_way) {
        num_sets = size / (2*line_size);
        for (int i = 0; i < 2; i++) {
            lineData* table = new lineData();
            maps.push_back(table);
            //initialize LRU bits
            mru_bits.push_back(vec);
        }
        for (int i = 0; i < num_sets; i++) {
            lineData* table = new lineData();
            for (int j = 0; j < 2; j++) {
                table.tag.push_back(0); //mru bits not needed
            }
            maps.push_back(table);
        }
    } else if (a == four_way) {
        num_sets = size / (4*line_size);
        //initializes table
        std::vector<bool> vec = {false, false, false, false};
        for(int x = 0; x < 4; x++) {
            lineData* table = new lineData;
            maps.push_back(table);
            //initialize LRU bits
            mru_bits.push_back(vec);
        }
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < num_sets; j++) {
                maps[i]->tag.push_back(0);
                maps[i]->valid_bits.push_back(false);
            }
        }
    } else {
        //fully mapped
        std::cout << "Still working on implementation" << std::endl;
    }*/
}

void Cache::process() {
    std::cout << "Cache is responding to a request on Tick: " << currTick() << std::endl;
    cpu_port->sendResp(request);
}

void Cache::recvReq(PacketPtr pkt) {
    std::cout << "Cache received a request on Tick: " << currTick() << std::endl;
    if (pkt->isRead()) {
        if (isHit(pkt->getAddr())) {    //if hit just send slave port data
            std::cout << "Cache Read Hit on Tick: " << currTick() << std::endl;
            //update LRU when read
            getDataAtAddr(pkt->getAddr(), pkt->getBuffer(), pkt->getSize());
            schedule(e, currTick()+responseLatency);
            hit_num += 1;
        } else {                        //miss we must schedule memory read
            std::cout << "Cache Read Miss on Tick: " << currTick() << std::endl;
            uint32_t offset = pkt->getAddr() % line_size;
            uint32_t set, curr_tag, num_sets;
            if (!lineDirty(pkt->getAddr())) {   //if not dirty just evict
                //get offset to calculate beginning of line
                //update line
                PacketPtr cache_line = new Packet(true, (pkt->getAddr() - offset), line_size);
                mem_port->sendReq(cache_line);
            } else {    //if it is dirty, write-back cache line
                std::cout << "Writing back dirty cache line" << std::endl;
                if (assoc == none) {
                    num_sets = size / line_size;
                    Addr mem_addr = pkt->getAddr() / line_size;
                    set = (int)(mem_addr % num_sets) << (int)std::log2(line_size);
                    curr_tag = maps[set]->tag[0];
                    //shift tag value to beginning of addr by adding set and offset bits
                    curr_tag = curr_tag << (int)(std::log2(num_sets) + std::log2(line_size));
                    mem_addr = curr_tag + set;
                    PacketPtr cache_line = new Packet(false, mem_addr, line_size);
                    getDataAtAddr(cache_line->getAddr(), cache_line->getBuffer(), cache_line->getSize());   //load data into buffer
                    mem_port->sendReq(cache_line);
                }
            }
            miss_num += 1;
        }
    } else {
        if (isHit(pkt->getAddr())) {
            std::cout << "Cache Write Hit on Tick: " << currTick() << std::endl;
            //write-back so just write to cache for now
            setDataAtAddr(pkt->getAddr(), pkt->getBuffer(), pkt->getSize());
            schedule(e, currTick()+responseLatency);
            hit_num += 1;
        } else {
            //miss
            std::cout << "Cache Write Miss on Tick: " << currTick() << std::endl;
            uint32_t offset = pkt->getAddr() % line_size;
            uint32_t set, curr_tag, num_sets;
            if (!lineDirty(pkt->getAddr())) {   //if not dirty just evict
                //get offset to calculate beginning of line
                //update line
                PacketPtr cache_line = new Packet(true, (pkt->getAddr() - offset), line_size);
                mem_port->sendReq(cache_line);
            } else {    //if it is dirty, write-back cache line
                std::cout << "Writing back dirty cache line" << std::endl;
                if (assoc == none) {
                    num_sets = size / line_size;
                    Addr mem_addr = pkt->getAddr() / line_size;
                    set = (int)(mem_addr % num_sets) << (int)std::log2(line_size);
                    curr_tag = maps[set]->tag[0];
                    //shift tag value to beginning of addr by adding set and offset bits
                    curr_tag = curr_tag << (int)(std::log2(num_sets) + std::log2(line_size));
                    mem_addr = curr_tag + set;
                    PacketPtr cache_line = new Packet(false, mem_addr, line_size);
                    getDataAtAddr(cache_line->getAddr(), cache_line->getBuffer(), cache_line->getSize());   //load data into buffer
                    mem_port->sendReq(cache_line);
                }
            }
            miss_num += 1;
        }
    }
    request = pkt;
}

void Cache::recvResp(PacketPtr pkt) {
    std::cout << "Cache received response from memory on tick: " << currTick() << " with size: " << pkt->getSize() << std::endl;
    if (pkt->isRead()) {  //if new line write to cache
        setDataAtAddr(pkt->getAddr(), pkt->getBuffer(), pkt->getSize());

        if (request->isRead()) {    //if read get data for cpu
            getDataAtAddr(request->getAddr(), request->getBuffer(), request->getSize());
        } else {   //if write, write to the cache line
            setDataAtAddr(request->getAddr(), request->getBuffer(), request->getSize());
        }
        //schedule send response
        schedule(e, currTick()+responseLatency);

    } else {
        //after write-back we must complete original cache line request
        uint32_t offset = pkt->getAddr() % line_size;
        PacketPtr cache_line = new Packet(true, (request->getAddr() - offset), line_size);
        mem_port->sendReq(cache_line);     
    }
}

void Cache::setDataAtAddr(Addr ad, uint8_t* buff, size_t len) {
    assert((ad >= addrs.first) && ((ad+len) <= addrs.second+1));
    uint32_t set;
    uint32_t offset;

    if (assoc == none) {
        Addr cache_addr = ad % size;
        //find set of addr by shifting out offset bits
        set = cache_addr >> (int)std::log2(line_size);
        maps[set]->tag[0] = ad / size;   //set tag bit
        maps[set]->valid_bits[0] = true; //set valid bit
        if (len == line_size) {  //if whole line reset dirty bits 
            std::cout << "Writing line to Set: " << set << std::endl;
            maps[set]->dirty_bits[0] = false;
        } else {    //writing to cache from processor set dirty bit
            std::cout << "Writing data to Set: " << set << std::endl;
            maps[set]->dirty_bits[0] = true;
        }
        std::memcpy((memory+cache_addr), buff, len);
    } 

    /*else if (assoc == two_way) {
        Addr cache_addr = ad % (size / 2);
        offset = ad % line_size;
        set = cache_addr >> (int)std::log2(line_size);
        uint32_t line_addr;
        //iterate through ways to find correct line
        for (int i = 0; i < 2; i++) {
            if (mru_bits[i][set] == false) {
                //update mru and valid bit and tag
                mru_bits[i][set] = true;
                maps[i]->valid_bits[set] = true;
                maps[i]->tag[set] = ad / (size / 2);
                line_addr = (set << 1) + i;
                std::cout << "Writing line to way: " << i << " and Set: " << set << std::endl;
                break;
            }
        }
        //update MRU bits if all set
        bool oneFalse = false;
        for (int j = 0; j < 2; j++) {
            if (mru_bits[j][set] == false) {
                oneFalse = true;
                break;
            }
        }
        //if all are set make them all false
        if (oneFalse == false) {
            for (int k = 0; k < 2; k++) {
                mru_bits[k][set] = false;
            }
        }
        //add back in offset
        cache_addr = (line_addr << (int)std::log2(line_size)) + offset;
        std::memcpy((memory+cache_addr), buff, len);
    } 

    else if (assoc == four_way) {
        Addr cache_addr = ad % (size / 4);
        offset = ad % line_size;
        set = cache_addr >> (int)std::log2(line_size);
        uint32_t line_addr;
        //iterate through ways to find correct line
        for (int i = 0; i < 4; i++) {
            if (mru_bits[i][set] == false) {
                //update mru and valid bit and tag
                mru_bits[i][set] = true;
                maps[i]->valid_bits[set] = true;
                maps[i]->tag[set] = ad / (size / 4);
                line_addr = (set << 2) + i;
                std::cout << "Writing line to way: " << i << " and Set: " << set << std::endl;
                break;
            }
        }
        //update MRU bits if all set
        bool oneFalse = false;
        for (int j = 0; j < 2; j++) {
            if (mru_bits[j][set] == false) {
                oneFalse = true;
                break;
            }
        }
        //if all are set make them all false
        if (oneFalse == false) {
            for (int k = 0; k < 2; k++) {
                mru_bits[k][set] = false;
            }
        }
        //add back in offset
        cache_addr = (line_addr << (int)std::log2(line_size)) + offset;
        std::memcpy((memory+cache_addr), buff, len);
    } */
}

void Cache::getDataAtAddr(Addr ad, uint8_t* buff, size_t len) {
    assert((ad >= addrs.first) && ((ad+len) <= addrs.second+1));
    uint32_t set;
    uint32_t offset;

    if (assoc == none) {
        Addr cache_addr = ad % size;
        set = cache_addr >> (int)std::log2(line_size);
        std::cout << "Reading from Set: " << set << std::endl;
        std::memcpy(buff, (memory+cache_addr), len);
    }/* else if (assoc == two_way) {
        Addr cache_addr = ad % (size / 2);
        offset = ad % line_size;
        set = cache_addr >> (int)std::log2(line_size);
        uint32_t add_tag = ad / (size / 2);
        uint32_t line_addr = 0;
        for (int i = 0; i < 2; i++) {
            if (maps[i]->tag[set] == add_tag) {
                line_addr = (set << 1) + i;
                mru_bits[i][set] = true;
                std::cout << "Found line in way: " << i << " and Set: " << set << std::endl;
                break;
            }
        }
        //check if all mru bits are set if they are reset
        bool oneFalse = false;
        for (int j = 0; j < 2; j++) {
            if (mru_bits[j][set] == false) {
                oneFalse = true;
                break;
            }
        }
        //if all are set make them all false
        if (oneFalse == false) {
            for (int k = 0; k < 2; k++) {
                mru_bits[k][set] = false;
            }
        }
        cache_addr = (line_addr << (int)std::log2(line_size)) + offset;
        std::memcpy(buff, (memory+cache_addr), len);
    } else if (assoc == four_way) {
        Addr cache_addr = ad % (size / 4);
        offset = ad % line_size;
        set = cache_addr >> (int)std::log2(line_size);
        uint32_t add_tag = ad / (size / 4);
        uint32_t line_addr = 0;
        for (int i = 0; i < 4; i++) {
            if (maps[i]->tag[set] == add_tag) {
                line_addr = (set << 2) + i;
                mru_bits[i][set] = true;
                std::cout << "Found line in way: " << i << " and Set: " << set << std::endl;
                break;
            }
        }
        //check if all mru bits are set if they are reset
        bool oneFalse = false;
        for (int j = 0; j < 4; j++) {
            if (mru_bits[j][set] == false) {
                oneFalse = true;
                break;
            }
        }
        //if all are set make them all false
        if (oneFalse == false) {
            for (int k = 0; k < 4; k++) {
                mru_bits[k][set] = false;
            }
        }
        cache_addr = (line_addr << (int)std::log2(line_size)) + offset;
        std::memcpy(buff, (memory+cache_addr), len);
    }*/
}

bool Cache::isHit(Addr ad) {
    Addr cache_addr;
    uint32_t ad_tag;
    uint32_t set;
    if (assoc == none) {
        cache_addr = ad % size;
        ad_tag = ad / size;
        set = cache_addr >> (int)std::log2(line_size);
        if ((maps[set]->valid_bits[0]) && (maps[set]->tag[0] == ad_tag)) {
            return true;
        } else {
            return false;
        }
    } else if (assoc == two_way) {
        cache_addr = ad % (size / 2);
        ad_tag = ad / (size / 2);
        set = cache_addr >> (int)std::log2(line_size);
        //check in ways
        for (int i = 0; i < 2; i++) {
            if ((maps[i]->valid_bits[set]) && (maps[i]->tag[set] == ad_tag)) {
                return true;
            }
        }
        //if not found return false
        return false;
    } else if (assoc == four_way) {
        cache_addr = ad % (size / 4);
        ad_tag = ad / (size / 4);
        set = cache_addr >> (int)std::log2(line_size);
        //check in ways
        for (int i = 0; i < 4; i++) {
            if ((maps[i]->valid_bits[set]) && (maps[i]->tag[set] == ad_tag)) {
                return true;
            }
        }
        //if not found return false
        return false;
    } else {
        return false;
    }
}

bool Cache::lineDirty(Addr ad) {
    Addr cache_addr;
    uint32_t ad_tag;
    uint32_t set;
    if (assoc == none) {
        cache_addr = ad % size;
        set = cache_addr >> (int)std::log2(line_size);
        return maps[set]->dirty_bits[0];
    } else {
        return true;
    }
}
