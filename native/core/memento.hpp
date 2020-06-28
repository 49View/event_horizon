//
// Created by dado on 27/06/2020.
//

#pragma once

#include <vector>
#include <memory>

template <typename T>
class Memento {
public:
    Memento() {
        mementos.emplace_back(nullptr);
    }

    void push() {
        push( current );
    }

    void push(std::shared_ptr<T> _value) {
        if ( _value ) {
            mementos.erase(mementos.begin() + stackPointer + 1, mementos.end());
            if ( mementos.size() == 1 ) {
                current = std::make_shared<T>(_value->serialize());
            }
            mementos.emplace_back(std::make_shared<T>(_value->serialize()));
            stackPointer = mementos.size()-1;
        }
    }

    void undo() {
        if ( stackPointer > 0 ) {
            auto ptr = mementos[--stackPointer];
            current =  ptr ? std::make_shared<T>(ptr->serialize()) : nullptr;
        }
    }

    void redo() {
        if ( stackPointer+1 != static_cast<int>( mementos.size() ) ) {
            current = std::make_shared<T>(mementos[++stackPointer]->serialize());
        }
    }

    std::shared_ptr<T> operator()() {
        return current;
    }

    std::shared_ptr<T> bufferAt(size_t index) {
        return index < mementos.size() ? mementos[index] : nullptr;
    }

    void reset( std::shared_ptr<T> _value ) {
        stackPointer = 0;
        push(_value);
    }

private:
    std::vector<std::shared_ptr<T>> mementos{};
    std::shared_ptr<T> current = nullptr;
    int stackPointer = 0;
};

