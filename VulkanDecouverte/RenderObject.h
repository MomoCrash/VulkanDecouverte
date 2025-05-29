#pragma once

#include "framework.h"

class Mesh;

class RenderObject
{
    Mesh* m_mesh;
public:

    RenderObject(Mesh* mesh);
    ~RenderObject();
    
    Mesh const* getMesh();
};
