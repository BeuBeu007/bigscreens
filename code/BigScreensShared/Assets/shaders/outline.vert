#version 150 core

in vec2         position;
uniform vec4    uColor;

void main( void )
{
	gl_Position	= vec4(position, 0.0, 1.0);
}