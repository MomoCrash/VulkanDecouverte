#include "RenderObject.h"

RenderObject::RenderObject(Mesh* mesh)
    : m_transform(mat4(1.0f))
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

mat4& RenderObject::getTransform()
{
    return m_transform;
}

void RenderObject::setTransform(mat4 transform)
{
    m_transform = transform;
}
