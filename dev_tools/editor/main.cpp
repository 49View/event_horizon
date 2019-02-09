#include "render_scene_graph/event_horizon.h"
#include "render_scene_graph/layouts/editor/full_editor_layout.h"

int main( [[maybe_unused]] int argc, [[maybe_unused]] char *argv[] ) {

    EventHorizon<FullEditor> ev{};//{ fullEditor(), LoginFields::Computer(), InitializeWindowFlags::HalfSize };

    return 0;
}

