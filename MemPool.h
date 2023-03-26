#ifndef MEMPOOL_H
#define MEMPOOL_H

#include <cstddef>
#include <cstdint>
#include <rtos.h> // osWaitForever, Mutex, Semaphore, ScopedLock

enum Res {
    InvalidPtr = -2,
    Error = - 1,
    Ok = 0,
};

template <typename T, size_t poolSize>
class MemPool: private NonCopyable<MemPool<T, poolSize>>{
public:
    /**
     * Конструктор класса
     */
    MemPool();

    /**
     * Выделить блок памяти
     * @param[in] timeout Максимальное время ожидания свободного блока [мс]
     * @return Указатель на выделенный блок, nullptr в случае ошибки
     */
    T *alloc(uint32_t timeout = osWaitForever);

    /**
     * Освободить блок памяти
     * @param[in] ptr Указатель на блок выделенный с помощью метода alloc
     * @return Res::Ok - В случае успеха
     *         Res::InvalidPtr - Указатель на освобождаемый блок неверный
     *         Res::Error - В оставшихся случаях
     */
    Res free(T *block);

    /**
     * Запрос максимального кол-ва блоков
     * @return Кол-во блоков в memory pool
     */
    size_t size() const;

private:
    // Мьютекс для защиты контрольного блока при многопоточном использовании
    Mutex mutex_;
    // Семафор для реализации блокировки выделения памяти
    Semaphore semph_;
    // Контрольный блок. Служит для маркировки признака занятости блока
    uint8_t cb_[((poolSize) + 8 - 1) / 8];
    // Память под блоки
    T mem_[poolSize];
};

template<typename T, size_t poolSize>
MemPool<T, poolSize>::MemPool() : mutex_(), semph_(poolSize), cb_{}, mem_{} {
}

template <typename T, size_t poolSize>
T *MemPool<T, poolSize>::alloc(uint32_t timeout) {
    // Ожидаем свободный блок
    if (!semph_.acquire(timeout)) {
        return nullptr;
    }
    ScopedLock<Mutex> guard(mutex_);
    for (int i = 0; i < sizeof(cb_); i++ ) {
        uint8_t &byte = cb_[i / 8];
        if (byte == 0xff) {
            i += 8;
            continue;
        }
        for (int j = 0; j < poolSize % 8; j++) {
            if (!(byte & (1 << j))) {
                // Помечаем свободный блок как занятый
                byte |= (1 << j);
                // Возвращаем память на блок
                return &mem_[i * 8 + j];
            }
        }
    }
    return nullptr;
}

template<typename T, size_t poolSize>
Res MemPool<T, poolSize>::free(T *block) {
    // Проверка адреса блока
    if (block < mem_ || block > &mem_[poolSize]) {
        // Адрес блока вне диапазона пула
        return Res::InvalidPtr;
    }
    // Нормированный адрес блока
    size_t blockAddr = reinterpret_cast<size_t>(block) - reinterpret_cast<size_t>(mem_);
    if (blockAddr % sizeof(T)) {
        // Неверный адрес (адрес не выровнен на границу блока)
        return Res::InvalidPtr;
    }
    // Номер блока в пуле
    size_t blockNum = blockAddr / sizeof(T);
    ScopedLock<Mutex> guard(mutex_);
    // Сброс бита контрольного блока
    uint8_t &byte = cb_[blockNum / 8];
    const size_t bitMask = 1 << (blockNum % 8);
    if (byte & bitMask) {
        byte &= ~bitMask;
        semph_.release();
        return Res::Ok;
    }
    return Res::Error;
}

template<typename T, size_t poolSize>
size_t MemPool<T, poolSize>::size() const {
    return poolSize;
}

#endif //MEMPOOL_H
