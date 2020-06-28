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
            mementos.emplace_back(_value);
            stackPointer = mementos.size()-1;
        }
    }

    std::shared_ptr<T> undo() {
        stackPointer = max( -1, --stackPointer);
        if ( stackPointer >= 0 ) {
            return mementos[stackPointer];
        }
        return nullptr;
    }

    std::shared_ptr<T> redo() {
        if ( stackPointer == static_cast<int>( mementos.size() - 1 ) ) {
            return mementos[stackPointer];
        }
        if ( stackPointer < static_cast<int>( mementos.size() - 1 ) ) {
            return mementos[++stackPointer];
        }
        return nullptr;
    }

    std::shared_ptr<T> operator()() {
        return stackPointer < 0 ? nullptr : mementos[stackPointer];
    }

    void reset( std::shared_ptr<T> _value ) {
        mementos.clear();
        stackPointer = -1;
        push(_value);
    }

private:
    std::vector<std::shared_ptr<T>> mementos{};
    int stackPointer = -1;
};

