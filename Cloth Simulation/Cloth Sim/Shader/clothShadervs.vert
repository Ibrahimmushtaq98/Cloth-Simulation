#version 330 core

in vec3 vPosition;
in vec3 vNormal;
in vec2 vTextCoord;

uniform mat4 modelViewProjection;
uniform mat4 model;

out vec4 position;
out vec3 normals;
out vec2 tc;

void main(){
	gl_Position = modelViewProjection * vec4(vPosition, 1.0);
	position = model * vec4(vPosition, 1.0);
	normals = vNormal;
	tc = vTextCoord;
}
