#version 430

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

layout(location = 0) in vec3 vPosition;

out vec4 vColor;

void main() {
    vColor = vec4(1.0, 0.0, 0.0, 1.0);
    gl_Position = projection * view * model * vec4(vPosition, 1.0);
}
