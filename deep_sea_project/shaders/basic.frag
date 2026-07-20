#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D wallTexture;
uniform sampler2D floorTexture;

void main()
{
    vec3 norm=normalize(Normal);

    // SÀN
    if(abs(norm.z)>0.7 && FragPos.z<0.2)
        FragColor=texture(floorTexture,TexCoord);

    // TRẦN
    else if(abs(norm.z)>0.7 && FragPos.z>0.6)
        FragColor=vec4(1.0,1.0,1.0,1.0);

    // TƯỜNG
    else
        FragColor=texture(wallTexture,TexCoord);
}