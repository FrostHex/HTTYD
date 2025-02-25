using System;

namespace UnityEngine.Rendering.UnifiedRayTracing
{
    internal class ComputeRayTracingBackend : IRayTracingBackend
    {
        public ComputeRayTracingBackend(RayTracingResources resources)
        {
            m_Resources = resources;
        }

        public IRayTracingShader CreateRayTracingShader(string shaderFilenameWithoutExtension, string dispatchFuncName, Func<string, Type, Object> shaderFileLoader)
        {
            return new ComputeRayTracingShader(shaderFilenameWithoutExtension, dispatchFuncName, shaderFileLoader);
        }

        public IRayTracingAccelStruct CreateAccelerationStructure(AccelerationStructureOptions options, GeometryPool geometryPool, ReferenceCounter counter)
        {
            return new ComputeRayTracingAccelStruct(options, geometryPool, m_Resources, counter);
        }

        RayTracingResources m_Resources;
    }
}
