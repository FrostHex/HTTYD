
namespace UnityEngine.Rendering.UnifiedRayTracing
{
    internal class RayTracingResources : ScriptableObject
    {
        [Reload("Runtime/UnifiedRayTracing/Common/GeometryPool/GeometryPoolKernels.compute")]
        public ComputeShader geometryPoolKernels;
        [Reload("Runtime/UnifiedRayTracing/Common/Utilities/CopyBuffer.compute")]
        public ComputeShader copyBuffer;
        [Reload("Runtime/UnifiedRayTracing/Hardware/HWRayTracingMaterial.shader")]
        public Shader hardwareRayTracingMaterial;

        [Reload("Runtime/UnifiedRayTracing/Compute/RadeonRays/kernels/bit_histogram.compute")]
        public ComputeShader bitHistogram;
        [Reload("Runtime/UnifiedRayTracing/Compute/RadeonRays/kernels/block_reduce_part.compute")]
        public ComputeShader blockReducePart;
        [Reload("Runtime/UnifiedRayTracing/Compute/RadeonRays/kernels/block_scan.compute")]
        public ComputeShader blockScan;
        [Reload("Runtime/UnifiedRayTracing/Compute/RadeonRays/kernels/build_hlbvh.compute")]
        public ComputeShader buildHlbvh;
        [Reload("Runtime/UnifiedRayTracing/Compute/RadeonRays/kernels/reorder_triangle_indices.compute")]
        public ComputeShader reorderTriangleIndices;
        [Reload("Runtime/UnifiedRayTracing/Compute/RadeonRays/kernels/restructure_bvh.compute")]
        public ComputeShader restructureBvh;
        [Reload("Runtime/UnifiedRayTracing/Compute/RadeonRays/kernels/scatter.compute")]
        public ComputeShader scatter;
        [Reload("Runtime/UnifiedRayTracing/Compute/RadeonRays/kernels/top_level_intersector.compute")]
        public ComputeShader topLevelIntersector;
        [Reload("Runtime/UnifiedRayTracing/Compute/RadeonRays/kernels/intersector.compute")]
        public ComputeShader intersector;

    }
}


