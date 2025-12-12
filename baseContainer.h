#ifndef BASE_CONTAINER_H
#define BASE_CONTAINER_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <iostream>

template<typename T>
class BaseContainer {
public:
    using value_type = T;
    using size_type = std::size_t;
    using reference = T&;
    using const_reference = const T&;
    
    virtual ~BaseContainer() = default;
    
    // Базовый интерфейс
    virtual size_type size() const noexcept = 0;
    virtual bool empty() const noexcept = 0;
    virtual void clear() = 0;
    
    // Виртуальные методы
    virtual void push_back(const T& value) = 0;
    virtual void push_back(T&& value) = 0;
    virtual void insert(size_type pos, const T& value) = 0;
    virtual void insert(size_type pos, T&& value) = 0;
    virtual void erase(size_type pos) = 0;
    
    virtual reference operator[](size_type idx) = 0;
    virtual const_reference operator[](size_type idx) const = 0;
    
    // Для вывода содержимого
    virtual void print(std::ostream& os = std::cout) const = 0;
    
protected:
    void check_index(size_type idx, size_type size, const char* msg = "Index out of range") const {
        if (idx >= size) {
            throw std::out_of_range(msg);
        }
    }
    
    void check_position(size_type pos, size_type size, const char* msg = "Position out of range") const {
        if (pos > size) {
            throw std::out_of_range(msg);
        }
    }
};

#endif