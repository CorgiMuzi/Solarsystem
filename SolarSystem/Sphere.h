//////////////////////////////////////////////////////////////////
// Sphere.h
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// AUTHOR : Kim Ji Hun
// CREATED : 21-05-23
//////////////////////////////////////////////////////////////////

#pragma once

class Sphere {
public:
    Sphere(float radius = 1.0f, int longCount = 36, int latCount = 18, bool smooth = true);

    float  getRadius() const { return radius; }
    int getLongCount() const { return longCount; }
    int getLatCount() const { return latCount; }
    void set(float radius, int longCount, int latCount, bool smooth);
    void setRadius(float radius);
    void setLongCount(int longCount);
    void setLatCount(int latCount);
    void setSmooth(bool smooth);

    unsigned int getVertexCount() const { return (unsigned int)vertices.size() / 3; }
    unsigned int getNormalCount() const { return (unsigned int)normals.size() / 3; }
    unsigned int getTexCoordCount() const { return (unsigned int)texCoords.size() / 2; }
    unsigned int getIndexCount() const { return (unsigned int)indices.size(); }
    unsigned int getLineIndexCount() const { return (unsigned int)lineIndices.size(); }
    unsigned int getTriangleCount() const { return getIndexCount() / 3; }
    unsigned int getVertexSize() const { return (unsigned int)vertices.size() * sizeof(float); }
    unsigned int getNormalSize() const { return (unsigned int)normals.size() * sizeof(float); }
    unsigned int getTexCoordSize() const { return (unsigned int)texCoords.size() * sizeof(float); }
    unsigned int getIndexSize() const { return (unsigned int)indices.size() * sizeof(unsigned int); }
    unsigned int getLineIndexSize() const { return (unsigned int)lineIndices.size() * sizeof(unsigned int); }
    const float* getVertices() const { return vertices.data(); }
    const float* getNormals() const { return normals.data(); }
    const float* getTexCoords() const { return texCoords.data(); }
    const unsigned int* getIndices() const { return indices.data(); }
    const unsigned int* getLineIndices() const { return lineIndices.data(); }

    unsigned int getInterleavedVertexCount() const { return getVertexCount(); }    // # of vertices
    unsigned int getInterleavedVertexSize() const { return (unsigned int)interleavedVertices.size() * sizeof(float); }    // # of bytes
    int getInterleavedStride() const { return interleavedStride; }   // 32 bytes
    const float* getInterleavedVertices() const { return interleavedVertices.data(); }

    void draw() const;                                  // draw surface
    //void drawLines(const float lineColor[4]) const;     // draw lines only
    //void drawWithLines(const float lineColor[4]) const; // draw surface and lines

private : 
    float radius;
    int longCount;                        // longitude, # of slices
    int latCount;                         // latitude, # of stacks
    bool smooth;
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> texCoords;
    std::vector<unsigned int> indices;
    std::vector<unsigned int> lineIndices;

    std::vector<float> interleavedVertices;
    int interleavedStride;

    void buildVerticesSmooth();
    void buildVerticesFlat();
    void buildInterleavedVertices();
    void clearArrays();
    void addVertex(float x, float y, float z);
    void addNormal(float x, float y, float z);
    void addTexCoord(float s, float t);
    void addIndices(unsigned int i1, unsigned int i2, unsigned int i3);
    std::vector<float> computeFaceNormal(float x1, float y1, float z1,
        float x2, float y2, float z2,
        float x3, float y3, float z3);


};
