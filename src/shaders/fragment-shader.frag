#version 330
out vec4 FragColor;
// Fragment shader is calculating the color output of the pixels.
// Fragment shader only requires one variable and this defines the
// final color output.
void main()
{
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}