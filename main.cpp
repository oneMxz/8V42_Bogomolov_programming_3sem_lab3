#include <iostream>
#include <algorithm>
#include <vector>
#include "baseContainer.h"
#include "simpleVector.h"
#include "singlyLinkedList.h"
#include "doublyLinkedList.h"

// Функция для демонстрации всех операций из задания
template <typename Container>
void runDemo(const std::string& title) {
    std::cout << "\n=== " << title << " ===" << std::endl;
    
    Container c;
    
    // 1. Добавление 10 элементов (0..9)
    for (int i = 0; i < 10; ++i) {
        c.push_back(i);
    }
    
    // 2. Вывод содержимого
    std::cout << "Start: ";
    c.print();
    std::cout << std::endl;
    
    // 3. Вывод размера
    std::cout << "Size: " << c.size() << std::endl;
    
    // 4. Удаление 3-го, 5-го и 7-го элементов (индексы 2, 4, 6)
    // Важно: после каждого удаления индексы сдвигаются!
    c.erase(2);  // Удаляем третий элемент (значение 2)
    c.erase(4);  // Теперь удаляем пятый элемент (значение 4)
    c.erase(6);  // Теперь удаляем седьмой элемент (значение 6)
    
    std::cout << "After erase: ";
    c.print();
    std::cout << std::endl;
    
    // 5. Добавление элемента 10 в начало
    c.insert(0, 10);
    std::cout << "After push_front(10): ";
    c.print();
    std::cout << std::endl;
    
    // 6. Добавление элемента 20 в середину
    size_t mid = c.size() / 2;
    c.insert(mid, 20);
    std::cout << "After insert middle(20): ";
    c.print();
    std::cout << std::endl;
    
    // 7. Добавление элемента 30 в конец
    c.push_back(30);
    std::cout << "After push_back(30): ";
    c.print();
    std::cout << std::endl;
}

// Тестирование конструкторов и семантики перемещения
void testConstructors() {
    std::cout << "\n=== Тестирование конструкторов ===" << std::endl;
    
    // Конструктор от initializer_list
    SimpleVector<int> vec1 = {1, 2, 3, 4, 5};
    SinglyLinkedList<int> sll1 = {6, 7, 8, 9, 10};
    DoublyLinkedList<int> dll1 = {11, 12, 13, 14, 15};
    
    std::cout << "Конструктор от initializer_list:" << std::endl;
    std::cout << "vec1: "; vec1.print(); std::cout << std::endl;
    std::cout << "sll1: "; sll1.print(); std::cout << std::endl;
    std::cout << "dll1: "; dll1.print(); std::cout << std::endl;
    
    // Конструктор копирования
    SimpleVector<int> vec2 = vec1;
    std::cout << "\nКонструктор копирования SimpleVector:" << std::endl;
    std::cout << "vec2: "; vec2.print(); std::cout << std::endl;
    
    // Конструктор перемещения
    SimpleVector<int> vec3 = std::move(vec2);
    std::cout << "После перемещения:" << std::endl;
    std::cout << "vec3: "; vec3.print(); std::cout << std::endl;
    std::cout << "vec2 (пустой): "; vec2.print(); std::cout << std::endl;
}

int main() {
    runDemo<SimpleVector<int>>("SimpleVector");
    runDemo<SinglyLinkedList<int>>("SinglyLinkedList");
    runDemo<DoublyLinkedList<int>>("DoublyLinkedList");
    testConstructors();
    std::cout << "\nProgram executed successfully" << std::endl;
    return 0;
}