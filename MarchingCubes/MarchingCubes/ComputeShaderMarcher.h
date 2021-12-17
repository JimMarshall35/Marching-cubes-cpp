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
	void Evaluate(const std::vector<MetaBall>& metaballs, const glm::vec3& startpos, float cellsize);
	void StopwatchCallback(double value, std::string timerName);
	inline unsigned int GetNumVertices() { return _NumVertices; }
	inline GLuint GetVAO() { return _VAO; }
private:
	Stopwatch _StopWatch;
	float* _Values;
	GLuint _IntermediateDataSSBO;
	GLuint _VertexSSBO;
	GLuint _Texture;
	ComputeShader _EvaluateVoxels;
	ComputeShader _GetNonEmptyVoxels;
	ComputeShader _GenerateVertices;
	GLuint _VAO;
	unsigned int _NumVertices;
};