#ifndef SIMPLE_VECTOR_H
#define SIMPLE_VECTOR_H

#include "baseContainer.h"
#include <memory>
#include <utility>
#include <stdexcept>
#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <new>

template<typename T>
class SimpleVector : public BaseContainer<T> {
public:
    using value_type = typename BaseContainer<T>::value_type;
    using size_type = typename BaseContainer<T>::size_type;
    using reference = typename BaseContainer<T>::reference;
    using const_reference = typename BaseContainer<T>::const_reference;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using const_pointer = const T*;
    
    // Random Access Iterator
    class Iterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;
        
        Iterator() noexcept : ptr_(nullptr) {}
        explicit Iterator(pointer p) noexcept : ptr_(p) {}
        
        reference operator*() const { return *ptr_; }
        pointer operator->() const { return ptr_; }
        
        Iterator& operator++() { ++ptr_; return *this; }
        Iterator operator++(int) { Iterator tmp = *this; ++ptr_; return tmp; }
        
        Iterator& operator--() { --ptr_; return *this; }
        Iterator operator--(int) { Iterator tmp = *this; --ptr_; return tmp; }
        
        Iterator& operator+=(difference_type n) { ptr_ += n; return *this; }
        Iterator& operator-=(difference_type n) { ptr_ -= n; return *this; }
        
        Iterator operator+(difference_type n) const { return Iterator(ptr_ + n); }
        Iterator operator-(difference_type n) const { return Iterator(ptr_ - n); }
        
        friend Iterator operator+(difference_type n, const Iterator& it) {
            return Iterator(it.ptr_ + n);
        }
        
        difference_type operator-(const Iterator& other) const { 
            return ptr_ - other.ptr_; 
        }
        
        reference operator[](difference_type n) const { return ptr_[n]; }
        
        bool operator==(const Iterator& other) const { return ptr_ == other.ptr_; }
        bool operator!=(const Iterator& other) const { return ptr_ != other.ptr_; }
        bool operator<(const Iterator& other) const { return ptr_ < other.ptr_; }
        bool operator>(const Iterator& other) const { return ptr_ > other.ptr_; }
        bool operator<=(const Iterator& other) const { return ptr_ <= other.ptr_; }
        bool operator>=(const Iterator& other) const { return ptr_ >= other.ptr_; }
        
