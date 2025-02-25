using System;

namespace UnityEngine.Rendering.UnifiedRayTracing
{
    internal interface IRayTracingBackend
    {
        IRayTracingShader CreateRayTracingShader(
            string shaderFilenameWithoutExtension,
            string dispatchFuncName,
            Func<string, Type, Object> fileResourceLoader);

        IRayTracingAccelStruct CreateAccelerationStructure(
            AccelerationStructureOptions options,
            GeometryPool geometryPool,
            ReferenceCounter counter);
    }
}
