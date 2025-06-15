#include "GeometryFactory.h"

#include <fstream>
#include <sstream>

GeometryFactory::GeometryFactory()
{
	
	mPrimitives.try_emplace(Primitive::CUBE, Primitive(CreateCube(1, 1, 1)));
	mPrimitives.try_emplace(Primitive::PLANE, Primitive(CreatePlane(1, 1)));
	
}

GeometryFactory::~GeometryFactory()
{
	
	for (auto& meshPair : mLoadedMesh)
	{
		delete meshPair.second;
	}

	for (auto& primitivePair : mPrimitives)
	{
		delete primitivePair.second.Mesh;
	}
	
}

GeometryFactory& GeometryFactory::getInstance()
{
	static GeometryFactory instance;
	return instance;
}

MeshData* GeometryFactory::LoadOrGetMeshFromFile(std::wstring path, bool invertV)
{

	if (getInstance().mLoadedMesh.contains(path))
	{
		return getInstance().mLoadedMesh.at(path);
	}

	MeshData* data = new MeshData();
	
	std::fstream meshFile;
	meshFile.open(GEOMETRIES_FOLDER + path, std::ios::in);
	
	std::vector<vec2> tempTexCoords;
	std::vector<vec3> tempPosition;
	std::vector<vec3> tempNormal;
	std::unordered_map<std::string, int> vertexCache;  // Maps "v/vt/vn" to a unique index

	for(std::string line; std::getline(meshFile, line); )   // Read stream line by line
	{
	    std::istringstream in(line);      
	    std::string type;
	    in >> type;

	    if(type == "v") 
	    {
	        float x, y, z;
	        in >> x >> y >> z;
	    	tempPosition.push_back({x,y,z});
	    } 
	    else if (type == "vn") 
	    {
	        float x, y, z;
	        in >> x >> y >> z;
	        tempNormal.push_back({x, y, z});
	    } 
	    else if (type == "vt") 
	    { 
	        float u, v;
	        in >> u >> v;
	        tempTexCoords.push_back( vec2(u, invertV ? 1 -v : v) ); // Flip V to match DirectX/OpenGL convention
	    } 
	    else if (type == "f") 
	    {
	        std::vector<int> faceIndices;
	        std::string indicesString;

	        while (in >> indicesString) 
	        {
	            std::istringstream indices(indicesString);
	            std::string vIndex, vtIndex, vnIndex;
	            int v = -1, vt = -1, vn = -1;

	            std::getline(indices, vIndex, '/');  
	            if (!vIndex.empty()) v = std::stoi(vIndex) - 1;

	            if (std::getline(indices, vtIndex, '/')) 
	                if (!vtIndex.empty()) vt = std::stoi(vtIndex) - 1;

	            if (std::getline(indices, vnIndex, '/')) 
	                if (!vnIndex.empty()) vn = std::stoi(vnIndex) - 1;

	            // Create a unique key for this combination of v/vt/vn
	            std::string key = vIndex + "/" + vtIndex + "/" + vnIndex;

	            // Check if we've already created this vertex
	            if (!vertexCache.contains(key)) 
	            {
	                Vertex newVertex; // Copy position
	            	if (v != -1 && v < tempPosition.size()) newVertex.position = tempPosition[v];
	            	if (vn != -1 && vn < tempNormal.size()) newVertex.normal = tempNormal[vn];
	            	if (vt != -1 && vt < tempTexCoords.size()) newVertex.texCoords = tempTexCoords[vt];

	                data->Vertices.push_back(newVertex);
	                vertexCache[key] = data->Vertices.size() - 1;
	            }

	            // Store the correct index
	            faceIndices.push_back(vertexCache[key]);
	        }

	        // Handle triangulation for quads
	        if (faceIndices.size() == 3) {
	            data->Indices.push_back(faceIndices[0]);
	            data->Indices.push_back(faceIndices[1]);
	            data->Indices.push_back(faceIndices[2]);
	        } 
	        else if (faceIndices.size() == 4) {
	            // Convert quad to two triangles
	            data->Indices.push_back(faceIndices[0]);
	            data->Indices.push_back(faceIndices[1]);
	            data->Indices.push_back(faceIndices[2]);

	            data->Indices.push_back(faceIndices[0]);
	            data->Indices.push_back(faceIndices[2]);
	            data->Indices.push_back(faceIndices[3]);
	        }
	    }
	}


	meshFile.close();
	
	return data;
}

MeshData* GeometryFactory::GetPrimitive(Primitive::Type primitiveType)
{
	if (!getInstance().mPrimitives.contains(primitiveType))
	{
		std::cout << "Primitive " << primitiveType << " not found" << std::endl;
		return new MeshData();
	}
	return getInstance().mPrimitives.at(primitiveType).Mesh;
}

MeshData* GeometryFactory::CreateCube(float width, float height, float depth)
{
	MeshData* meshData = new MeshData();

	float w2 = 0.5f*width;
	float h2 = 0.5f*height;
	float d2 = 0.5f*depth;

    //
	// Create the vertices.
	//

	meshData->Vertices = {
		// FRONT FACE
		{-w2, -h2, +d2, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f},
		{+w2, -h2, +d2, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f},
		{+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},
		{-w2, +h2, +d2, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f},

		 // BACK FACE
		{-w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f},
		{-w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f},
		{+w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f},
		{+w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f},

		// TOP FACE
		{-w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},
		{-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f},
		{+w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
		{+w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},

		// Bottom face
		{-w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},
		{-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f},
		{+w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
		{+w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},

		// Left face
		{-w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f},
		{-w2, +h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f},
		{-w2, +h2, -d2, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f},
		{-w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f},

		// Right face
		{+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f},
		{+w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f},
		{+w2, +h2, +d2, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f},
		{+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f},
	};

	meshData->Indices = {
		// Front face
		0, 1, 2,
		0, 2, 3,

		// Back face
		4, 5, 6,
		4, 6, 7,

		// Top face
		8, 9, 10,
		8, 10, 11,

		// Bottom face
		12, 14, 13,
		12, 15, 14,

		// Left face
		16, 17, 18,
		16, 18, 19,

		// Right face
		20, 21, 22,
		20, 22, 23
	};

	return meshData;
}

MeshData* GeometryFactory::CreatePlane(float width, float height)
{

	MeshData* meshData = new MeshData();
	
	meshData->Vertices = {
		{ -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
		{0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f },
		{0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f},
		{-0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f}
	};

	meshData->Indices = {
		0, 2, 1, 2, 0, 3
	};

	return meshData;

}
