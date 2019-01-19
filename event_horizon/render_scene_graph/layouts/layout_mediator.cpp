//
// Created by Dado on 2019-01-19.
//

#include "layout_mediator.hpp"
#include "render_scene_graph/layouts/editor/timeline_layout.h"

namespace LayoutMediator {

namespace Timeline {
    std::string TimeLineName() {
        return GuiTimeline::TimeLineName();
    }

    float CurrentTime() {
        return GuiTimeline::CurrentTime();
    }
};

};
