//
// Created by dado on 27/06/2020.
//

#pragma once

#include <vector>
#include <memory>

template <typename T>
class Memento {
public:
    void push(std::shared_ptr<T> _value) {
        if ( _value ) {
            if ( stackPointer != static_cast<int>( mementos.size() - 1 ) ) {
                mementos.erase(mementos.begin() + stackPointer+1, mementos.end());
            }
            if ( mementos.empty() ) {
                current = _value;
            }
            mementos.emplace_back(_value);
            stackPointer = mementos.size()-1;
        }
    }

    std::shared_ptr<T> undo() {
        stackPointer = max( -1, --stackPointer);
        current = stackPointer >= 0 ? mementos[stackPointer] : nullptr;
        return current;
    }

    std::shared_ptr<T> redo() {
        if ( stackPointer < static_cast<int>( mementos.size() - 1 ) ) {
            current = mementos[++stackPointer];
        }
        return current;
    }

    std::shared_ptr<T> operator()() {
        return current;
    }

    void reset( std::shared_ptr<T> _value ) {
        mementos.clear();
        stackPointer = -1;
        push(_value);
        current = _value;
    }

private:
    std::vector<std::shared_ptr<T>> mementos{};
    std::shared_ptr<T> current = nullptr;
    int stackPointer = -1;
};

