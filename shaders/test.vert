#version 460 core
#extension GL_ARB_gpu_shader_int64 : enable
#extension GL_NV_gpu_shader5 : enable

layout(location = 0) in vec2 position;
layout(location = 1) in uint64_t row;
layout(location = 2) in uint64_t wall_buffer;
layout(location = 3) in uint light_buffer;

out vec2 rowPos;
out uint64_t blocks;
out uint64_t walls;
out uint lightMap;

void main() {
	rowPos = position;
	blocks = row;
	walls = wall_buffer;
	lightMap = light_buffer;
}
