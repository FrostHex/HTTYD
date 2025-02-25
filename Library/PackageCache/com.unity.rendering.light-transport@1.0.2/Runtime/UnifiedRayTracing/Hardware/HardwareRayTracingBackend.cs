using System;

namespace UnityEngine.Rendering.UnifiedRayTracing
{
    internal class HardwareRayTracingBackend : IRayTracingBackend
    {
        public HardwareRayTracingBackend(RayTracingResources resources)
        {
            m_Resources = resources;
        }

        public IRayTracingShader CreateRayTracingShader(string shaderFilenameWithoutExtension, string dispatchFuncName, Func<string, Type, Object> shaderFileLoader)
        {
            return new HardwareRayTracingShader(shaderFilenameWithoutExtension, dispatchFuncName, shaderFileLoader);
        }
        public IRayTracingAccelStruct CreateAccelerationStructure(AccelerationStructureOptions options, GeometryPool geometryPool, ReferenceCounter counter)
        {
            return new HardwareRayTracingAccelStruct(geometryPool, m_Resources.hardwareRayTracingMaterial, counter);
        }

        RayTracingResources m_Resources;
    }
}
