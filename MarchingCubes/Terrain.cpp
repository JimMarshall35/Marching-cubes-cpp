#include "Terrain.h"

TerrainChunk::TerrainChunk()
{
	glGenBuffers(1, &Buffer.VBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Buffer.VBO);

	glGenVertexArrays(1, &Buffer.VAO);
	glBindVertexArray(Buffer.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, Buffer.VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(4 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

bool TerrainChunk::IsInFrustum(const Frustum& f)
{
	return
		isOnOrForwardPlane(f.leftFace) &&
		isOnOrForwardPlane(f.rightFace) &&
		isOnOrForwardPlane(f.topFace) &&
		isOnOrForwardPlane(f.bottomFace) &&
		isOnOrForwardPlane(f.nearFace) &&
		isOnOrForwardPlane(f.farFace);

}

bool TerrainChunk::isOnOrForwardPlane(const Plane& plane) const
{
	auto center = ChunkPosLL + (ChunkDim * 0.5f);
	float radius = glm::length(ChunkDim * 0.5f);
	return plane.getSignedDistanceToPlane(center) > -radius; 
}

Terrain::Terrain()
{
	_Stopwatch.RegisterTimer([](double val, std::string name) {
		std::cout << name << " : " << val << std::endl;
	}, "init", 1);
}

void Terrain::Render(const Camera& camera, int window_w, int window_h)
{
	Frustum f = createFrustumFromCamera(camera, (float)window_w / (float)window_h, glm::radians(45.0f), .1f, 100.0f);
	int verts_culled = 0;
	int chunks_culled = 0;
	for (TerrainChunk t : _Chunks) {
		if (t.NumVertices == 0) continue;
		if (t.IsInFrustum(f)) {
			glBindVertexArray(t.Buffer.VAO);
			glBindBuffer(GL_ARRAY_BUFFER, t.Buffer.VBO);
			Renderer::render(camera, t.Buffer.VAO, t.NumVertices);
			//t.Wireframe.Render(camera, window_w, window_h);
		}
		else {
			verts_culled += t.NumVertices;
			chunks_culled++;

		}
	}
	//std::cout << verts_culled << " vertices culled in "<< chunks_culled << " chunks" << std::endl;
}

void Terrain::Init()
{
	_Stopwatch.StartTimer("init");
	for (int x = 0; x < 30; x++) {
		for (int y = 0; y < 3; y++) {
			for (int z = 0; z < 30; z++) {
				TerrainChunk chunk;
				glm::vec3 chunkDim(4.0, 4.0, 4.0);
				chunk.ChunkPosLL.x = (float)x * chunkDim.x;
				chunk.ChunkPosLL.y = -1.2f + (float)y* chunkDim.y;
				chunk.ChunkPosLL.z = (float)z* chunkDim.z;
				chunk.ChunkDim = chunkDim;

				chunk.NumVertices = _Marcher.GenerateMesh(chunk.ChunkPosLL,chunkDim, glm::ivec3(32, 32, 32), 0.0f, chunk.Buffer.VBO);
				chunk.Wireframe.SetDimsInitial(1.0, 1.0, 1.0, vec3(chunk.ChunkPosLL.x, chunk.ChunkPosLL.y, chunk.ChunkPosLL.z));
				_Chunks.push_back(chunk);
			}
		}
	}
	_Stopwatch.StopTimer("init");
}

Frustum createFrustumFromCamera(const Camera& cam, float aspect, float fovY, float zNear, float zFar)
{
	Frustum     frustum;
	const float halfVSide = zFar * tanf(fovY * .5f);
	const float halfHSide = halfVSide * aspect;
	const glm::vec3 frontMultFar = zFar * cam.Front;

	frustum.nearFace = { cam.Position + zNear * cam.Front, cam.Front };
	frustum.farFace = { cam.Position + frontMultFar, -cam.Front };
	frustum.rightFace = { cam.Position, glm::cross(cam.Up, frontMultFar + cam.Right * halfHSide) };
	frustum.leftFace = { cam.Position, glm::cross(frontMultFar - cam.Right * halfHSide, cam.Up) };
	frustum.topFace = { cam.Position, glm::cross(cam.Right, frontMultFar - cam.Up * halfVSide) };
	frustum.bottomFace = { cam.Position, glm::cross(frontMultFar + cam.Up * halfVSide, cam.Right) };

	return frustum;
}

