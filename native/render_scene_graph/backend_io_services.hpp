//
// Created by dado on 22/07/2020.
//

#pragma once

#include <graphics/mouse_input.hpp>

class Camera;

struct OnTickControlKeyEvent {
    const AggregatedInputData& aid;
};
struct OnTickEvent {
    const AggregatedInputData& aid;
};

struct OnAltPressedEvent {
};
struct OnClearEvent {
};
struct OnDoubleTapEvent {
};
struct OnUndoEvent {
};
struct OnRedoEvent {
};
struct OnTakeScreenShotEvent {
    std::function<void( const SerializableContainer& )> screenShotCallback = nullptr;
};

struct OnTouchUpWithModKeyCtrlEvent {
    V2f mousePos{ V2fc::HUGE_VALUE_NEG };
};

struct OnFirstTimeTouchDownWithModKeyCtrlEvent {
    V2f mousePos{ V2fc::HUGE_VALUE_NEG };
    const AggregatedInputData& aid;
};

struct OnFirstTimeTouchDownEvent {
    V2f mousePos{ V2fc::HUGE_VALUE_NEG };
    const AggregatedInputData& aid;
};
struct OnFirstTimeTouchDownViewportSpaceEvent {
    V2f viewportPos{ V2fc::HUGE_VALUE_NEG };
};

struct OnTouchMoveEvent {
    V2f mousePos{ V2fc::HUGE_VALUE_NEG };
    const AggregatedInputData& aid;
};

struct OnTouchMoveWithModKeyCtrlEvent {
    V2f mousePos{ V2fc::HUGE_VALUE_NEG };
    const AggregatedInputData& aid;
};
struct OnTouchMoveViewportSpaceEvent {
    V2f viewportPos{ V2fc::HUGE_VALUE_NEG };
};

struct OnSingleTapEvent {
    V2f mousePos{ V2fc::HUGE_VALUE_NEG };
};

struct OnTouchUpEvent {
    V2f mousePos{ V2fc::HUGE_VALUE_NEG };
};

struct OnSingleTapViewportSpaceEvent {
    V2f viewportPos{ V2fc::HUGE_VALUE_NEG };
};

struct OnSingleTapSecondaryEvent {
    V2f viewportPos{ V2fc::HUGE_VALUE_NEG };
};

struct OnTouchUpViewportSpaceEvent {
    V2f viewportPos{ V2fc::HUGE_VALUE_NEG };
};

struct OnKeyToggleEvent {
    int keyCode = 0;
    V2f viewportPos{ V2fc::HUGE_VALUE_NEG };
};

struct OnFinaliseEvent {
};

struct OnEscapeEvent {
};

struct OnSpaceEvent {
};

struct OnSpecialSpaceEvent {
};

struct OnDeleteEvent {
};

struct OnDecreaseValueFastAltEvent{
    int increment = -5;
};

struct OnIncreaseValueFastAltEvent{
    int increment = 5;
};

struct OnDecreaseValueAltEvent{
    int increment = -1;
};

struct OnIncreaseValueAltEvent{
    int increment = 1;
};

struct OnScrollEvent{
    float scrollAmount = 1.0f;
};

struct OnCloneEvent{
};

