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
    m_VertexPos = new float[3 * mapWidth * mapLength] {}; // 3D location of each point
    for (int j = 0; j < mapLength; j++)
    {
        for (int i = 0; i < mapWidth; i++)
        {
            // x value of the (i,j) point
            m_VertexPos[3 * mapWidth * j + 3 * i + 0] = 20.0f * i / (mapWidth - 1) - 10.0f;
            // y value of the (i,j) point
            m_VertexPos[3 * mapWidth * j + 3 * i + 1] = heightMap.m_Map[j * mapWidth + i];
            // z value of the (i,j) point
            m_VertexPos[3 * mapWidth * j + 3 * i + 2] = 20.0f * j / (mapLength - 1) - 10.0f;
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

    // if there are mapWidth x mapLength points, there will be (mapWidth - 1) x (mapLength - 1) squares, so 2(mapWidth - 1) x 2(mapLength - 1) triangles
    // side note (this can be added to loop above for efficiency, but is isolated for clarity
    for (int j = 0; j < mapLength - 1; j++)
    {
        std::vector<std::vector<glm::vec3>> rowFaceNormals;
        for (int i = 0; i < mapWidth - 1; i++)
        {
            std::vector<glm::vec3> triFaceNormals;
            // get square vertices
            glm::vec3 vert0 = glm::vec3(
                m_VertexPos[3 * (j * mapWidth + i) + 0], 
                m_VertexPos[3 * (j * mapWidth + i) + 1], 
                m_VertexPos[3 * (j * mapWidth + i) + 2]
            );
            glm::vec3 vert1 = glm::vec3(
                m_VertexPos[3 * (j * mapWidth + (i + 1)) + 0], 
                m_VertexPos[3 * (j * mapWidth + (i + 1)) + 1], 
                m_VertexPos[3 * (j * mapWidth + (i + 1)) + 2]
            );
            glm::vec3 vert2 = glm::vec3(
                m_VertexPos[3 * ((j + 1) * mapWidth + (i + 1)) + 0],
                m_VertexPos[3 * ((j + 1) * mapWidth + (i + 1)) + 1],
                m_VertexPos[3 * ((j + 1) * mapWidth + (i + 1)) + 2]
            );
            glm::vec3 vert3 = glm::vec3(
                m_VertexPos[3 * ((j + 1) * mapWidth + i) + 0],
                m_VertexPos[3 * ((j + 1) * mapWidth + i) + 1],
                m_VertexPos[3 * ((j + 1) * mapWidth + i) + 2]
            );

            // first triangle of the quad (bottom right) (0, 1, 2)
            glm::vec3 dir1 = vert2 - vert1;
            glm::vec3 dir2 = vert0 - vert1;
            triFaceNormals.push_back(-glm::normalize(glm::cross(dir1, dir2)));

            // second triangle of the quad (top left) (2, 3, 0)
            dir1 = vert0 - vert3;
            dir2 = vert2 - vert3;
            triFaceNormals.push_back(-glm::normalize(glm::cross(dir1, dir2)));
            rowFaceNormals.push_back(triFaceNormals);
        }
        m_FaceNormals.push_back(rowFaceNormals);
    }

    // same size as m_VertexPos
    m_VertexNormals = new float[3 * mapWidth * mapLength] {}; // 3D normal of each vertex
    // consider the 6 potential candidate faces which can touch the current vertex
    std::vector<std::vector<int>> adj = { {0, 0, 0}, {0, 0, 1}, {-1, 0, 0}, {-1, -1, 1}, {-1, -1, 0}, {0, -1, 1} };
    for (int j = 0; j < mapLength; j++)
    {
        for (int i = 0; i < mapWidth; i++)
        {
            glm::vec3 currVertNormal = glm::vec3( 0, 0, 0 );
            int adjFaces = 0;

            for (std::vector<int> adjFaceIdx : adj)
            {
                int x = i + adjFaceIdx[0];
                if (x == -1 || x == mapWidth - 1) // left edge or right edge of the mesh
                    continue;
                int y = j + adjFaceIdx[1];
                if (y == -1 || y == mapWidth - 1) // bottom edge or top edge of the mesh
                    continue;
                // otherwise, add face normal to crrVertNormal and increment adjFaces for normalization later
                currVertNormal += m_FaceNormals[y][x][adjFaceIdx[2]];
                adjFaces += 1;
            }

            currVertNormal /= adjFaces;
            
            // x value of the normal
            m_VertexNormals[3 * mapWidth * j + 3 * i + 0] = currVertNormal.x;
            // y value of the normal
            m_VertexNormals[3 * mapWidth * j + 3 * i + 1] = currVertNormal.y;
            // z value of the normal
            m_VertexNormals[3 * mapWidth * j + 3 * i + 2] = currVertNormal.z;
        }
    }

    // 3D pos and normal of each vertex
    m_Vertices = new float[6 * mapWidth * mapLength] {}; 
    for (int j = 0; j < mapLength; j++)
    {
        for (int i = 0; i < mapWidth; i++)
        {
            // x value of the (i,j) point
            m_Vertices[6 * (mapWidth * j + i) + 0] = m_VertexPos[3 * (mapWidth * j + i) + 0];
            // y value of the (i,j) point
            m_Vertices[6 * (mapWidth * j + i) + 1] = m_VertexPos[3 * (mapWidth * j + i) + 1];
            // z value of the (i,j) point
            m_Vertices[6 * (mapWidth * j + i) + 2] = m_VertexPos[3 * (mapWidth * j + i) + 2];

            // x value of the normal
            m_Vertices[6 * (mapWidth * j + i) + 3] = m_VertexNormals[3 * (mapWidth * j + i) + 0];
            // y value of the normal
            m_Vertices[6 * (mapWidth * j + i) + 4] = m_VertexNormals[3 * (mapWidth * j + i) + 1];
            // z value of the normal
            m_Vertices[6 * (mapWidth * j + i) + 5] = m_VertexNormals[3 * (mapWidth * j + i) + 2];
        }
    }
}

void Mesh::Destroy()
{
    delete[] m_VertexPos;
    delete[] m_Indices;
}

void Mesh::Regenerate(const HeightMap& heightMap)
{
    Destroy();
    Generate(heightMap);
}
