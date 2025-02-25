using System;

namespace UnityEngine.Rendering.UnifiedRayTracing
{
    [GenerateHLSL]
    internal static class GeometryPoolConstants
    {
        public static int GeoPoolPosByteSize = 3 * 4;
        public static int GeoPoolUV0ByteSize = 1 * 4;
        public static int GeoPoolNormalByteSize = 1 * 4;

        public static int GeoPoolPosByteOffset = 0;
        public static int GeoPoolUV0ByteOffset = GeoPoolPosByteOffset + GeoPoolPosByteSize;
        public static int GeoPoolNormalByteOffset = GeoPoolUV0ByteOffset + GeoPoolUV0ByteSize;

        public static int GeoPoolIndexByteSize = 4;
        public static int GeoPoolVertexByteSize = GeoPoolPosByteSize + GeoPoolUV0ByteSize + GeoPoolNormalByteSize;
    }

    [GenerateHLSL(needAccessors = false)]
    internal struct GeoPoolVertex
    {
        public Vector3 pos;
        public Vector2 uv;
        public Vector3 N;
    }

    [GenerateHLSL(needAccessors = false)]
    internal struct GeoPoolMeshChunk
    {
        public int indexOffset;
        public int indexCount;
        public int vertexOffset;
        public int vertexCount;
    }

    [Flags]
    [GenerateHLSL]
    internal enum GeoPoolVertexAttribs { Position = 1, Normal = 2, Uv = 4 }
}
