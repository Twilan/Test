#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 vertexColor;

uniform mat4 uModelViewProjectMatrix;
out vec4 fragmentColor;

void main()
{
	gl_Position = uModelViewProjectMatrix * position;
	//gl_Position = position;

	fragmentColor = vertexColor;
};

#shader fragment
#version 330 core

// Interpolated values from the vertex shaders
in vec4 fragmentColor;
out vec4 color;

uniform vec4 uColor;

void main()
{
	color = fragmentColor;
	//color = uColor;
};