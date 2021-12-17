#include "ComputeShaderMarcher.h"
#include <string>
#include <iostream>
#include "ErrorHandling.h"


ComputeShaderMarcher::ComputeShaderMarcher()
{
	GLClearErrors();
	_Values = new float[33 * 33 * 33];
	_EvaluateVoxels.Load("EvalGrid.glsl");
	_GetNonEmptyVoxels.Load("list_nonempty_voxels.glsl");
	_GenerateVertices.Load("generate_vertices.glsl");
	glGenTextures(1, &_Texture);
	GLPrintErrors("glGenTextures");
	glBindTexture(GL_TEXTURE_3D, _Texture);
	GLPrintErrors("glBindTexture");
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F,33, 33, 33,0, GL_RED, GL_FLOAT, _Values);
	//glBindImageTexture(0, _Texture, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32F);

	GLPrintErrors("glTexImage3D");


	glBindTexture(GL_TEXTURE_3D, 0);


	// make intermediate ssbo
	glGenBuffers(1, &_IntermediateDataSSBO);
	GLPrintErrors("glGenBuffers");
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _IntermediateDataSSBO);
	GLPrintErrors("glBindBuffer");
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int)* ((32 * 32 * 32 * 15) + 2), NULL, GL_DYNAMIC_DRAW); //sizeof(data) only works for statically sized C/C++ arrays.
	GLPrintErrors("glBufferData");
	

	int blockIndex = glGetProgramResourceIndex(_GetNonEmptyVoxels.GetId(), GL_SHADER_STORAGE_BLOCK, "Values");
	GLPrintErrors("glGetProgramResourceIndex");
	if (blockIndex != GL_INVALID_INDEX) {
		glShaderStorageBlockBinding(_GetNonEmptyVoxels.GetId(), blockIndex, 3);
		GLPrintErrors("glShaderStorageBlockBinding");
	}
	else {
		std::cout << ("Warning: binding Values not found");
	}
	GLPrintErrors("glShaderStorageBlockBinding");
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind
	GLPrintErrors("glBindBuffer");

	// make vertex ssbo
	glGenBuffers(1, &_VertexSSBO);
	GLPrintErrors("glGenBuffers");
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _VertexSSBO);
	GLPrintErrors("glBindBuffer");
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int) * ((32 * 32 * 32 * 15) + 2), NULL, GL_DYNAMIC_DRAW); //sizeof(data) only works for statically sized C/C++ arrays.
	GLPrintErrors("glBufferData");


	int blockIndex2 = glGetProgramResourceIndex(_GenerateVertices.GetId(), GL_SHADER_STORAGE_BLOCK, "Vertices");
	GLPrintErrors("glGetProgramResourceIndex");
	if (blockIndex2 != GL_INVALID_INDEX) {
		glShaderStorageBlockBinding(_GenerateVertices.GetId(), blockIndex2, 2);
		GLPrintErrors("glShaderStorageBlockBinding");
	}
	else {
		std::cout << ("Warning: binding Values not found");
	}
	GLPrintErrors("glShaderStorageBlockBinding");
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind
	GLPrintErrors("glBindBuffer");

	// make vao




	_StopWatch.RegisterTimer([](double val, std::string name){
		std::cout << "timer " << name << " : " << val << std::endl;
		}, "Evaluate", 1);
}

ComputeShaderMarcher::~ComputeShaderMarcher()
{
	delete[] _Values;
}

void ComputeShaderMarcher::SetStartPos(const glm::vec3& start) const
{
	_EvaluateVoxels.SetVec3("StartPos", start);
}

void ComputeShaderMarcher::SetCellSize(const float size) const
{
	_EvaluateVoxels.SetFloat("CellSize",size);
}

void ComputeShaderMarcher::SetMetaBalls(const std::vector<MetaBall>& metaBalls)
{
	_EvaluateVoxels.SetInt("NumMetaballs", metaBalls.size());
	for (size_t i = 0; i < metaBalls.size(); i++) {
		const MetaBall& m = metaBalls[i];
		std::string position_name = "Metaballs[" + std::to_string(i) + "].position";
		std::string radius_name = "Metaballs[" + std::to_string(i) + "].radius"; 
		std::string phase_name = "Metaballs[" + std::to_string(i) + "].phase";
		_EvaluateVoxels.SetVec3(position_name.c_str(), m.position);
		_EvaluateVoxels.SetFloat(radius_name.c_str(), m.radius);
		_EvaluateVoxels.SetFloat(phase_name.c_str(), (m.phase == PHASE::NEGATIVE) ? -1.0f : 1.0f);
	}
}


