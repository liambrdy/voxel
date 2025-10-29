#version 450 core

layout (location = 0) out vec4 color;

layout (location = 0) in vec2 fragUv;

layout (binding = 0) uniform sampler2D texSampler;

void main() {
    color = texture(texSampler, fragUv);
}