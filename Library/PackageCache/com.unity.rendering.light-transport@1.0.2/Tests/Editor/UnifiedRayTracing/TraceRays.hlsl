#include "Packages/com.unity.rendering.light-transport/Runtime/UnifiedRayTracing/FetchGeometry.hlsl"
#include "Packages/com.unity.rendering.light-transport/Runtime/UnifiedRayTracing/TraceRay.hlsl"

UNITY_DECLARE_RT_ACCEL_STRUCT(_AccelStruct);

struct RayWithFlags
{
    float3 origin;
    float tMin;
    float3 direction;
    float tMax;
    uint culling;
    uint instanceMask;
    uint padding;
    uint padding2;
};

StructuredBuffer<RayWithFlags> _Rays;
RWStructuredBuffer<UnifiedRT::Hit> _Hits;

void RayGenExecute(UnifiedRT::DispatchInfo dispatchInfo)
{

    RayWithFlags rayWithFlags = _Rays[dispatchInfo.globalThreadIndex];
    UnifiedRT::Ray ray;
    ray.origin = rayWithFlags.origin;
    ray.direction = rayWithFlags.direction;
    ray.tMin = rayWithFlags.tMin;
    ray.tMax = rayWithFlags.tMax;

    UnifiedRT::RayTracingAccelStruct accelStruct = UNITY_GET_RT_ACCEL_STRUCT(_AccelStruct);
    UnifiedRT::Hit hitResult = UnifiedRT::TraceRayClosestHit(dispatchInfo, accelStruct, rayWithFlags.instanceMask, ray, rayWithFlags.culling);

    _Hits[dispatchInfo.globalThreadIndex] = hitResult;
}
