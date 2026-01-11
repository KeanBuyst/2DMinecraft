#include "Util.h"

#include <random>
#include <ctime>
#include <chrono>

#include "Application.h"
#include "gl/Shader.h"

// Jan 1, 2024
static constexpr uint64_t EPOCH = 1704067200000ULL;

uint64_t Util::GetTimeBasedID()
{
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch(); // 1970 epoch
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() - EPOCH;
}

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
    /*glBegin(GL_LINE_LOOP);
    for (int i = 0; i < size; ++i) {
        glVertex2f((points[i].x - world::origin.x) * world::PIXEL_SCALE, (points[i].y - world::origin.y) * world::PIXEL_SCALE);
    }
    glEnd();*/
}

Util::ByteStream::ByteStream(std::vector<uint8_t>* array) : stream(nullptr), vector(array), cursor(0)
{}

Util::ByteStream::ByteStream(std::iostream* stream) : stream(stream), vector(nullptr), cursor(0)
{}

void Util::ByteStream::write(const void* data, size_t size)
{
    if (stream)
    {
        stream->write(reinterpret_cast<const char*>(data),size);
        return;
    }
    if (vector)
    {
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data);
        vector->insert(vector->end(),bytes,bytes + size);
        return;
    }
}

void Util::ByteStream::reset()
{
    cursor = 0;
    if (stream)
    {
        stream->clear();
        stream->seekg(0, std::ios::beg);
        stream->seekp(0, std::ios::beg);
    }
}

void Util::ByteStream::read(void* data, size_t size)
{
    if (stream)
    {
        stream->read(reinterpret_cast<char*>(data),size);
        return;
    }
    if (vector)
    {
        size_t length = cursor + size > vector->size() ? vector->size() : size;
        std::memcpy(data,vector->data() + cursor, length);
        cursor += length;
    }
}

bool Util::ByteStream::hasNext()
{
    if (vector)
    {
        return cursor != vector->size();
    }
    if (stream)
    {
        return !stream->eof();
    }
    return false;
}