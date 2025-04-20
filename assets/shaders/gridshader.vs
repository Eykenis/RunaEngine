#version 330 core
layout(location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 perspective;

out vec3 worldPos;
out vec3 camPos;

void main()
{
    vec4 world = model * vec4(aPos, 1.0);
    worldPos = world.xyz;
    gl_Position = perspective * view * world;
    camPos = (inverse(view) * vec4(0, 0, 0, 1.0)).xyz;
}