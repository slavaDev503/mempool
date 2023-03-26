#ifndef SCOPEDLOCK_H
#define SCOPEDLOCK_H

#include <NonCopyable.h>

template< typename T>
class ScopedLock: private NonCopyable<ScopedLock<T>> {
public:
    explicit ScopedLock(T& obj) : obj_(obj) {
        obj_.lock();
    }

    ~ScopedLock() {
        obj_.unlock();
    }

private:
    T& obj_;
};

#endif //SCOPEDLOCK_H
