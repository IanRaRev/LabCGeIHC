#version 330 core
layout (location = 0) in vec3 in_position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 coordTexture;

void main()
{
	coordTexture = in_position;
	vec4 position = projection * view * model * vec4(in_position, 1.0);
	gl_Position = position.xyww;
}