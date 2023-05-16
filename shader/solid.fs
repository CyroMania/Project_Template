#version 440

layout (location = 0) out vec4 FragColour;
uniform vec4 Colour;

void main() 
{
    FragColour = Colour;
}