#pragma once

#include "framework.h"

class Mesh;

class RenderObject
{
    Mesh* m_mesh;
    mat4 m_transform;
public:

    RenderObject(Mesh* mesh);
    ~RenderObject();
    
    Mesh const* getMesh();
    mat4& getTransform();
    void setTransform(mat4 transform);
};
