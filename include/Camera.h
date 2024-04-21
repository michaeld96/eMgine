#ifndef CAMERA_H
#define CAMERA_H
#include "Common.h"
class Camera
{
private:
    static inline float x = 0.0f;
    static inline float y = 0.0f;
    static inline float zoom_factor = 1.0f;
public:
    Camera() = default;
    static void SetPosition(float in_x, float in_y)
    {
        x = in_x;
        y = in_y;
    }
    static float GetPositionX()
    {
        return x;
    }
    static float GetPositionY()
    {
        return y;
    }
    static void SetZoom(float in_zoom_factor)
    {
        zoom_factor = in_zoom_factor;
    }
    static float GetZoom()
    {
        return zoom_factor;
    }
};
#endif // CAMERA_H