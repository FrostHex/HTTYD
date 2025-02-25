using UnityEngine;
using UnityEditor;

// 缩放 terrain 至指定比例
public class ScaleTerrain : MonoBehaviour
{
    [MenuItem("Tools/Scale Terrain")]
    public static void ScaleSelectedTerrainAndTextures()
    {
        if (Selection.activeGameObject != null)
        {
            Terrain terrain = Selection.activeGameObject.GetComponent<Terrain>();
            if (terrain != null)
            {
                TerrainData terrainData = terrain.terrainData;
                if (terrainData != null)
                {
                    // 缩放比例
                    float scale = 0.5f;

                    // 缩放地形大小
                    Vector3 newSize = terrainData.size * scale;
                    terrainData.size = newSize;

                    // 缩放高度图
                    int heightmapWidth = terrainData.heightmapResolution;
                    int heightmapHeight = terrainData.heightmapResolution;
                    float[,] heights = terrainData.GetHeights(0, 0, heightmapWidth, heightmapHeight);
                    int newHeightmapWidth = Mathf.RoundToInt(heightmapWidth * scale);
                    int newHeightmapHeight = Mathf.RoundToInt(heightmapHeight * scale);
                    float[,] newHeights = new float[newHeightmapWidth, newHeightmapHeight];

                    for (int y = 0; y < newHeightmapHeight; y++)
                    {
                        for (int x = 0; x < newHeightmapWidth; x++)
                        {
                            int oldX = Mathf.RoundToInt(x );
                            int oldY = Mathf.RoundToInt(y);
                            if (oldX >= 0 && oldX < heightmapWidth && oldY >= 0 && oldY < heightmapHeight )
                            {
                                newHeights[y, x] = heights[oldY, oldX];
                            }
                        }
                    }

                    // terrainData.heightmapResolution = newHeightmapWidth; // Width and height should be the same
                    terrainData.SetHeights(0, 0, newHeights);

                    // 缩放纹理图
                    int alphamapWidth = terrainData.alphamapWidth;
                    int alphamapHeight = terrainData.alphamapHeight;
                    float[,,] alphaMaps = terrainData.GetAlphamaps(0, 0, alphamapWidth, alphamapHeight);
                    int newAlphamapWidth = Mathf.RoundToInt(alphamapWidth * scale);
                    int newAlphamapHeight = Mathf.RoundToInt(alphamapHeight * scale);
                    float[,,] newAlphaMaps = new float[newAlphamapWidth, newAlphamapHeight, terrainData.alphamapLayers];

                    for (int y = 0; y < newAlphamapHeight; y++)
                    {
                        for (int x = 0; x < newAlphamapWidth; x++)
                        {
                            int oldX = Mathf.RoundToInt(x / scale);
                            int oldY = Mathf.RoundToInt(y / scale);
                            if (oldX >= 0 && oldX < alphamapWidth && oldY >= 0 && oldY < alphamapHeight)
                            {
                                for (int layer = 0; layer < terrainData.alphamapLayers; layer++)
                                {
                                    newAlphaMaps[x, y, layer] = alphaMaps[oldX, oldY, layer];
                                }
                            }
                        }
                    }

                    terrainData.alphamapResolution = newAlphamapWidth; // Width and height should be the same
                    terrainData.SetAlphamaps(0, 0, newAlphaMaps);

                    // 保存并刷新
                    EditorUtility.SetDirty(terrainData);
                    AssetDatabase.SaveAssets();
                    AssetDatabase.Refresh();

                    Debug.Log("Terrain and Textures scaled to 0.6x successfully.");
                }
                else
                {
                    Debug.LogError("No TerrainData found on the selected Terrain object.");
                }
            }
            else
            {
                Debug.LogError("Selected object is not a Terrain.");
            }
        }
        else
        {
            Debug.LogError("No object selected.");
        }
    }
}
