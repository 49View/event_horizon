#include <render_scene_graph/event_horizon.h>
#include "editor/full_editor_layout.h"

int main( int argc, char *argv[] ) {

    EventHorizon<EditorBackEnd> ev{ argc, argv };

    return 0;
}

