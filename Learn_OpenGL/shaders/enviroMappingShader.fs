#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main()
{
    float refractRatio = 1.00 / 1.52;
    vec3 I = normalize(Position - cameraPos);
    
    // reflection
    vec3 R = reflect(I, normalize(Normal));

    // refraction
    //vec3 R = refract(I, normalize(Normal), refractRatio);

    FragColor = vec4(texture(skybox, R).rgb, 1.0);
}