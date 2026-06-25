#pragma once

#include <cassert>
#include <initializer_list>
#include <algorithm>
#include <stdexcept>
#include "array_ptr.h"
#include <utility>
struct ReserveProxyObj {
    size_t capacity_;
};

inline ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return { capacity_to_reserve };
}

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    explicit SimpleVector(size_t size) 
        : data_(new Type[size]{}),
        size_(size),
        capacity_(size) {}

    SimpleVector(size_t size, const Type& value) 
        : data_(new Type[size]{}),
        size_(size),
        capacity_(size) {
        for (size_t i = 0; i < size; ++i) {
            data_[i] = value;
        }
    }

    SimpleVector(std::initializer_list<Type> init) 
        : data_(new Type[init.size()]{}),
        size_(init.size()),
        capacity_(init.size()) {
        std::copy(init.begin(), init.end(), data_.Get());
    }

    SimpleVector(const SimpleVector& other) 
        : data_(other.size_ ? new Type[other.size_] : nullptr),
        size_(other.size_),
        capacity_(other.size_) {
        if (other.size_) {
            std::copy(other.begin(), other.end(), data_.Get());
        }
    }

    SimpleVector(SimpleVector&& other) noexcept
        : data_(std::move(other.data_)),
        size_(other.size_),
        capacity_(other.capacity_) {
        other.size_ = 0;
        other.capacity_ = 0;
    }

    explicit SimpleVector(ReserveProxyObj proxy) 
        : data_(new Type[proxy.capacity_]{}),
        size_(0),
        capacity_(proxy.capacity_) {}

    SimpleVector& operator=(SimpleVector&& other) noexcept {
        if (this != &other) {
            swap(other);
        }
        return *this;
    }

    ~SimpleVector() = default;

    SimpleVector& operator=(const SimpleVector& rhs) {
        if (this != &rhs) {
            SimpleVector tmp(rhs);
            swap(tmp);
        }
        return *this;
    }

    void PushBack(const Type& item) {
        Insert(end(), item);
    }

    void PushBack(Type&& item) {
        Insert(end(), std::move(item));
    }

    Iterator Insert(ConstIterator pos, Type&& value) {
        assert(pos >= begin() && pos <= end());
        size_t idx = pos - cbegin();
        if (size_ == capacity_) {
            size_t new_capacity = (capacity_ == 0) ? 1 : capacity_ * 2;
            Reserve(new_capacity);
        }
        std::move_backward(begin() + idx, end(), end() + 1);
        data_[idx] = std::move(value);
        ++size_;
        return begin() + idx;
    }

    Iterator Insert(ConstIterator pos, const Type& value) {
        assert(pos >= begin() && pos <= end());
        size_t idx = pos - cbegin();
        if (size_ == capacity_) {
            size_t new_capacity = (capacity_ == 0) ? 1 : capacity_ * 2;
            Reserve(new_capacity);
        }
        std::move_backward(begin() + idx, end(), end() + 1);
        data_[idx] = value;
        ++size_;
        return begin() + idx;
    }

    void PopBack() noexcept {
        assert(size_ > 0);
        --size_;
    }

    Iterator Erase(ConstIterator pos) {
        assert(size_ > 0 && pos >= begin() && pos < end());
        size_t idx = pos - cbegin();
        std::move(begin() + idx + 1, end(), begin() + idx);
        --size_;
        return begin() + idx;
    }

    void swap(SimpleVector& other) noexcept {
        data_.Swap(other.data_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    size_t GetSize() const noexcept {
        return size_;
    }

    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    Type& operator[](size_t index) noexcept {
        assert(index < size_);
        return data_[index];
    }

    const Type& operator[](size_t index) const noexcept {
        assert(index < size_);
        return data_[index];
    }

    Type& At(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("SimpleVector::At: index out of range");
        }
        return data_[index];
    }

    const Type& At(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("SimpleVector::At: index out of range");
        }
        return data_[index];
    }

    void Clear() noexcept {
        size_ = 0;
    }

    void Resize(size_t new_size) {
        if (new_size == size_) {
            return;
        }
        if (new_size < size_) {
            size_ = new_size;
            return;
        }
        if (new_size <= capacity_) {
            for (size_t i = size_; i < new_size; ++i) {
                data_[i] = std::move(Type{});
            }
            size_ = new_size;
        }
        else {
            size_t new_capacity = std::max(capacity_ * 2, new_size);
            Reserve(new_capacity);
            size_ = new_size;
        }
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            ArrayPtr<Type> new_data(new Type[new_capacity]{});
            std::move(begin(), end(), new_data.Get());
            data_.Swap(new_data);
            capacity_ = new_capacity;
        }
    }

    Iterator begin() noexcept {
        return data_.Get();
    }

    Iterator end() noexcept {
        return data_.Get() + size_;
    }

    ConstIterator begin() const noexcept {
        return data_.Get();
    }

    ConstIterator end() const noexcept {
        return data_.Get() + size_;
    }

    ConstIterator cbegin() const noexcept {
        return data_.Get();
    }

    ConstIterator cend() const noexcept {
        return data_.Get() + size_;
    }

private:
    ArrayPtr<Type> data_;
    size_t size_ = 0;
    size_t capacity_ = 0;
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    if (&lhs == &rhs) {
        return true;
    }
    if (lhs.GetSize() != rhs.GetSize()) {
        return false;
    }
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);
}