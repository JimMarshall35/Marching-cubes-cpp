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

Terrain::Terrain()
{
	_Stopwatch.RegisterTimer([](double val, std::string name) {
		std::cout << name << " : " << val << std::endl;
	}, "init", 1);
}

void Terrain::Render(const Camera& camera, int window_w, int window_h)
{
	for (TerrainChunk t : _Chunks) {
		if (t.NumVertices == 0) continue;
		glBindVertexArray(t.Buffer.VAO);
		glBindBuffer(GL_ARRAY_BUFFER, t.Buffer.VBO);
		Renderer::render(camera, t.Buffer.VAO, t.NumVertices);
		//t.Wireframe.Render(camera, window_w, window_h);
	}
}

void Terrain::Init()
{
	_Stopwatch.StartTimer("init");
	for (int x = 0; x < 13; x++) {
		for (int y = 0; y < 5; y++) {
			for (int z = 0; z < 13; z++) {
				TerrainChunk chunk;
				chunk.ChunkPosLL.x = (float)x;
				chunk.ChunkPosLL.y = -2.5f + (float)y;
				chunk.ChunkPosLL.z = (float)z;

				chunk.NumVertices = _Marcher.GenerateMesh(chunk.ChunkPosLL, glm::vec3(1.0, 1.0, 1.0), glm::ivec3(32, 32, 32), 0.0f, chunk.Buffer.VBO);
				chunk.Wireframe.SetDimsInitial(1.0, 1.0, 1.0, vec3(chunk.ChunkPosLL.x, chunk.ChunkPosLL.y, chunk.ChunkPosLL.z));
				_Chunks.push_back(chunk);
			}
		}
	}
	_Stopwatch.StopTimer("init");
}
