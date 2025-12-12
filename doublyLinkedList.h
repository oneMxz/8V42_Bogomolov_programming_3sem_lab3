#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H

#include "BaseContainer.h"
#include <memory>
#include <utility>
#include <stdexcept>
#include <initializer_list>
#include <iterator>

template<typename T>
class DoublyLinkedList : public BaseContainer<T> {
private:
    struct Node {
        T data;
        std::unique_ptr<Node> next;
        Node* prev;
        
        Node(const T& v, Node* p = nullptr) : data(v), next(nullptr), prev(p) {}
        Node(T&& v, Node* p = nullptr) : data(std::move(v)), next(nullptr), prev(p) {}
    };
    
public:
    using value_type = typename BaseContainer<T>::value_type;
    using size_type = typename BaseContainer<T>::size_type;
    using reference = typename BaseContainer<T>::reference;
    using const_reference = typename BaseContainer<T>::const_reference;
    
    // Bidirectional Iterator
    class Iterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
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
        
        Iterator& operator--() {
            node_ = node_ ? node_->prev : nullptr;
            return *this;
        }
        
        Iterator operator--(int) {
            Iterator temp = *this;
            node_ = node_ ? node_->prev : nullptr;
            return temp;
        }
        
        bool operator==(const Iterator& other) const { return node_ == other.node_; }
        bool operator!=(const Iterator& other) const { return node_ != other.node_; }
        
    private:
        Node* node_;
    };
    
    class ConstIterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
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
        
        ConstIterator& operator--() {
            node_ = node_ ? node_->prev : nullptr;
            return *this;
        }
        
        ConstIterator operator--(int) {
            ConstIterator temp = *this;
            node_ = node_ ? node_->prev : nullptr;
            return temp;
        }
        
        bool operator==(const ConstIterator& other) const { return node_ == other.node_; }
        bool operator!=(const ConstIterator& other) const { return node_ != other.node_; }
        
    private:
        Node* node_;
    };
    
    using iterator = Iterator;
    using const_iterator = ConstIterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    
    DoublyLinkedList() = default;
    
    DoublyLinkedList(std::initializer_list<T> init) {
        for (const auto& item : init) {
            push_back(item);
        }
    }
    
    // Конструктор копирования
    DoublyLinkedList(const DoublyLinkedList& other) {
        Node* current = other.head_.get();
        while (current) {
            push_back(current->data);
            current = current->next.get();
        }
    }
    
    // Конструктор перемещения
    DoublyLinkedList(DoublyLinkedList&& other) noexcept
        : head_(std::move(other.head_)), tail_(other.tail_), size_(other.size_) {
        if (head_) {
            head_->prev = nullptr;
        }
        other.tail_ = nullptr;
        other.size_ = 0;
    }
    
    // Оператор присваивания копированием
    DoublyLinkedList& operator=(const DoublyLinkedList& other) {
        if (this != &other) {
            DoublyLinkedList temp(other);
            swap(temp);
        }
        return *this;
    }
    
    // Оператор присваивания перемещением
    DoublyLinkedList& operator=(DoublyLinkedList&& other) noexcept {
        if (this != &other) {
            clear();
            head_ = std::move(other.head_);
            tail_ = other.tail_;
            size_ = other.size_;
            
            if (head_) {
                head_->prev = nullptr;
            }
            other.tail_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }
    
    ~DoublyLinkedList() = default;
    
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
        auto new_node = std::make_unique<Node>(value, tail_);
        Node* raw_ptr = new_node.get();
        
        if (!head_) {
            head_ = std::move(new_node);
        } else {
            tail_->next = std::move(new_node);
        }
        tail_ = raw_ptr;
        ++size_;
    }
    
    void push_back(T&& value) override {
        auto new_node = std::make_unique<Node>(std::move(value), tail_);
        Node* raw_ptr = new_node.get();
        
        if (!head_) {
            head_ = std::move(new_node);
        } else {
            tail_->next = std::move(new_node);
        }
        tail_ = raw_ptr;
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
            erase_front();
        } else if (pos == size_ - 1) {
            erase_back();
        } else {
            erase_middle(pos);
        }
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
        auto new_node = std::make_unique<Node>(value, nullptr);
        if (head_) {
            new_node->next = std::move(head_);
            new_node->next->prev = new_node.get();
        } else {
            tail_ = new_node.get();
        }
        head_ = std::move(new_node);
        ++size_;
    }
    
    void push_front(T&& value) {
        auto new_node = std::make_unique<Node>(std::move(value), nullptr);
        if (head_) {
            new_node->next = std::move(head_);
            new_node->next->prev = new_node.get();
        } else {
            tail_ = new_node.get();
        }
        head_ = std::move(new_node);
        ++size_;
    }
    
    // Итераторы
    iterator begin() noexcept { return iterator(head_.get()); }
    iterator end() noexcept { return iterator(nullptr); }
    
    const_iterator begin() const noexcept { return const_iterator(head_.get()); }
    const_iterator end() const noexcept { return const_iterator(nullptr); }
    
    const_iterator cbegin() const noexcept { return const_iterator(head_.get()); }
    const_iterator cend() const noexcept { return const_iterator(nullptr); }
    
    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    
    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
    
    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }
    const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }
    
    void swap(DoublyLinkedList& other) noexcept {
        using std::swap;
        swap(head_, other.head_);
        swap(tail_, other.tail_);
        swap(size_, other.size_);
        
        // Обновляем указатели prev
        if (head_) head_->prev = nullptr;
        if (other.head_) other.head_->prev = nullptr;
    }
    
private:
    std::unique_ptr<Node> head_;
    Node* tail_ = nullptr;
    size_type size_ = 0;
    
    Node* get_node_at(size_type idx) const {
        if (idx < size_ / 2) {
            Node* current = head_.get();
            for (size_type i = 0; i < idx; ++i) {
                current = current->next.get();
            }
            return current;
        } else {
            Node* current = tail_;
            for (size_type i = size_ - 1; i > idx; --i) {
                current = current->prev;
            }
            return current;
        }
    }
    
    void insert_middle(size_type pos, const T& value) {
        Node* current = get_node_at(pos);
        auto new_node = std::make_unique<Node>(value, current->prev);
        
        new_node->next = std::move(current->prev->next);
        current->prev = new_node.get();
        new_node->next->prev = new_node.get();
        new_node->prev->next = std::move(new_node);
        
        ++size_;
    }
    
    void insert_middle(size_type pos, T&& value) {
        Node* current = get_node_at(pos);
        auto new_node = std::make_unique<Node>(std::move(value), current->prev);
        
        new_node->next = std::move(current->prev->next);
        current->prev = new_node.get();
        new_node->next->prev = new_node.get();
        new_node->prev->next = std::move(new_node);
        
        ++size_;
    }
    
    void erase_front() {
        head_ = std::move(head_->next);
        if (head_) {
            head_->prev = nullptr;
        } else {
            tail_ = nullptr;
        }
        --size_;
    }
    
    void erase_back() {
        if (tail_->prev) {
            tail_ = tail_->prev;
            tail_->next.reset();
        } else {
            head_.reset();
            tail_ = nullptr;
        }
        --size_;
    }
    
    void erase_middle(size_type pos) {
        Node* node_to_erase = get_node_at(pos);
        node_to_erase->next->prev = node_to_erase->prev;
        node_to_erase->prev->next = std::move(node_to_erase->next);
        --size_;
    }
};

#endif // DOUBLY_LINKED_LIST_H