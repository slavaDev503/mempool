#ifndef MUTEX_H
#define MUTEX_H

#include "NonCopyable.h"

class Mutex: private NonCopyable<Mutex> {
public:
    void lock() {

    }
    void unlock() {

    }
};

#endif //MUTEX_H
