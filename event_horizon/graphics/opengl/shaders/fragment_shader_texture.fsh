#version #opengl_version

       in vec2 v_texCoord;
       out vec4 FragColor;

       uniform sampler2D colorTexture;
       uniform float alpha;
       uniform float opacity;
       uniform vec3 diffuseColor;

       void main()
       {
           vec4 textureCol = texture(colorTexture, v_texCoord);
           FragColor = vec4( textureCol.rgb * diffuseColor, min(alpha, textureCol.a*opacity));
       }
    