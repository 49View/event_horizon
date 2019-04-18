#include <render_scene_graph/event_horizon.h>
#include <render_scene_graph/layouts/editor/full_editor_layout.h>

int main( int argc, char *argv[] ) {

    EventHorizon<FullEditor> ev{ argc, argv };

    return 0;
}

