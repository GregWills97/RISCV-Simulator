#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "event.h"
#include <vector>

class System {
    private:
        Tick currentTick;
        std::vector<Event*> MEQ;
        std::vector<Event*>::iterator findEvent(Event *e);
    public:
        System(): currentTick(0) {};
        void schedule(Event *e, Tick t);
        void reschedule(Event *e, Tick t);
        void runSim(Tick endTick=-1);
        Tick currTick() {return currentTick;}
        void printMEQ();
        Event* popEvent();
};

#endif //__SYSTEM_H__
