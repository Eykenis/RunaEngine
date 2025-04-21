#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTexCoord;
out vec3 normal;
out vec4 worldPos;
out vec2 texcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 perspective;

void main()
{
    worldPos = model * vec4(aPos, 1.0);
    gl_Position = perspective * view * worldPos;
    normal = (transpose(inverse(mat3(model))) * aNormal);
    texcoord = vec2(aTexCoord.x, aTexCoord.y);
}