#version 420 core
out vec4 FragColor;

in vec2 TexCoords;

uniform vec3 objectColor;
uniform sampler2D texture1;

//layout (depth_greater) out float gl_FragDepth;

void main()
{
    bool condition = (gl_FragCoord.x < 400);
    //condition = (gl_FrontFacing);

    if (condition)
    {
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    }
    else
    {
        FragColor = vec4(0.0, 1.0, 0.0, 1.0);
    }

    //gl_FragDepth = gl_FragCoord.z + 0.1;
}