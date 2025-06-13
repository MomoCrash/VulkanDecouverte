#pragma once
#include <string>

#include "Mesh.h"

struct Primitive {

    enum Type : int8
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

    static GeometryFactory& getInstance();

    [[nodiscard]] static MeshData* GetPrimitive(Primitive::Type);
    
    [[nodiscard]] static MeshData* LoadOrGetMeshFromFile(std::wstring path, bool invertV);
    [[nodiscard]] static MeshData* CreateCube(float width, float height, float depth);

    static const inline wstring GEOMETRIES_FOLDER = L"geometries\\";

private :

    std::map<wstring, MeshData*> mLoadedMesh = {};
    std::map<int8, Primitive> mPrimitives = {};

};
