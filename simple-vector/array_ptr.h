#pragma once
#include <utility>
#include <cstddef>

template <typename Type>
class ArrayPtr {
    public:
    ArrayPtr() noexcept : ptr_(nullptr) {}

    explicit ArrayPtr(Type* ptr) noexcept : ptr_(ptr) {}

    ArrayPtr(const ArrayPtr&) = delete;
    ArrayPtr& operator=(const ArrayPtr&) = delete;

    ArrayPtr(ArrayPtr&& other) noexcept : ptr_(other.ptr_) {
        other.ptr_ = nullptr;
    }

    ArrayPtr& operator=(ArrayPtr&& other) noexcept {
        if (this != &other) {
            delete[] ptr_;
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        }
        return *this;
    }
    
    ~ArrayPtr() {
        delete[] ptr_;
    }

    Type* Get() const noexcept {
        return ptr_;
    }

    Type* Release() noexcept {
        Type* tmp = ptr_;
        ptr_ = nullptr;
        return tmp;
    }

    void Reset(Type* ptr = nullptr) noexcept {
        if (ptr_ != ptr) {
            delete[] ptr_;
            ptr_ = ptr;
        }
    }

    void Swap(ArrayPtr& other) noexcept {
        std::swap(ptr_,other.ptr_);
    }

    Type& operator*() const noexcept {
        return *ptr_;
    }

    Type& operator[](size_t index) const noexcept {
        return ptr_[index];
    }

    Type* operator+(size_t shift) const noexcept {
        return ptr_ + shift;
    }

    Type* operator-(size_t shift) const noexcept {
        return ptr_ - shift;
    }

    bool operator==(const ArrayPtr& other) const noexcept {
        return ptr_ == other.ptr_;
    }

    bool operator!=(const ArrayPtr& other) const noexcept {
        return !(ptr_==other.ptr_);
    }

    private:
    Type* ptr_;
};