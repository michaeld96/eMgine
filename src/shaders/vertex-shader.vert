#version 330 core
// Creating a variable is after the in vec3 keywords.
// 
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_color;

out vec3 our_color;

void main()
{
    // gl_Position is pre-defined.
    gl_Position = vec4(a_pos.x, a_pos.y, a_pos.z, 1.0);
    our_color = a_color;
}