//
// Created by dado on 26/05/2020.
//

#ifndef HOUSE_MAKER_UI_VIEW_TYPES_HPP
#define HOUSE_MAKER_UI_VIEW_TYPES_HPP

struct UICallbackHandle {
    UICallbackHandle() = default;
    UICallbackHandle( const std::string& _index ) {
        index = std::stoi( _index );
    }
    UICallbackHandle( int _index ) : index( _index ) {}

    int index = -1;
};

using ControlDefKey = std::string;
using ControlTapKey = uint64_t;
using ControlDefIconRef = std::string;
using UICallbackFunc = std::function<void( const UICallbackHandle& )>;
using ControlSingleTapCallback = UICallbackFunc;//std::function<void( ControlTapKey )>;
using UICallbackMap  = std::unordered_map<std::string, UICallbackFunc>;

#endif //HOUSE_MAKER_UI_VIEW_TYPES_HPP
