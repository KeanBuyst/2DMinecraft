#version 430 core

layout(location = 0) in vec2 position;
layout(location = 1) in uint data;

out vec2 _position;
out uint _block;
out uint _wall;
out uint _light;
out uint _border;
out uint _breakState;

void main() {
	_position = position;
	_block = data & 0xFFu;
	_wall = (data >> 8) & 0xFFu;
	_light = (data >> 16) & 0xFu;
	_border = (data >> 20) & 0xFu;
	_breakState = (data >> 24) & 0xFu;
}
