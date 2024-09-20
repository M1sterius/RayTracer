#include "RayTracer.hpp"

RayTracer::RayTracer()
{

}

RayTracer::~RayTracer()
{

}

void RayTracer::Update() const
{

}

RayTracer& RayTracer::Init()
{
    static auto rayTracer = RayTracer();
    return rayTracer;
}
