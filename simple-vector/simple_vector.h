#pragma once
#include "array_ptr.h"
#include <cassert>
#include <initializer_list>
#include <stdexcept>
#include <algorithm>
class ReserveProxyObj {
public:
    
    ReserveProxyObj(size_t obj) 
        : value_(obj) {}
    
    size_t GetValue() {
        return value_;
    }
private:
    size_t value_;
};

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;
 
    SimpleVector(ReserveProxyObj obj) {
        Reserve(obj.GetValue());
    }
 
    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size)
            : size_(size), capacity_(size), array_(size)
    {
        std::fill_n(array_.Get(), size_, Type{});
    }
 
    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value)
            : size_(size), capacity_(size), array_(size)
    {
        std::fill_n(array_.Get(), size_, value);
    }
 
    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init)
            : size_(init.size()), capacity_(init.size()), array_(init.size())
    {
        std::copy(init.begin(), init.end(), array_.Get());
    }
    
    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        // Напишите тело самостоятельно
        return size_;
    }

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
        // Напишите тело самостоятельно
        return capacity_;
    }

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        return size_==0;
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        return array_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        return array_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("index >= size !");
        }
        return array_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("index >= size !");
        }
        return array_[index];
    }
    
    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        size_ = 0;
        array_.Release();
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        if (new_size > capacity_) {
            //создайте новый массив с нужной вместимостью, скопируйте в него прежнее содержимое и заполните остальные элементы значением по умолчанию. Затем старый массив можно удалить и использовать копию. После этого не забудьте обновить размер и вместимость вектора.
            ArrayPtr<Type> tmp(new_size);
            std::move(array_.Get(), array_.Get()+size_, tmp.Get());
            //std::fill(tmp.Get()+size_, tmp.Get()+new_size, Type{});
            for (auto a = tmp.Get()+size_; a < tmp.Get()+new_size; a++){
                *a = Type{};
            }
            size_ = new_size;
            capacity_ = new_size;
            array_ = (std::move(tmp));
        }
        else {
            if (new_size > size_) {
                //заполните добавленные элементы значением по умолчанию для типа Type.
                //std::fill(array_.Get() + size_, array_.Get()+new_size, Type{});
                for (auto a = array_.Get() + size_; a <= array_.Get()+new_size; a++){
                    *a = Type{};
                }
                size_ = new_size;
            }
            else {size_ = new_size;}
        }
    }
    

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return array_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        return array_.Get() + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        return array_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        return array_.Get() + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        return array_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        return array_.Get() + size_;
    }
    
    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    
    void PushBack(Type&& item) {
    if (size_==0) {
        ArrayPtr<Type> a(static_cast<size_t>(1));
        array_.swap(a);
    }
        if (size_ < capacity_) {
            array_[size_] = std::move(item);
            size_++;
        }
        else if (size_ == capacity_){
            auto new_size = (size_ == 0 ? 1 : 2*size_);
            ArrayPtr<Type> tmp(new_size);
            std::move(array_.Get(), array_.Get()+size_, tmp.Get());
            tmp[size_] = std::move(item);
            size_++;
            capacity_ = new_size;
            array_.swap(tmp);
        }
    }
    
    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        if (!IsEmpty()) {
            size_--;
        }
    }
    
    // Обменивает значение с другим вектором
    /*void swap(SimpleVector&& other) noexcept {
        array_.swap(other.array_);
        std::swap(other.size_, size_);
        std::swap(other.capacity_, capacity_);
    }*/
    void swap(SimpleVector& other) noexcept {
        array_.swap(other.array_);
        std::swap(other.size_, size_);
        std::swap(other.capacity_, capacity_);
    }    
    
    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {
        std::move(const_cast<Iterator>(pos+1), array_.Get()+size_, const_cast<Iterator>(pos));
        size_--;
        return const_cast<Iterator>(pos);
    }
    
      Iterator Insert(ConstIterator pos, Type&& value) {
        if (size_ == 0) {
            ArrayPtr<Type> a(static_cast<size_t>(1));
            array_.swap(a);
            *begin() = std::move(value);
            size_ = 1;
            if (capacity_ == 0) { 
                capacity_ = 1; 
            }
            return Iterator{begin()};
    } else {
        Iterator c = const_cast<Iterator>(pos);
        if(size_ < capacity_) {
            std::move_backward(c, array_.Get() + size_, array_.Get() + size_ + 1);
            *const_cast<Iterator>(pos) = std::move(value);
            ++size_;
            return const_cast<Iterator>(pos);
        }
        size_t cc = capacity_ ? 2*capacity_: 2;
        auto delta = std::distance(array_.Get(), const_cast<Iterator>(pos));
        ArrayPtr<Type> tmp{cc};
        if (0 == delta) {
            std::move(begin(), end(), tmp.Get() + 1);
        } else {
            std::move(begin(), c, tmp.Get());
            std::move(c, end(), tmp.Get() + delta + 1);
        }
        *(tmp.Get() + delta) = std::move(value);
        array_.swap(tmp);
        capacity_ = cc;
        ++size_;
        return Iterator{begin() + delta};}
    }
    
    void Reserve(size_t new_capacity) {
        if (new_capacity >= capacity_) {
            ArrayPtr<Type> new_items(new_capacity);
            std::move(begin(), end(), new_items.Get());
            array_.swap(new_items);
            capacity_ = new_capacity;
        }
    }
    
    SimpleVector& operator=(const SimpleVector& rhs) {
        if (this != &rhs) {
            auto rhs_copy(std::move(rhs));
            swap(rhs_copy);
        }
        return *this;
    }
    
    SimpleVector(const SimpleVector& other) {
        ArrayPtr<Type> tmp{other.capacity_};
        std::copy(other.array_.Get(), other.array_.Get() + other.size_, tmp.Get());
        array_.swap(tmp);
        capacity_ = other.capacity_;
        size_ = other.size_;
    }
    
    SimpleVector& operator=(SimpleVector&& rhs) {
        if (this != &rhs) {
            SimpleVector<Type> rhs_copy(std::move(rhs));
            swap(rhs_copy);
        }
        return *this;
    }
    
    SimpleVector(SimpleVector&& other) {
        array_ = std::move(other.array_);
        size_ = std::exchange(other.size_, size_);
        capacity_ = std::exchange(other.capacity_, capacity_);
    }

private:
    size_t size_ = 0;
    size_t capacity_ = 0;
    ArrayPtr<Type> array_;
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !operator==(lhs, rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return rhs<lhs;
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs<lhs);
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs>lhs);
} 

ReserveProxyObj Reserve (size_t obj) {
    ReserveProxyObj a(obj);
    return a; 
}
