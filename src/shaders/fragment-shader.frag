#version 330
out vec4 FragColor;
// Fragment shader is calculating the color output of the pixels.
// Fragment shader only requires one variable and this defines the
// final color output.

in vec3 our_color;
void main()
{
    // FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    FragColor = vec4(our_color, 1.0f);
}