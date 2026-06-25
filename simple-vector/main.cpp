#include "simple_vector.h"

#include <cassert>
#include <iostream>
#include <numeric>

using namespace std;

class X {
public:
    X()
        : X(5) {
    }
    X(size_t num)
        : x_(num) {
    }
    X(const X& other) = delete;
    X& operator=(const X& other) = delete;
    X(X&& other) {
        x_ = exchange(other.x_, 0);
    }
    X& operator=(X&& other) {
        x_ = exchange(other.x_, 0);
        return *this;
    }
    size_t GetX() const {
        return x_;
    }

private:
    size_t x_;
};

SimpleVector<int> GenerateVector(size_t size) {
    SimpleVector<int> v(size);
    iota(v.begin(), v.end(), 1);
    return v;
}

void TestTemporaryObjConstructor() {
    const size_t size = 1000000;
    cout << "Test with temporary object, copy elision" << endl;
    SimpleVector<int> moved_vector(GenerateVector(size));
    assert(moved_vector.GetSize() == size);
    cout << "Done!" << endl << endl;
}

void TestTemporaryObjOperator() {
    const size_t size = 1000000;
    cout << "Test with temporary object, operator=" << endl;
    SimpleVector<int> moved_vector;
    assert(moved_vector.GetSize() == 0);
    moved_vector = GenerateVector(size);
    assert(moved_vector.GetSize() == size);
    cout << "Done!" << endl << endl;
}

void TestNamedMoveConstructor() {
    const size_t size = 1000000;
    cout << "Test with named object, move constructor" << endl;
    SimpleVector<int> vector_to_move(GenerateVector(size));
    assert(vector_to_move.GetSize() == size);

    SimpleVector<int> moved_vector(move(vector_to_move));
    assert(moved_vector.GetSize() == size);
    assert(vector_to_move.GetSize() == 0);
    cout << "Done!" << endl << endl;
}

void TestNamedMoveOperator() {
    const size_t size = 1000000;
    cout << "Test with named object, operator=" << endl;
    SimpleVector<int> vector_to_move(GenerateVector(size));
    assert(vector_to_move.GetSize() == size);

    SimpleVector<int> moved_vector = move(vector_to_move);
    assert(moved_vector.GetSize() == size);
    assert(vector_to_move.GetSize() == 0);
    cout << "Done!" << endl << endl;
}

void TestNoncopiableMoveConstructor() {
    const size_t size = 5;
    cout << "Test noncopiable object, move constructor" << endl;
    SimpleVector<X> vector_to_move;
    for (size_t i = 0; i < size; ++i) {
        vector_to_move.PushBack(X(i));
    }

    SimpleVector<X> moved_vector = move(vector_to_move);
    assert(moved_vector.GetSize() == size);
    assert(vector_to_move.GetSize() == 0);

    for (size_t i = 0; i < size; ++i) {
        assert(moved_vector[i].GetX() == i);
    }
    cout << "Done!" << endl << endl;
}

void TestNoncopiablePushBack() {
    const size_t size = 5;
    cout << "Test noncopiable push back" << endl;
    SimpleVector<X> v;
    for (size_t i = 0; i < size; ++i) {
        v.PushBack(X(i));
    }

    assert(v.GetSize() == size);

    for (size_t i = 0; i < size; ++i) {
        assert(v[i].GetX() == i);
    }
    cout << "Done!" << endl << endl;
}

void TestNoncopiableInsert() {
    const size_t size = 5;
    cout << "Test noncopiable insert" << endl;
    SimpleVector<X> v;
    for (size_t i = 0; i < size; ++i) {
        v.PushBack(X(i));
    }

    // в начало
    v.Insert(v.begin(), X(size + 1));
    assert(v.GetSize() == size + 1);
    assert(v.begin()->GetX() == size + 1);
    // в конец
    v.Insert(v.end(), X(size + 2));
    assert(v.GetSize() == size + 2);
    assert((v.end() - 1)->GetX() == size + 2);
    // в середину
    v.Insert(v.begin() + 3, X(size + 3));
    assert(v.GetSize() == size + 3);
    assert((v.begin() + 3)->GetX() == size + 3);
    cout << "Done!" << endl << endl;
}

void TestNoncopiableErase() {
    const size_t size = 3;
    cout << "Test noncopiable erase" << endl;
    SimpleVector<X> v;
    for (size_t i = 0; i < size; ++i) {
        v.PushBack(X(i));
    }

    auto it = v.Erase(v.begin());
    assert(it->GetX() == 1);
    cout << "Done!" << endl << endl;
}

void TestMoveAfterReserve() {
    const size_t size = 5;
    cout << "Test move after Reserve()" << endl;
    SimpleVector<X> v;
    v.Reserve(100);
    for (size_t i = 0; i < size; ++i) {
        v.PushBack(X(i));
    }
    assert(v.GetCapacity() >= 100);
    assert(v.GetSize() == size);

    SimpleVector<X> moved = std::move(v);
    assert(moved.GetSize() == size);
    assert(moved.GetCapacity() >= 100);
    assert(v.GetSize() == 0);
    assert(v.GetCapacity() == 0);

    for (size_t i = 0; i < size; ++i) {
        assert(moved[i].GetX() == i);
    }
    cout << "Done!" << endl << endl;
}

void TestMoveAfterResize() {
    const size_t initial = 3;
    const size_t new_size = 7;
    cout << "Test move after Resize()" << endl;
    SimpleVector<X> v(initial);
    for (size_t i = 0; i < initial; ++i) {
        v[i] = X(i);
    }
    v.Resize(new_size);
    assert(v.GetSize() == new_size);

    SimpleVector<X> moved = std::move(v);
    assert(moved.GetSize() == new_size);
    assert(v.GetSize() == 0);
    assert(v.GetCapacity() == 0);

    for (size_t i = 0; i < initial; ++i) {
        assert(moved[i].GetX() == i);
    }
    for(size_t i = initial; i< new_size; ++i) {
        assert(moved[i].GetX() == 5);
    }
    cout << "Done!" << endl << endl;
}

void TestMoveEmptyVector() {
    cout << "Test move of empty vector" << endl;
    SimpleVector<X> v;
    assert(v.GetSize() == 0);
    assert(v.GetCapacity() == 0);

    SimpleVector<X> moved = std::move(v);
    assert(moved.GetSize() == 0);
    assert(moved.GetCapacity() == 0);
    assert(v.GetSize() == 0);
    assert(v.GetCapacity() == 0);

    moved.PushBack(X(42));
    assert(moved.GetSize() == 1);
    assert(moved[0].GetX() == 42);
    cout << "Done!" << endl << endl;
}

int main() {
    TestTemporaryObjConstructor();
    TestTemporaryObjOperator();
    TestNamedMoveConstructor();
    TestNamedMoveOperator();
    TestNoncopiableMoveConstructor();
    TestNoncopiablePushBack();
    TestNoncopiableInsert();
    TestNoncopiableErase();

    TestMoveAfterReserve();
    TestMoveAfterResize();
    TestMoveEmptyVector();
    return 0;
}