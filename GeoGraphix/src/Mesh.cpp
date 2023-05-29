#include "Mesh.h"

Mesh::Mesh(const HeightMap& heightMap)
{
    Generate(heightMap);
}

Mesh::~Mesh()
{
    Destroy();
}

void Mesh::Generate(const HeightMap& heightMap)
{
    int mapWidth = heightMap.m_Width;
    int mapLength = heightMap.m_Length;

    // make the map from -10.0f to 10.0f, regardless of how many sample points are on the map
    m_Vertices = new float[3 * mapWidth * mapLength] {}; // 3D location of each point
    for (int j = 0; j < mapLength; j++)
    {
        for (int i = 0; i < mapWidth; i++)
        {
            // x value of the (i,j) point
            m_Vertices[3 * mapWidth * j + 3 * i + 0] = 20.0f * i / (mapWidth - 1) - 10.0f;
            // y value of the (i,j) point
            m_Vertices[3 * mapWidth * j + 3 * i + 1] = heightMap.m_Map[j * mapWidth + i];
            // z value of the (i,j) point
            m_Vertices[3 * mapWidth * j + 3 * i + 2] = 20.0f * j / (mapLength - 1) - 10.0f;
        }
    }

    // if there are mapWidth x mapLength points, there will be (mapWidth - 1) x (mapLength - 1) squares, so 6 * (mapWidth - 1) x (mapLength - 1) indices
    m_Indices = new unsigned int[6 * (mapWidth - 1) * (mapLength - 1)];
    for (int j = 0; j < mapLength - 1; j++)
    {
        for (int i = 0; i < mapWidth - 1; i++)
        {
            // first triangle of the quad (bottom right)
            m_Indices[6 * (mapWidth - 1) * j + 6 * i + 0] = j * mapWidth + i;
            m_Indices[6 * (mapWidth - 1) * j + 6 * i + 1] = j * mapWidth + (i + 1);
            m_Indices[6 * (mapWidth - 1) * j + 6 * i + 2] = (j + 1) * mapWidth + (i + 1);

            // second triangle of the quad (top left)
            m_Indices[6 * (mapWidth - 1) * j + 6 * i + 3] = (j + 1) * mapWidth + (i + 1);
            m_Indices[6 * (mapWidth - 1) * j + 6 * i + 4] = (j + 1) * mapWidth + i;
            m_Indices[6 * (mapWidth - 1) * j + 6 * i + 5] = j * mapWidth + i;
        }
    }
}

void Mesh::Destroy()
{
    delete[] m_Vertices;
    delete[] m_Indices;
}

void Mesh::Regenerate(const HeightMap& heightMap)
{
    Destroy();
    Generate(heightMap);
}
