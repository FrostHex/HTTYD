//
// This file was automatically generated. Please don't edit by hand. Execute Editor command [ Edit > Rendering > Generate Shader Includes ] instead
//

#ifndef GEOMETRYPOOLDEFS_CS_HLSL
#define GEOMETRYPOOLDEFS_CS_HLSL
//
// UnityEngine.Rendering.UnifiedRayTracing.GeometryPoolConstants:  static fields
//
#define GEO_POOL_POS_BYTE_SIZE (12)
#define GEO_POOL_UV0BYTE_SIZE (4)
#define GEO_POOL_NORMAL_BYTE_SIZE (4)
#define GEO_POOL_POS_BYTE_OFFSET (0)
#define GEO_POOL_UV0BYTE_OFFSET (12)
#define GEO_POOL_NORMAL_BYTE_OFFSET (16)
#define GEO_POOL_INDEX_BYTE_SIZE (4)
#define GEO_POOL_VERTEX_BYTE_SIZE (20)

//
// UnityEngine.Rendering.UnifiedRayTracing.GeoPoolVertexAttribs:  static fields
//
#define GEOPOOLVERTEXATTRIBS_POSITION (1)
#define GEOPOOLVERTEXATTRIBS_NORMAL (2)
#define GEOPOOLVERTEXATTRIBS_UV (4)

// Generated from UnityEngine.Rendering.UnifiedRayTracing.GeoPoolMeshChunk
// PackingRules = Exact
struct GeoPoolMeshChunk
{
    int indexOffset;
    int indexCount;
    int vertexOffset;
    int vertexCount;
};

// Generated from UnityEngine.Rendering.UnifiedRayTracing.GeoPoolVertex
// PackingRules = Exact
struct GeoPoolVertex
{
    float3 pos;
    float2 uv;
    float3 N;
};


#endif
