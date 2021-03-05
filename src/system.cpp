#include "system.h"
#include <iostream>
#include <cassert>

std::vector<Event*>::iterator System::findEvent(Event *e){
    for (std::vector<Event*>::iterator it=MEQ.begin(); it != MEQ.end(); it++){
        if (((*it)->time() == e->time()) && ((*it)->description() == e->description())) {
            return it;
        }
    }
    assert(0);          //not found assert fault
}

void System::schedule(Event *e, Tick t) {
    assert(t >= currentTick);
    std::cout << "Attempting to schedule " << e->description() << " at time " << t << std::endl;
    e->schedule(t);
    for (auto it = MEQ.begin(); it != MEQ.end(); it++) {    //sorted insert
        if (e->time() < (*it)->time()) {
            MEQ.insert(it, e);
            return;
        }
    }
    MEQ.push_back(e);
}


void System::reschedule(Event *e, Tick t) {
    assert(t >= currentTick);
    std::cout << "Attemption to schedule " << e->description() << " at time " << t << std::endl;
    if (e->isScheduled() && t < e->time()) {
        MEQ.erase(findEvent(e));
        e->schedule(t);
        for (auto it = MEQ.begin(); it != MEQ.end(); it++) {    //sorted insert
            if (e->time() < (*it)->time()) {
                MEQ.insert(it, e);
                return;
            }
        }
        MEQ.push_back(e);
        return;
    }
}

void System::runSim(Tick endTick) {
    std::cout << "Simulation Tick: " << currentTick << std::endl;
    while ((currentTick <= endTick) && !(MEQ.empty())) {
        //printMEQ();
        while (MEQ.begin() != MEQ.end()) {
            if (MEQ.front()->time() < currentTick) {
                std::cout << "Event was scheduled prior to current tick " << std::endl;
                assert(0);
            } else if (MEQ.front()->time() == currentTick) {
                popEvent()->process();
            } else {
                break;
            }
        }
        currentTick++;
    }
}

void System::printMEQ() {
    std::cout << "Start of MEQ" << std::endl;
    for (auto e : MEQ) {
        std::cout << e->time() << ":" << e->description() << std::endl;
    }
    std::cout << "End of MEQ" << std::endl;
}

Event* System::popEvent() {
    Event* tmp = MEQ.front();
    tmp->deschedule();
    MEQ.erase(MEQ.begin());
    return tmp;
}
