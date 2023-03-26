#ifndef SEMAPHORE_H
#define SEMAPHORE_H

class Semaphore {
public:
    Semaphore(int count) : counter_(count), maxCount_(count) {

    }

    int acquire(int ms) {
        if (counter_ > 0 ) {
            counter_ --;
            return 1;
        }
        return 0;
    }

    int release() {
        if (counter_ < maxCount_) {
            counter_ ++;
            return 1;
        }
        return 0;
    }

private:
    uint32_t counter_;
    const uint32_t maxCount_;
};

#endif // SEMAPHORE_H
