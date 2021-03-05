#ifndef __EVENT_H__
#define __EVENT_H__

#include <cinttypes>

#define Tick uint64_t

class Event {
    private:
        Tick eventTime;
        bool scheduled;

    public:
        Event(): eventTime(0), scheduled(false) {};
        void schedule(Tick newTime){
            eventTime = newTime;
            scheduled = true;
        }
        void deschedule() {
            eventTime = -1;
            scheduled = false;
        }
        bool isScheduled() {return scheduled;}
        Tick time() {return eventTime;}
        virtual void process() = 0;
        virtual const char* description() = 0;
};
      
#endif // __EVENT_H__