void printStage3(GLuint vertexSSBO, GLuint vertex_counter) {
	float* vertex_vals = (float*)glMapNamedBuffer(vertexSSBO, GL_READ_ONLY);

	unsigned int vertCounter = 0;


	/*
	for (int i = 4; i < (vertex_counter * 8) + 1; i += 8) {
		std::cout << "vertex " << vertCounter << std::endl;
		std::cout << "pos: " << vertex_vals[i + 0] << ", " << vertex_vals[i + 1] << ", " << vertex_vals[i + 2] << std::endl;
		std::cout << "norm: " << vertex_vals[i + 4] << ", " << vertex_vals[i + 5] << ", " << vertex_vals[i + 6] << std::endl;
		vertCounter++;
		std::cout << std::endl;
	}
	*/

	int counter = 0;
	for (int i = 0; i < (vertex_counter * 8) + 1; i++) {
		std::cout << vertex_vals[i] << ", ";
		counter++;
		if (counter == 8) {
			counter = 0;
			std::cout << std::endl;
		}
	}


	std::cout << "vertexOutputCounter" << *((unsigned int*)vertex_vals) << std::endl;

	glUnmapNamedBuffer(vertexSSBO);
}
void printStage2(GLuint intermediateSSBO, bool drawslices = false) {
	// prints the stage 2 output as a list of non empty cells as well as an 'ascii art' representation of each slice through the chunk
	GLint bufMask = GL_MAP_READ_BIT; 
	unsigned int* vals = (unsigned int*)glMapNamedBuffer(intermediateSSBO, GL_READ_ONLY);
	unsigned int index_counter = vals[0];
	unsigned int vertex_counter = vals[1];
	std::cout << "indexCounter: " << index_counter << " vertexCounter: " << vertex_counter << std::endl;
	char slice[32][32][32];
	for(int i=0; i<32; i++)
		for(int j=0; j<32; j++)
			memset(slice[i][j], 'o', 32);

	for (size_t i = 2; i < index_counter; i++) {
		/* print the details of each non-empty cube and set a char in the 3d array so that it can be graphically represented */
		unsigned int raw = vals[i];
		unsigned int x, y, z, cubecase;
		cubecase = raw & 0x0000ff;
		x = (raw >> 8) & 0x0000ff;
		y = (raw >> 16) & 0x0000ff;
		z = (raw >> 24) & 0x0000ff;
		std::cout << "x: " << x << " y: " << y << " z: " << z << " case: " << cubecase << std::endl;
		slice[x][y][z] = 'x';
	}
	if (!drawslices) {
		glUnmapNamedBuffer(intermediateSSBO);
		return;
	}
	// draw the chunk slice by slice in the console using x's for a full cell and o's for empty cells
	std::cout << std::endl;
	for (int i = 0; i < 32; i++) {
		std::cout << "x = " << i << std::endl;
		for (int j = 0; j < 32; j++) {
			for (int k = 0; k < 32; k++) {
				std::cout << slice[i][j][k];
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
		std::cout << std::endl;
	}
	glUnmapNamedBuffer(intermediateSSBO);
}
void ComputeShaderMarcher::Evaluate(const std::vector<MetaBall>& metaballs, const glm::vec3& startpos, float cellsize)
{
	GLClearErrors(); 
	_StopWatch.StartTimer("Evaluate");

	/*
	* Stage 1- Populate the 3d texture with voxel values
	*/
	_EvaluateVoxels.Use();
	glBindImageTexture(2, _Texture, 0, GL_TRUE, NULL, GL_READ_WRITE, GL_R32F);
	SetCellSize(cellsize);
	SetMetaBalls(metaballs);
	SetStartPos(startpos);
	glDispatchCompute(33,33,33);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	/*
	* Stage 2 - Calculate the marching cube's case for each cube of 8 voxels, 
	* listing those that contain polygons and counting the no of vertices that will be produced
	*/
	_GetNonEmptyVoxels.Use();
	_GetNonEmptyVoxels.SetFloat("IsoLevel", 0.5f);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, _IntermediateDataSSBO);
	glDispatchCompute(32, 32, 32);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	//printStage2(_IntermediateDataSSBO, true);


	// which method of getting vertex_counter from gpu memory is faster?
	//unsigned int* vals = (unsigned int*)glMapNamedBuffer(_IntermediateDataSSBO, GL_READ_ONLY);
	//unsigned int vertex_counter = vals[1];
	//glUnmapNamedBuffer(_IntermediateDataSSBO);
	unsigned int vertex_counter;
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 4, 4, &vertex_counter);

	/*
	*  Stage 2.5 - allocate the buffer that vertices will go into
	*/
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _VertexSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int) +(vertex_counter* 8 * sizeof(float)), NULL, GL_DYNAMIC_DRAW);

	/*
	*  Stage 3 - generate the vertices - setting _VAO so it can be used to render 
	*/
	_GenerateVertices.Use();
	_GenerateVertices.SetInt("ChunkSize", 32);
	_GenerateVertices.SetFloat("CellSize", cellsize);
	_GenerateVertices.SetVec3("StartPos", startpos);
	_GenerateVertices.SetFloat("IsoLevel", 0.5f);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, _VertexSSBO);
	glDispatchCompute(vertex_counter,1,1);
	glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

	//printStage3(_VertexSSBO,vertex_counter);
	
	glGenVertexArrays(1, &_VAO);
	
	glBindVertexArray(_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, _VertexSSBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(4 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(1);
	
	

	glBindTexture(GL_TEXTURE_3D, 0);
	_StopWatch.StopTimer("Evaluate");
	
}

void ComputeShaderMarcher::StopwatchCallback(double value, std::string timerName)
{
	std::cout << "timer " << timerName << " : " << value << std::endl;
}
