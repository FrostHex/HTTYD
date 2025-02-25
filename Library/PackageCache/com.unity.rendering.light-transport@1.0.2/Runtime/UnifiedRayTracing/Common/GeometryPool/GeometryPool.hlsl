#ifndef GEOMETRY_POOL_H
#define GEOMETRY_POOL_H

namespace GeometryPool
{

float2 msign(float2 v)
{
    return float2(
        (v.x >= 0.0) ? 1.0 : -1.0,
        (v.y >= 0.0) ? 1.0 : -1.0);
}

uint NormalToOctahedral32(float3 normal)
{
    normal.xy /= (abs(normal.x) + abs(normal.y) + abs(normal.z));
    normal.xy = (normal.z >= 0.0) ? normal.xy : (1.0 - abs(normal.yx)) * msign(normal.xy);

    uint2 d = uint2(round(32767.5 + normal.xy * 32767.5));
    return d.x | (d.y << 16u);
}

float3 Octahedral32ToNormal(uint data)
{
    uint2 iv = uint2(data, data >> 16u) & 65535u;
    float2 v = float2(iv) / 32767.5 - 1.0;

    float3 normal = float3(v, 1.0 - abs(v.x) - abs(v.y));
    float t = max(-normal.z, 0.0);
    normal.x += (normal.x > 0.0) ? -t : t;
    normal.y += (normal.y > 0.0) ? -t : t;

    return normalize(normal);
}

uint UvsToUint32(float2 uv)
{
    return f32tof16(uv.x) | (f32tof16(uv.y) << 16);
}

float2 Uint32ToUvs(uint data)
{
    return float2(f16tof32(data & 65535u), f16tof32(data >> 16u));
}

void StoreVertex(
    uint vertexIndex,
    in GeoPoolVertex vertex,
    int outputBufferSize,
    RWStructuredBuffer<uint> output)
{
    uint posIndex = vertexIndex * GEO_POOL_VERTEX_BYTE_SIZE / 4;;
    output[posIndex] = asuint(vertex.pos.x);
    output[posIndex+1] = asuint(vertex.pos.y);
    output[posIndex+2] = asuint(vertex.pos.z);

    uint2 data;
    data.x = UvsToUint32(vertex.uv);
    data.y = NormalToOctahedral32(vertex.N);
    uint dataIndex = (vertexIndex * GEO_POOL_VERTEX_BYTE_SIZE + GEO_POOL_UV0BYTE_OFFSET) / 4;
    output[dataIndex] = data.x;
    output[dataIndex + 1] = data.y;
}

void LoadVertex(
    uint vertexIndex,
    int vertexBufferSize,
    int vertexFlags,
    StructuredBuffer<uint> vertexBuffer,
    out GeoPoolVertex outputVertex)
{
    uint posIndex = vertexIndex * GEO_POOL_VERTEX_BYTE_SIZE / 4;
    float3 pos = asfloat(uint3(vertexBuffer[posIndex], vertexBuffer[posIndex+1], vertexBuffer[posIndex+2]));

    uint dataIndex = (vertexIndex * GEO_POOL_VERTEX_BYTE_SIZE + GEO_POOL_UV0BYTE_OFFSET )/ 4;
    uint2 data = uint2(vertexBuffer[dataIndex], vertexBuffer[dataIndex+1]);

    outputVertex.pos = pos;
    outputVertex.uv = Uint32ToUvs(data.x);
    outputVertex.N = Octahedral32ToNormal(data.y);
}

}

#endif
