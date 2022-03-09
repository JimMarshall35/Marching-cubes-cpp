#pragma once
#include "ComputeShaderMarcher.h"
#include <vector>
#include "Camera.h"
#include "Renderer.h"
#include "CubeDrawer.h"

#define CHUNK_POOL_SIZE 200

struct Plane
{
	glm::vec3 normal = { 0.f, 1.f, 0.f }; // unit vector
	float     distance = 0.f;        // Distance with origin

	Plane() = default;

	Plane(const glm::vec3& p1, const glm::vec3& norm)
		: normal(glm::normalize(norm)),
		distance(glm::dot(normal, p1))
	{}

	float getSignedDistanceToPlane(const glm::vec3& point) const
	{
		return glm::dot(normal, point) - distance;
	}
};

struct Frustum
{
	Plane topFace;
	Plane bottomFace;

	Plane rightFace;
	Plane leftFace;

	Plane farFace;
	Plane nearFace;
};

struct VAOVBOPair {
	GLuint VAO;
	GLuint VBO;
};

Frustum createFrustumFromCamera(const Camera& cam, float aspect, float fovY, float zNear, float zFar);

struct TerrainChunk {
public:
	TerrainChunk();
	glm::vec3 ChunkPosLL;
	glm::vec3 ChunkDim;
	glm::ivec3 VoxelDim;
	VAOVBOPair Buffer;
	WireFrameCubeGL Wireframe;
	unsigned int NumVertices;
	bool IsInFrustum(const Frustum& f);
private:
	bool isOnOrForwardPlane(const Plane& plane) const;
	
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

