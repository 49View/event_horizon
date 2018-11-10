#include "render_scene_graph/event_horizon.h"
#include "render_scene_graph/layouts/editor/full_editor_layout.h"

int main( [[maybe_unused]] int argc, [[maybe_unused]] char *argv[] ) {

    EventHorizon ev{ fullEditor() };

    return 0;
}

