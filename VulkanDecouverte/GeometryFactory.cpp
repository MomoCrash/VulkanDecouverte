#include "GeometryFactory.h"

#include <fstream>
#include <sstream>

GeometryFactory::GeometryFactory()
{
	
	mPrimitives.try_emplace(Primitive::CUBE, Primitive(CreateCube(1, 1, 1)));
	
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

    //
	// Create the vertices.
	//

	meshData->Vertices = std::vector<Vertex>(24);

	float w2 = 0.5f*width;
	float h2 = 0.5f*height;
	float d2 = 0.5f*depth;
    
	// Fill in the front face vertex data.
	meshData->Vertices[0] = Vertex(-w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	meshData->Vertices[1] = Vertex(-w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	meshData->Vertices[2] = Vertex(+w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	meshData->Vertices[3] = Vertex(+w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	// Fill in the back face vertex data.
	meshData->Vertices[4] = Vertex(-w2, -h2, +d2, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	meshData->Vertices[5] = Vertex(+w2, -h2, +d2, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	meshData->Vertices[6] = Vertex(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	meshData->Vertices[7] = Vertex(-w2, +h2, +d2, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);

	// Fill in the top face vertex data.
	meshData->Vertices[8]  = Vertex(-w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	meshData->Vertices[9]  = Vertex(-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	meshData->Vertices[10] = Vertex(+w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	meshData->Vertices[11] = Vertex(+w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the bottom face vertex data.
	meshData->Vertices[12] = Vertex(-w2, -h2, -d2, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f);
	meshData->Vertices[13] = Vertex(+w2, -h2, -d2, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f);
	meshData->Vertices[14] = Vertex(+w2, -h2, +d2, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f);
	meshData->Vertices[15] = Vertex(-w2, -h2, +d2, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);

	// Fill in the left face vertex data.
	meshData->Vertices[16] = Vertex(-w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	meshData->Vertices[17] = Vertex(-w2, +h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	meshData->Vertices[18] = Vertex(-w2, +h2, -d2, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	meshData->Vertices[19] = Vertex(-w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the right face vertex data.
	meshData->Vertices[20] = Vertex(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	meshData->Vertices[21] = Vertex(+w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	meshData->Vertices[22] = Vertex(+w2, +h2, +d2, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	meshData->Vertices[23] = Vertex(+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
 
	//
	// Create the indices.
	//

	uint32 i[36];

	// Fill in the front face index data
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// Fill in the back face index data
	i[6] = 4; i[7]  = 5; i[8]  = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// Fill in the top face index data
	i[12] = 8; i[13] =  9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;

	// Fill in the bottom face index data
	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;

	// Fill in the left face index data
	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;

	// Fill in the right face index data
	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;

	meshData->Indices.assign(&i[0], &i[36]);

	return meshData;
}
