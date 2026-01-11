#include "Frame.h"

#include "../Constants.h"

void gl::Frame::operator*=(float scale)
{
    left *= scale;
    right *= scale;
    top *= scale;
    bottom *= scale;
}

// positions in  pixels
gl::Frame gl::GetFrame(float pX, float pY, float pWidth, float pHeight)
{
    pX -= pWidth / 2.0f;
    pY -= pHeight / 2.0f;
    return {
        pX / BLOCK_SIZE,
        (pX + pWidth) / BLOCK_SIZE,
        (pY + pHeight) / BLOCK_SIZE,
        pY / BLOCK_SIZE
    };
}