    private:
        pointer ptr_;
    };
    
    class ConstIterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = const T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;
        
        ConstIterator() noexcept : ptr_(nullptr) {}
        explicit ConstIterator(pointer p) noexcept : ptr_(p) {}
        ConstIterator(const Iterator& it) noexcept : ptr_(&*it) {}
        
        reference operator*() const { return *ptr_; }
        pointer operator->() const { return ptr_; }
        
        ConstIterator& operator++() { ++ptr_; return *this; }
        ConstIterator operator++(int) { ConstIterator tmp = *this; ++ptr_; return tmp; }
        
        ConstIterator& operator--() { --ptr_; return *this; }
        ConstIterator operator--(int) { ConstIterator tmp = *this; --ptr_; return tmp; }
        
        ConstIterator& operator+=(difference_type n) { ptr_ += n; return *this; }
        ConstIterator& operator-=(difference_type n) { ptr_ -= n; return *this; }
        
        ConstIterator operator+(difference_type n) const { return ConstIterator(ptr_ + n); }
        ConstIterator operator-(difference_type n) const { return ConstIterator(ptr_ - n); }
        
        friend ConstIterator operator+(difference_type n, const ConstIterator& it) {
            return ConstIterator(it.ptr_ + n);
        }
        
        difference_type operator-(const ConstIterator& other) const { 
            return ptr_ - other.ptr_; 
        }
        
        reference operator[](difference_type n) const { return ptr_[n]; }
        
        bool operator==(const ConstIterator& other) const { return ptr_ == other.ptr_; }
        bool operator!=(const ConstIterator& other) const { return ptr_ != other.ptr_; }
        bool operator<(const ConstIterator& other) const { return ptr_ < other.ptr_; }
        bool operator>(const ConstIterator& other) const { return ptr_ > other.ptr_; }
        bool operator<=(const ConstIterator& other) const { return ptr_ <= other.ptr_; }
        bool operator>=(const ConstIterator& other) const { return ptr_ >= other.ptr_; }
        
    private:
        pointer ptr_;
    };
    
    using iterator = Iterator;
    using const_iterator = ConstIterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    
    SimpleVector() noexcept = default;
    
    SimpleVector(std::initializer_list<T> init) : size_(init.size()), capacity_(init.size()) {
        if (capacity_ > 0) {
            // Выделяем сырую память
            data_ = static_cast<T*>(::operator new(capacity_ * sizeof(T)));
            size_type i = 0;
            try {
                for (auto it = init.begin(); it != init.end(); ++it, ++i) {
                    new (&data_[i]) T(*it);
                }
            } catch (...) {
                // Уничтожаем созданные объекты
                for (size_type j = 0; j < i; ++j) {
                    data_[j].~T();
                }
                ::operator delete(data_);
                throw;
            }
        }
    }
    
    // Конструктор копирования
    SimpleVector(const SimpleVector& other) : size_(other.size_), capacity_(other.size_) {
        if (capacity_ > 0) {
            data_ = static_cast<T*>(::operator new(capacity_ * sizeof(T)));
            size_type i = 0;
            try {
                for (; i < size_; ++i) {
                    new (&data_[i]) T(other.data_[i]);
                }
            } catch (...) {
                for (size_type j = 0; j < i; ++j) {
                    data_[j].~T();
                }
                ::operator delete(data_);
                throw;
            }
        }
    }
    
    // Конструктор перемещения
    SimpleVector(SimpleVector&& other) noexcept
        : size_(other.size_), capacity_(other.capacity_), data_(other.data_) {
        other.size_ = 0;
        other.capacity_ = 0;
        other.data_ = nullptr;
    }
    
    // Оператор присваивания копированием
    SimpleVector& operator=(const SimpleVector& other) {
        if (this != &other) {
            SimpleVector temp(other);
            swap(temp);
        }
        return *this;
    }
    
    // Оператор присваивания перемещением
    SimpleVector& operator=(SimpleVector&& other) noexcept {
        if (this != &other) {
            clear_memory();
            size_ = other.size_;
            capacity_ = other.capacity_;
            data_ = other.data_;
            
            other.size_ = 0;
            other.capacity_ = 0;
            other.data_ = nullptr;
        }
        return *this;
    }
    
    ~SimpleVector() {
        clear_memory();
    }
    
    // Реализация методов BaseContainer
    size_type size() const noexcept override { return size_; }
    bool empty() const noexcept override { return size_ == 0; }
    
    void clear() override {
        destroy_elements();
        size_ = 0;
    }
    
    void push_back(const T& value) override {
        ensure_capacity(size_ + 1);
        new (&data_[size_]) T(value);
        ++size_;
    }
    
    void push_back(T&& value) override {
        ensure_capacity(size_ + 1);
        new (&data_[size_]) T(std::move(value));
        ++size_;
    }
    
    void insert(size_type pos, const T& value) override {
        this->check_position(pos, size_);
        ensure_capacity(size_ + 1);
        
        if (pos == size_) {
            // Вставка в конец
            new (&data_[size_]) T(value);
        } else {
            // Вставка в середину или начало
            // Создаем новый элемент в конце
            new (&data_[size_]) T(std::move(data_[size_ - 1]));
            
            // Сдвигаем элементы вправо
            for (size_type i = size_; i > pos; --i) {
                data_[i] = std::move(data_[i - 1]);
            }
            
            // Вставляем новый элемент
            data_[pos] = value;
        }
        ++size_;
    }
    
    void insert(size_type pos, T&& value) override {
        this->check_position(pos, size_);
        ensure_capacity(size_ + 1);
        
        if (pos == size_) {
            // Вставка в конец
            new (&data_[size_]) T(std::move(value));
        } else {
            // Вставка в середину или начало
            // Создаем новый элемент в конце
            new (&data_[size_]) T(std::move(data_[size_ - 1]));
            
            // Сдвигаем элементы вправо
            for (size_type i = size_; i > pos; --i) {
                data_[i] = std::move(data_[i - 1]);
            }
            
            // Вставляем новый элемент
            data_[pos] = std::move(value);
        }
        ++size_;
    }
    
    void erase(size_type pos) override {
        this->check_index(pos, size_);
        
        // Уничтожаем элемент на позиции pos
        data_[pos].~T();
        
        // Сдвигаем элементы влево
        for (size_type i = pos; i < size_ - 1; ++i) {
            new (&data_[i]) T(std::move(data_[i + 1]));
            data_[i + 1].~T();
        }
        
        --size_;
    }
    
    reference operator[](size_type idx) override {
        this->check_index(idx, size_);
        return data_[idx];
    }
    
    const_reference operator[](size_type idx) const override {
        this->check_index(idx, size_);
        return data_[idx];
    }
    
    void print(std::ostream& os = std::cout) const override {
        for (size_type i = 0; i < size_; ++i) {
            os << data_[i];
            if (i != size_ - 1) os << " ";
        }
    }
    
    // Итераторы
    iterator begin() noexcept { return iterator(data_); }
    iterator end() noexcept { return iterator(data_ + size_); }
    
    const_iterator begin() const noexcept { return const_iterator(data_); }
    const_iterator end() const noexcept { return const_iterator(data_ + size_); }
    
    const_iterator cbegin() const noexcept { return const_iterator(data_); }
    const_iterator cend() const noexcept { return const_iterator(data_ + size_); }
    
    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    
    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
    
    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }
    const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }
    
    void swap(SimpleVector& other) noexcept {
        using std::swap;
        swap(size_, other.size_);
        swap(capacity_, other.capacity_);
        swap(data_, other.data_);
    }
    
    void reserve(size_type new_cap) {
        if (new_cap <= capacity_) return;
        change_capacity(new_cap);
    }
    
    size_type capacity() const noexcept { return capacity_; }
    
