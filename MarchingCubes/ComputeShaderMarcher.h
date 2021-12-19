#include <glad/glad.h>
#include "ComputeShader.h"
#include <vector>
#include "MetaBalls.h"
#include "Stopwatch.h"
class ComputeShaderMarcher {
public:
	ComputeShaderMarcher();
	~ComputeShaderMarcher();
	void SetStartPos(const glm::vec3& start) const;
	void SetCellSize(const float size) const;
	void SetMetaBalls(const std::vector<MetaBall>& metaBalls);

	unsigned int GenerateMesh(const glm::vec3& chunkPosLL, const glm::vec3& chunkDim, const glm::ivec3& voxelDim, float isolevel, GLuint VBO);
	/// <summary>
	/// create a mesh from some metaballs which can be rendered using GetVAO()
	/// </summary>
	/// <param name="metaballs">a list of metaball structs with position, radius, and phase</param>
	/// <param name="chunkPosLL">the back lower left corner of the chunk to be polygonized</param>
	/// <param name="chunkDim"></param>
	/// <param name="voxelDim"></param>
	void GenerateMesh(const std::vector<MetaBall>& metaballs, const glm::vec3& chunkPosLL, const glm::vec3& chunkDim, const glm::ivec3& voxelDim, float isoValue);
	
	void StopwatchCallback(double value, std::string timerName);
	inline unsigned int GetNumVertices() { return _NumVertices; }
	inline GLuint GetVAO() { return _VAO; }
private:
	Stopwatch _StopWatch;
	GLuint _IntermediateDataSSBO;
	GLuint _VertexSSBO;
	GLuint _VoxelValuesTexture;
	GLuint _VoxelValuesDifferentialTexture; // not used
	GLuint _RandomSeedTexture;
	GLuint _AtomicCountersBuffer;
	GLuint _IntermediateReadbackBuffer;

	ComputeShader _EvaluateVoxels;
	ComputeShader _GetNonEmptyVoxels;
	ComputeShader _GenerateVertices;
	GLuint _VAO;
	unsigned int _NumVertices;
	unsigned int* _IntermediateBufferMappedPtr;
};