#pragma once

#include "framework.h"

class Mesh;

class RenderObject
{
    Mesh* m_mesh;
    mat4 m_transform;

    vec3 m_forward;
    vec3 m_right;
    vec3 m_up;

    vec3 m_scale;
    vec3 m_position;

    mat4 m_rotationMatrix;

public:

    RenderObject(Mesh* mesh);
    ~RenderObject();
    
    Mesh const* getMesh();
    mat4 const& getTransform();

    vec3& getPosition();
    vec3 const& forward();
    vec3 backward();
    vec3 const& right();
    vec3 left();
    vec3 const& up();
    vec3 down();

    void setPosition(vec3 const& position);
    void offsetPosition(vec3 const& position);
    
    void rotateYPR(vec3 const&rotation);

    

    void update();
    void reset();
};