private:
    size_type size_ = 0;
    size_type capacity_ = 0;
    T* data_ = nullptr;
    
    static constexpr double GROWTH_FACTOR = 1.5;
    
    void destroy_elements() {
        if (data_) {
            for (size_type i = 0; i < size_; ++i) {
                data_[i].~T();
            }
        }
    }
    
    void clear_memory() {
        destroy_elements();
        ::operator delete(data_);
        data_ = nullptr;
        size_ = 0;
        capacity_ = 0;
    }
    
    void ensure_capacity(size_type required_capacity) {
        if (required_capacity <= capacity_) return;
        
        size_type new_capacity = capacity_ == 0 ? 1 : 
                                 static_cast<size_type>(capacity_ * GROWTH_FACTOR);
        if (new_capacity < required_capacity) {
            new_capacity = required_capacity;
        }
        
        change_capacity(new_capacity);
    }
    
    void change_capacity(size_type new_capacity) {
        if (new_capacity == capacity_) return;
        
        if (new_capacity < size_) {
            // Нельзя уменьшить capacity меньше текущего размера
            return;
        }
        
        T* new_data = nullptr;
        if (new_capacity > 0) {
            new_data = static_cast<T*>(::operator new(new_capacity * sizeof(T)));
            size_type i = 0;
            try {
                for (; i < size_; ++i) {
                    new (&new_data[i]) T(std::move(data_[i]));
                }
            } catch (...) {
                for (size_type j = 0; j < i; ++j) {
                    new_data[j].~T();
                }
                ::operator delete(new_data);
                throw;
            }
        }
        
        // Уничтожаем старые элементы и освобождаем память
        if (data_) {
            for (size_type i = 0; i < size_; ++i) {
                data_[i].~T();
            }
            ::operator delete(data_);
        }
        
        data_ = new_data;
        capacity_ = new_capacity;
        // size_ не меняется
    }
};

#endif