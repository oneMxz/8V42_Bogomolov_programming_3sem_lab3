#ifndef SINGLY_LINKED_LIST_H
#define SINGLY_LINKED_LIST_H

#include "baseContainer.h"
#include <memory>
#include <utility>
#include <stdexcept>
#include <initializer_list>
#include <iterator>

template<typename T>
class SinglyLinkedList : public BaseContainer<T> {
private:
    struct Node {
        T data;
        std::unique_ptr<Node> next;
        
        Node(const T& v) : data(v), next(nullptr) {}
        Node(T&& v) : data(std::move(v)), next(nullptr) {}
        Node(const T& v, std::unique_ptr<Node> next_ptr) : data(v), next(std::move(next_ptr)) {}
    };
    
public:
    using value_type = typename BaseContainer<T>::value_type;
    using size_type = typename BaseContainer<T>::size_type;
    using reference = typename BaseContainer<T>::reference;
    using const_reference = typename BaseContainer<T>::const_reference;
    
    // Forward Iterator
    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;
        
        Iterator() noexcept : node_(nullptr) {}
        explicit Iterator(Node* node) noexcept : node_(node) {}
        
        reference operator*() const { return node_->data; }
        pointer operator->() const { return &node_->data; }
        
        Iterator& operator++() {
            node_ = node_->next.get();
            return *this;
        }
        
        Iterator operator++(int) {
            Iterator temp = *this;
            node_ = node_->next.get();
            return temp;
        }
        
        bool operator==(const Iterator& other) const { return node_ == other.node_; }
        bool operator!=(const Iterator& other) const { return node_ != other.node_; }
        
