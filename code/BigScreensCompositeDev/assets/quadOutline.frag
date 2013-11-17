#version 150 core
#extension all : warn

out vec4 color;
uniform vec4 uColor;

in GS_OUT
{
    float fog;
    
} fs_in;

void main(void)
{
    float brightness = 1.0 - (fs_in.fog * 0.000075);
    color = uColor * brightness;
}