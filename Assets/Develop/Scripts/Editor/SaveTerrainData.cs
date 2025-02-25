using UnityEngine;
using UnityEditor;

public class SaveTerrainData : MonoBehaviour
{
    [MenuItem("Tools/Save Terrain Data")]
    public static void SaveSelectedTerrainData()
    {
        if (Selection.activeGameObject != null)
        {
            Terrain terrain = Selection.activeGameObject.GetComponent<Terrain>();
            if (terrain != null)
            {
                TerrainData terrainData = terrain.terrainData;

                if (terrainData != null)
                {
                    string path = EditorUtility.SaveFilePanelInProject("Save Terrain Data", terrainData.name, "asset", "Please enter a file name to save the terrain data to");

                    if (!string.IsNullOrEmpty(path))
                    {
                        TerrainData newTerrainData = Object.Instantiate(terrainData);
                        AssetDatabase.CreateAsset(newTerrainData, path);
                        AssetDatabase.SaveAssets();
                        AssetDatabase.Refresh();
                        Debug.Log("Terrain Data saved to: " + path);
                    }
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
