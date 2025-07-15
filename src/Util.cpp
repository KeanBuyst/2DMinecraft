#include "Util.h"

#include <random>
#include <ctime>

#include "Application.h"
#include "glew.h"
#include "gl/Shader.h"

glm::vec2 Util::rotate(const glm::vec2& point, const float angle)
{
    const float cosTheta = cosf(angle);
    const float sinTheta = sinf(angle);

    return glm::vec2(
        point.x * cosTheta - point.y * sinTheta,
        point.x * sinTheta + point.y * cosTheta
    );
}

void Util::decreaseMagnitude(glm::vec2& vector, const float scaler)
{
    decreaseMagnitude(vector.x,scaler);
    decreaseMagnitude(vector.y,scaler);
}

void Util::decreaseMagnitude(float& vector, const float scaler)
{
    if (vector < 0)
    {
        vector += scaler;
        if (vector > 0)
            vector = 0;
    } else
    {
        vector -= scaler;
        if (vector < 0)
            vector = 0;
    }
}

void Util::drawDebugLines(const glm::vec2* points, int size)
{
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < size; ++i) {
        glVertex2f((points[i].x - world::origin.x) * world::PIXEL_SCALE, (points[i].y - world::origin.y) * world::PIXEL_SCALE);
    }
    glEnd();
}
