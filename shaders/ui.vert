#version 430 core

layout(location = 0) in vec2 position;
layout(location = 1) in uint type;
layout(location = 2) in int item;
layout(location = 3) in int stackSize;

out uint _ui;
out vec2 _position;
out int _item;
out int _stack;

void main() {
    _item = item;
    _ui = type;
    _position = position;
    _stack = stackSize;
}