    private:
        Node* node_;
    };
    
    class ConstIterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = const T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;
        
        ConstIterator() noexcept : node_(nullptr) {}
        explicit ConstIterator(Node* node) noexcept : node_(node) {}
        ConstIterator(const Iterator& it) noexcept : node_(it.node_) {}
        
        reference operator*() const { return node_->data; }
        pointer operator->() const { return &node_->data; }
        
        ConstIterator& operator++() {
            node_ = node_->next.get();
            return *this;
        }
        
        ConstIterator operator++(int) {
            ConstIterator temp = *this;
            node_ = node_->next.get();
            return temp;
        }
        
        bool operator==(const ConstIterator& other) const { return node_ == other.node_; }
        bool operator!=(const ConstIterator& other) const { return node_ != other.node_; }
        
    private:
        Node* node_;
    };
    
    using iterator = Iterator;
    using const_iterator = ConstIterator;
    
    SinglyLinkedList() = default;
    
    SinglyLinkedList(std::initializer_list<T> init) {
        for (const auto& item : init) {
            push_back(item);
        }
    }
    
    // Конструктор копирования
    SinglyLinkedList(const SinglyLinkedList& other) {
        Node* current = other.head_.get();
        while (current) {
            push_back(current->data);
            current = current->next.get();
        }
    }
    
    // Конструктор перемещения
    SinglyLinkedList(SinglyLinkedList&& other) noexcept
        : head_(std::move(other.head_)), tail_(other.tail_), size_(other.size_) {
        other.tail_ = nullptr;
        other.size_ = 0;
    }
    
    // Оператор присваивания копированием
    SinglyLinkedList& operator=(const SinglyLinkedList& other) {
        if (this != &other) {
            SinglyLinkedList temp(other);
            swap(temp);
        }
        return *this;
    }
    
    // Оператор присваивания перемещением
    SinglyLinkedList& operator=(SinglyLinkedList&& other) noexcept {
        if (this != &other) {
            clear();
            head_ = std::move(other.head_);
            tail_ = other.tail_;
            size_ = other.size_;
            
            other.tail_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }
    
    ~SinglyLinkedList() = default;
    
    // Реализация методов BaseContainer
    size_type size() const noexcept override { return size_; }
    bool empty() const noexcept override { return size_ == 0; }
    
    void clear() override {
        while (head_) {
            head_ = std::move(head_->next);
        }
        tail_ = nullptr;
        size_ = 0;
    }
    
    void push_back(const T& value) override {
        auto new_node = std::make_unique<Node>(value);
        Node* raw_ptr = new_node.get();
        
        if (!head_) {
            head_ = std::move(new_node);
            tail_ = raw_ptr;
        } else {
            tail_->next = std::move(new_node);
            tail_ = raw_ptr;
        }
        ++size_;
    }
    
    void push_back(T&& value) override {
        auto new_node = std::make_unique<Node>(std::move(value));
        Node* raw_ptr = new_node.get();
        
        if (!head_) {
            head_ = std::move(new_node);
            tail_ = raw_ptr;
        } else {
            tail_->next = std::move(new_node);
            tail_ = raw_ptr;
        }
        ++size_;
    }
    
    void insert(size_type pos, const T& value) override {
        this->check_position(pos, size_);
        
        if (pos == 0) {
            push_front(value);
        } else if (pos == size_) {
            push_back(value);
        } else {
            insert_middle(pos, value);
        }
    }
    
    void insert(size_type pos, T&& value) override {
        this->check_position(pos, size_);
        
        if (pos == 0) {
            push_front(std::move(value));
        } else if (pos == size_) {
            push_back(std::move(value));
        } else {
            insert_middle(pos, std::move(value));
        }
    }
    
    void erase(size_type pos) override {
        this->check_index(pos, size_);
        
        if (pos == 0) {
            head_ = std::move(head_->next);
            if (!head_) tail_ = nullptr;
        } else {
            Node* prev = head_.get();
            for (size_type i = 0; i < pos - 1; ++i) {
                prev = prev->next.get();
            }
            
            auto node_to_erase = std::move(prev->next);
            prev->next = std::move(node_to_erase->next);
            
            if (!prev->next) {
                tail_ = prev;
            }
        }
        --size_;
    }
    
    reference operator[](size_type idx) override {
        this->check_index(idx, size_);
        return get_node_at(idx)->data;
    }
    
    const_reference operator[](size_type idx) const override {
        this->check_index(idx, size_);
        return get_node_at(idx)->data;
    }
    
    void print(std::ostream& os = std::cout) const override {
        Node* current = head_.get();
        while (current) {
            os << current->data;
            current = current->next.get();
            if (current) os << " ";
        }
    }
    
    // Дополнительные методы
    void push_front(const T& value) {
        auto new_node = std::make_unique<Node>(value, std::move(head_));
        head_ = std::move(new_node);
        if (!tail_) tail_ = head_.get();
        ++size_;
    }
    
    void push_front(T&& value) {
        auto new_node = std::make_unique<Node>(std::move(value), std::move(head_));
        head_ = std::move(new_node);
        if (!tail_) tail_ = head_.get();
        ++size_;
    }
    
    // Итераторы
    iterator begin() noexcept { return iterator(head_.get()); }
    iterator end() noexcept { return iterator(nullptr); }
    
    const_iterator begin() const noexcept { return const_iterator(head_.get()); }
    const_iterator end() const noexcept { return const_iterator(nullptr); }
    
    const_iterator cbegin() const noexcept { return const_iterator(head_.get()); }
    const_iterator cend() const noexcept { return const_iterator(nullptr); }
    
    void swap(SinglyLinkedList& other) noexcept {
        using std::swap;
        swap(head_, other.head_);
        swap(tail_, other.tail_);
        swap(size_, other.size_);
    }
    
private:
    std::unique_ptr<Node> head_;
    Node* tail_ = nullptr;
    size_type size_ = 0;
    
    Node* get_node_at(size_type idx) const {
        Node* current = head_.get();
        for (size_type i = 0; i < idx; ++i) {
            current = current->next.get();
        }
        return current;
    }
    
    void insert_middle(size_type pos, const T& value) {
        Node* prev = head_.get();
        for (size_type i = 0; i < pos - 1; ++i) {
            prev = prev->next.get();
        }
        
        auto new_node = std::make_unique<Node>(value, std::move(prev->next));
        prev->next = std::move(new_node);
        ++size_;
    }
    
    void insert_middle(size_type pos, T&& value) {
        Node* prev = head_.get();
        for (size_type i = 0; i < pos - 1; ++i) {
            prev = prev->next.get();
        }
        
        auto new_node = std::make_unique<Node>(std::move(value), std::move(prev->next));
        prev->next = std::move(new_node);
        ++size_;
    }
};

#endif // SINGLY_LINKED_LIST_H