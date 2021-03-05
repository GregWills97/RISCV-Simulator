#ifndef __SIMOBJECT_H__
#define __SIMOBJECT_H__

#include "event.h"
#include "system.h"

class SimObject {
    private:
        System* sys;
        const char* name;
    public:
        SimObject(System* sys, const char* _name = ""): sys(sys), name(_name) {};
        Tick currTick() {return sys->currTick();}
        void schedule(Event* e, Tick t) {sys->schedule(e, t);}
        void reschedule(Event* e, Tick t) {sys->reschedule(e, t);}
        System* getSys() {return sys;}
        const char* getName() {return name;}
        virtual void initialize() = 0;
};

#endif //__SIMOBJECT_H__
