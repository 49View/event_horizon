#version #opengl_version
       layout( location = 0 ) in vec3 a_position;
       layout( location = 1 ) in vec2 a_t1;
       layout( location = 2 ) in vec2 a_tuv2;
       layout( location = 3 ) in vec3 a_t2;
       layout( location = 4 ) in vec4 a_t3;
       layout( location = 5 ) in vec3 a_t4;

    //    layout( location = 5 ) in vec3 shR1;
    //    layout( location = 6 ) in vec3 shR2;
    //    layout( location = 7 ) in vec3 shR3;

    //    layout( location = 8 ) in vec3 shG1;
    //    layout( location = 9 ) in vec3 shG2;
    //    layout( location = 10 ) in vec3 shG3;

    //    layout( location = 11 ) in vec3 shB1;
    //    layout( location = 12 ) in vec3 shB2;
    //    layout( location = 13 ) in vec3 shB3;

       layout( location = 6 ) in vec4 a_t8;

       out vec2 v_texCoord;
       out vec2 v_texCoord2;
      //  out vec3 v_color;
       out vec3 v_norm;
       out vec3 v_tan;
       out vec3 v_bitan;
       out vec3 v_shadowmap_coord3;
      //  out vec4 v_t8;

      out vec3 Position_worldspace;
      out vec3 tangentViewPos;
      out vec3 tangentFragPos;
    