template <typename T>
void backEndIOEvents( T* backEnd, const AggregatedInputData& _aid, const Camera* cam ) {
    // This acts like a classic update loop function in conventional render/update rendering, expect it's wired in the
    // state machine so we can unify the whole code path.
    backEnd->process_event(OnTickEvent{_aid});

    if ( _aid.mods().isAltPressed ) {
        backEnd->process_event(OnAltPressedEvent{});
    }
    if ( _aid.mods().isShiftPressed ) {
        if ( _aid.TI().checkKeyToggleOn(GMK_DELETE) ) {
            backEnd->process_event(OnClearEvent{});
        }
        if ( _aid.TI().checkKeyToggleOn(GMK_COMMA) ) {
            backEnd->process_event(OnDecreaseValueFastAltEvent{-5});
        }
        if ( _aid.TI().checkKeyToggleOn(GMK_PERIOD) ) {
            backEnd->process_event(OnIncreaseValueFastAltEvent{5});
        }
    }
    if ( _aid.TI().checkKeyToggleOn(GMK_COMMA) ) {
        backEnd->process_event(OnDecreaseValueAltEvent{-1});
    }
    if ( _aid.TI().checkKeyToggleOn(GMK_PERIOD) ) {
        backEnd->process_event(OnIncreaseValueAltEvent{1});
    }

    if ( _aid.mods().isControlKeyDown ) {
        if ( _aid.mods().isShiftPressed && _aid.TI().checkKeyToggleOn(GMK_Z) ) {
            backEnd->process_event(OnRedoEvent{});
        } else if ( _aid.TI().checkKeyToggleOn(GMK_Z) ) {
            backEnd->process_event(OnUndoEvent{});
        }
        if ( _aid.TI().checkKeyToggleOn(GMK_T) ) {
            backEnd->process_event(OnSpecialSpaceEvent{});
        }
    }

    // Comprehensive mouse events taps with mod keys

    if ( _aid.isMouseTouchedDownFirstTime(TOUCH_ZERO) ) {
        backEnd->process_event(OnFirstTimeTouchDownEvent{ _aid.mousePos(TOUCH_ZERO), _aid });
        backEnd->process_event(OnFirstTimeTouchDownViewportSpaceEvent{ _aid.mouseViewportPos(TOUCH_ZERO, cam) });
    }
    if ( _aid.isMouseTouchedDownAndMoving(TOUCH_ZERO) ) {
        backEnd->process_event(OnTouchMoveEvent{ _aid.mousePos(TOUCH_ZERO), _aid });
        backEnd->process_event(OnTouchMoveViewportSpaceEvent{ _aid.mouseViewportPos(TOUCH_ZERO, cam) });
    }
    if ( _aid.isMouseSingleTap( TOUCH_ZERO) ) {
        backEnd->process_event(OnSingleTapEvent{ _aid.mouseViewportPos(TOUCH_ZERO, cam) });
        backEnd->process_event(OnSingleTapViewportSpaceEvent{ _aid.mouseViewportPos(TOUCH_ZERO, cam) });
    }
    if ( _aid.isMouseSingleTap( TOUCH_ONE) ) {
        backEnd->process_event(OnSingleTapSecondaryEvent{ _aid.mouseViewportPos(TOUCH_ONE, cam) });
    }
    if ( _aid.isMouseTouchedUp(TOUCH_ZERO) ) {
        backEnd->process_event(OnTouchUpEvent{ _aid.mousePos(TOUCH_ZERO) });
        backEnd->process_event(OnTouchUpViewportSpaceEvent{ _aid.mouseViewportPos(TOUCH_ZERO, cam) });
    }
    if ( _aid.getScrollValue() != 0.0f ) {
        backEnd->process_event(OnScrollEvent{_aid.getScrollValue()});
    }

    // Principal keyboard inputs

    if ( _aid.TI().checkKeyToggleOn(GMK_ENTER) ) {
        backEnd->process_event(OnFinaliseEvent{});
    }
    if ( _aid.TI().checkKeyToggleOn(GMK_SPACE) ) {
        backEnd->process_event( OnSpaceEvent{} );
    }
    if ( _aid.TI().checkKeyToggleOn(GMK_ESCAPE) ) {
        backEnd->process_event(OnEscapeEvent{});
    }
    if ( _aid.TI().checkKeyToggleOn(GMK_DELETE) ) {
        backEnd->process_event(OnDeleteEvent{});
    }
    if ( _aid.TI().checkKeyToggleOn(GMK_BACKSLASH) ) {
        backEnd->process_event(OnCloneEvent{});
    }

}
