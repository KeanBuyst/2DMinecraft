#version 460 core

layout(location = 0) in vec2 position;
layout(location = 1) in uint data;

out vec2 _position;
out uint _block;
out uint _wall;
out uint _light;

void main() {
	_position = position;
	_block = data & 0xFFu;
	_wall = (data >> 8) & 0xFFu;
	_light = (data >> 16) & 0xFu;
}
