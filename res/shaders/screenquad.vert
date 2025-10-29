#version 450 core

layout (location = 0) out vec2 fragUv;

void main() {
    vec2 positions[4] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
    vec2 uvs[4] = {{0, 1}, {0, 0}, {1, 1}, {1, 0}};

    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    fragUv = uvs[gl_VertexIndex];
}