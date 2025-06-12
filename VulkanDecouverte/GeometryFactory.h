#pragma once
#include <string>

#include "Mesh.h"

struct Primitive {
    enum Type
    {
        CUBE,

        PRIMITIVE_COUNT
    };

    Primitive(MeshData* mesh) : Mesh(mesh) {}
    
    MeshData* Mesh;
};

class GeometryFactory
{
public:
    GeometryFactory();
    ~GeometryFactory();

    [[nodiscard]] MeshData* LoadOrGetMeshFromFile(std::wstring path, bool invertV);
    [[nodiscard]] MeshData* GetPrimitive(Primitive::Type);
    [[nodiscard]] MeshData* CreateCube(float width, float height, float depth);

    static const inline wstring GEOMETRIES_FOLDER = L"geometries\\";

private :

    std::map<wstring, MeshData*> mLoadedMesh;
    std::map<Primitive::Type, Primitive> mPrimitives;

};
