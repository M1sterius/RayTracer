#pragma once

class RayTracer
{
public:
    static RayTracer& Init();

    void Update() const;
private:
    RayTracer();
    ~RayTracer();
};
