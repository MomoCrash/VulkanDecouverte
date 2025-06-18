#include "RenderObject.h"

RenderObject::RenderObject(Mesh* mesh)
    : m_transform(mat4(1.0f))
{
    m_mesh = mesh;

    reset();
}

RenderObject::~RenderObject()
{
}

Mesh const* RenderObject::getMesh()
{
    return m_mesh;
}

mat4 const& RenderObject::getTransform()
{
    return m_transform;
}

vec3& RenderObject::getPosition()
{
    return m_position;
}

vec3 const& RenderObject::forward()
{
    return m_forward;
}

vec3 RenderObject::backward()
{
    return m_forward * -1.0f;
}

vec3 const& RenderObject::right()
{
    return m_right;
}

vec3 RenderObject::left()
{
    return m_right * -1.0f;
}

vec3 const& RenderObject::up()
{
    return m_up;
}

vec3 RenderObject::down()
{
    return m_up * -1.0f;
}

void RenderObject::setPosition(vec3 const& position)
{
    m_position = position;
}

void RenderObject::offsetPosition(vec3 const& position)
{
    m_position += position;
}

void RenderObject::rotateYPR(vec3 const& rotation)
{

    vec3 RIGHT		= { 1, 0, 0 };
    vec3 UP			= { 0, 1, 0 };
    vec3 FORWARD	= { 0, 0, 1 };

    m_rotationMatrix = rotate(m_rotationMatrix, rotation.x, right());
    m_rotationMatrix = rotate(m_rotationMatrix, rotation.y, forward());
    m_rotationMatrix = rotate(m_rotationMatrix, rotation.z, up());

    m_right     = { m_rotationMatrix[0][0], m_rotationMatrix[0][1], m_rotationMatrix[0][2] };
    m_up        = { m_rotationMatrix[1][0], m_rotationMatrix[1][1], m_rotationMatrix[1][2] };
    m_forward   = { m_rotationMatrix[2][0], m_rotationMatrix[2][1], m_rotationMatrix[2][2] };
    
}

void RenderObject::update()
{
    
    m_transform = mat4(1.0f);
    
    m_transform = scale(m_transform, m_scale);
    m_transform *= m_rotationMatrix;
    m_transform = translate(m_transform, m_position);
    
}

void RenderObject::reset()
{
    
    m_position  = vec3(0.0f);
    m_scale     = vec3(1.0f);
    
    m_right     = vec3( 1.0f, 0.0f, 0.0f );
    m_up        = vec3( 0.0f, 1.0f, 0.0f );
    m_forward   = vec3( 0.0f, 0.0f, 1.0f );

    m_rotationMatrix = mat4(1.0f);

}
