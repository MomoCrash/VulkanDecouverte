#include "RenderObject.h"

RenderObject::RenderObject(Mesh* mesh)
{
    m_mesh = mesh;
}

RenderObject::~RenderObject()
{
}

Mesh const* RenderObject::getMesh()
{
    return m_mesh;
}
