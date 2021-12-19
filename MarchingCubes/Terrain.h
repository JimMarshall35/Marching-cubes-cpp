#pragma once
#include "ComputeShaderMarcher.h"
#include <vector>
#include "Camera.h"
#include "Renderer.h"
#include "CubeDrawer.h"

#define CHUNK_POOL_SIZE 200

struct VAOVBOPair {
	GLuint VAO;
	GLuint VBO;
};

struct TerrainChunk {
public:
	TerrainChunk();
	glm::vec3 ChunkPosLL;
	glm::vec3 ChunkDim;
	glm::ivec3 VoxelDim;
	VAOVBOPair Buffer;
	WireFrameCubeGL Wireframe;
	unsigned int NumVertices;
};

class Terrain
{
public:
	Terrain();
	void Render(const Camera& camera, int window_w, int window_h);
	void Init();
private:
	std::vector<TerrainChunk> _Chunks;
	ComputeShaderMarcher _Marcher;
	Stopwatch _Stopwatch;
};

