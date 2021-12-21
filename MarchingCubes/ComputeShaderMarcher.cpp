#include "ComputeShaderMarcher.h"
#include <string>
#include <iostream>
#include "ErrorHandling.h"
#define HI 1.0f
#define LO -1.0f
//#define TIMER_MODE

GLuint Generate16by16by16RandomTexture() {
	float* data = new float[16 * 16 * 16];
	for (int i = 0; i < 16 * 16 * 16; i++) {
		data[i] = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
	}
	GLuint texture;
	glGenTextures(1, &texture);
	GLPrintErrors("glGenTextures");
	glBindTexture(GL_TEXTURE_3D, texture);
	GLPrintErrors("glBindTexture");
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, 16, 16, 16, 0, GL_RED, GL_FLOAT, data);
	glBindTexture(GL_TEXTURE_3D, 0);
	return texture;
}

ComputeShaderMarcher::ComputeShaderMarcher()
{
	GLClearErrors();
	_EvaluateVoxels.Load("EvalGrid.glsl");
	_GetNonEmptyVoxels.Load("list_nonempty_voxels_v2.glsl");
	_GenerateVertices.Load("generate_vertices_v2.glsl");
	glGenTextures(1, &_VoxelValuesTexture);
	GLPrintErrors("glGenTextures");
	glBindTexture(GL_TEXTURE_3D, _VoxelValuesTexture);
	GLPrintErrors("glBindTexture");
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F,33, 33, 33,0, GL_RED, GL_FLOAT, NULL);
	GLPrintErrors("glTexImage3D");
	glBindTexture(GL_TEXTURE_3D, 0);
	_RandomSeedTexture = Generate16by16by16RandomTexture();


	// make intermediate ssbo
	glGenBuffers(1, &_IntermediateDataSSBO);
	GLPrintErrors("glGenBuffers");
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _IntermediateDataSSBO);
	GLPrintErrors("glBindBuffer");
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int) * ((32 * 32 * 32)), NULL,
		GL_MAP_READ_BIT);

	// make atomic counters buffer
	glGenBuffers(1, &_AtomicCountersBuffer);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, _AtomicCountersBuffer);
	glBufferStorage(GL_ATOMIC_COUNTER_BUFFER, 2 * sizeof(unsigned int), NULL, 
		GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);

	//glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int)* ((32 * 32 * 32 * 15) + 2), NULL, GL_DYNAMIC_DRAW); //sizeof(data) only works for statically sized C/C++ arrays.
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
	//glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int) * ((32 * 32 * 32 * 15) + 2), NULL, GL_DYNAMIC_DRAW); //sizeof(data) only works for statically sized C/C++ arrays.
	
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
	glGenVertexArrays(1, &_VAO);

	glBindVertexArray(_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, _VertexSSBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(4 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(1);




	_StopWatch.RegisterTimer([](double val, std::string name){
		std::cout << "timer " << name << " : " << val << std::endl;
		}, "Evaluate", 100);
	_StopWatch.RegisterTimer([](double val, std::string name) {
		std::cout << "timer " << name << " : " << val << std::endl;
		}, "stage1", 1);
	_StopWatch.RegisterTimer([](double val, std::string name) {
		std::cout << "timer " << name << " : " << val << std::endl;
		}, "stage2", 1);
	_StopWatch.RegisterTimer([](double val, std::string name) {
		std::cout << "timer " << name << " : " << val << std::endl;
		}, "stage3", 1);
	_StopWatch.RegisterTimer([](double val, std::string name) {
		std::cout << "timer " << name << " : " << val << std::endl;
		}, "getvertexcounter", 1);
	_StopWatch.RegisterTimer([](double val, std::string name) {
		std::cout << "timer " << name << " : " << val << std::endl;
		}, "allocatevertbuffer", 1);
}

ComputeShaderMarcher::~ComputeShaderMarcher()
{
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
		/**/
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


void ComputeShaderMarcher::StopwatchCallback(double value, std::string timerName)
{
	std::cout << "timer " << timerName << " : " << value << std::endl;
}


void ComputeShaderMarcher::GenerateMesh(const std::vector<MetaBall>& metaballs, const glm::vec3& chunkPosLL, const glm::vec3& chunkDim, const glm::ivec3& voxelDim, float isoValue)
{
	glm::vec3 voxelCubeDims = glm::vec3(
		chunkDim.x / (float)voxelDim.x,
		chunkDim.y / (float)voxelDim.y,
		chunkDim.z / (float)voxelDim.z
	);
	GLClearErrors();
	_StopWatch.StartTimer("Evaluate");
#ifdef TIMER_MODE
	_StopWatch.StartTimer("stage1");
#endif
	/*
	* Stage 1- Populate the 3d texture with voxel values
	*/
	_EvaluateVoxels.Use();
	glActiveTexture(GL_TEXTURE0);
	GLPrintErrors("glActiveTexture(GL_TEXTURE0);");
	glBindTexture(GL_TEXTURE_3D, _RandomSeedTexture);
	glBindImageTexture(2, _VoxelValuesTexture, 0, GL_TRUE, NULL, GL_READ_WRITE, GL_R32F);
	_EvaluateVoxels.SetVec3("CellSize", voxelCubeDims);
	SetMetaBalls(metaballs);
	_EvaluateVoxels.SetVec3("StartPos", chunkPosLL);
	glDispatchCompute(voxelDim.x + 1, voxelDim.y + 1, voxelDim.z + 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
#ifdef TIMER_MODE
	_StopWatch.StopTimer("stage1");
	_StopWatch.StartTimer("stage2");
#endif
	/*
	* Stage 2 - Calculate the marching cube's case for each cube of 8 voxels,
	* listing those that contain polygons and counting the no of vertices that will be produced
	*/
	_GetNonEmptyVoxels.Use();
	_GetNonEmptyVoxels.SetFloat("IsoLevel", isoValue);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, _IntermediateDataSSBO);
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, _AtomicCountersBuffer);
	glDispatchCompute(voxelDim.x, voxelDim.y, voxelDim.z);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);
	//printStage2(_IntermediateDataSSBO, true);
#ifdef TIMER_MODE
	_StopWatch.StopTimer("stage2");
	_StopWatch.StartTimer("getvertexcounter");
#endif

	unsigned int* vals = (unsigned int*)glMapNamedBuffer(_AtomicCountersBuffer, GL_READ_WRITE);
	unsigned int vertex_counter = vals[1];
	unsigned int index_counter = vals[0];
	vals[0] = 0;
	vals[1] = 0;
	glUnmapNamedBuffer(_AtomicCountersBuffer);
	
	//unsigned int vertex_counter;
	//glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 4, 4, &vertex_counter);
#ifdef TIMER_MODE
	_StopWatch.StopTimer("getvertexcounter");
	_StopWatch.StartTimer("allocatevertbuffer");
#endif
	/*
	*  Stage 2.5 - allocate the buffer that vertices will go into
	*/
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _VertexSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int) + (vertex_counter * 8 * sizeof(float)), NULL, GL_DYNAMIC_DRAW);
#ifdef TIMER_MODE
	_StopWatch.StopTimer("allocatevertbuffer");
	_StopWatch.StartTimer("stage3");
#endif
	/*
	*  Stage 3 - generate the vertices - setting _VAO so it can be used to render
	*/
	_GenerateVertices.Use();
	_GenerateVertices.SetIvec3("ChunkSize", voxelDim);
	_GenerateVertices.SetVec3("CellSize", voxelCubeDims);
	_GenerateVertices.SetVec3("StartPos", chunkPosLL);
	_GenerateVertices.SetFloat("IsoLevel", isoValue);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, _VertexSSBO);
	glDispatchCompute(index_counter, 1, 1);
	glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

	//printStage3(_VertexSSBO,vertex_counter);

#ifdef TIMER_MODE
	_StopWatch.StopTimer("stage3");
#endif

	_NumVertices = vertex_counter;

	glBindTexture(GL_TEXTURE_3D, 0);
	_StopWatch.StopTimer("Evaluate");
}


unsigned int ComputeShaderMarcher::GenerateMesh(
	const glm::vec3& chunkPosLL, 
	const glm::vec3& chunkDim, 
	const glm::ivec3& voxelDim, 
	float isoValue, GLuint VBO)
{
	glm::vec3 voxelCubeDims = glm::vec3(
		chunkDim.x / (float)voxelDim.x,
		chunkDim.y / (float)voxelDim.y,
		chunkDim.z / (float)voxelDim.z
	);
	GLClearErrors();
	_StopWatch.StartTimer("Evaluate");
#ifdef TIMER_MODE
	_StopWatch.StartTimer("stage1");
#endif
	/*
	* Stage 1- Populate the 3d texture with voxel values
	*/
	_EvaluateVoxels.Use();
	glActiveTexture(GL_TEXTURE0);
	GLPrintErrors("glActiveTexture(GL_TEXTURE0);");
	glBindTexture(GL_TEXTURE_3D, _RandomSeedTexture);
	glBindImageTexture(2, _VoxelValuesTexture, 0, GL_TRUE, NULL, GL_READ_WRITE, GL_R32F);
	_EvaluateVoxels.SetVec3("CellSize", voxelCubeDims);
	_EvaluateVoxels.SetVec3("StartPos", chunkPosLL);
	glDispatchCompute(voxelDim.x + 1, voxelDim.y + 1, voxelDim.z + 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
#ifdef TIMER_MODE
	_StopWatch.StopTimer("stage1");
	_StopWatch.StartTimer("stage2");
#endif
	/*
	* Stage 2 - Calculate the marching cube's case for each cube of 8 voxels,
	* listing those that contain polygons and counting the no of vertices that will be produced
	*/
	_GetNonEmptyVoxels.Use();
	_GetNonEmptyVoxels.SetFloat("IsoLevel", isoValue);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, _IntermediateDataSSBO);
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, _AtomicCountersBuffer);
	glDispatchCompute(voxelDim.x, voxelDim.y, voxelDim.z);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);
	//printStage2(_IntermediateDataSSBO, true);
#ifdef TIMER_MODE
	_StopWatch.StopTimer("stage2");
	_StopWatch.StartTimer("getvertexcounter");
#endif
	// which method of getting vertex_counter from gpu memory is faster?

	/*
	unsigned int* vals = (unsigned int*)glMapNamedBufferRange(_IntermediateDataSSBO,0,2*sizeof(unsigned int),GL_MAP_READ_BIT);
	unsigned int vertex_counter = vals[1];
	unsigned int index_counter = vals[0];
	glUnmapNamedBuffer(_IntermediateDataSSBO);
	*/
	//GL_MAP_UNSYNCHRONIZED_BIT
	unsigned int* vals = (unsigned int*)glMapNamedBufferRange(_AtomicCountersBuffer, 0, 2*sizeof(unsigned int),GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
	unsigned int vertex_counter = vals[1];
	unsigned int index_counter = vals[0];
	vals[0] = 0;
	vals[1] = 0;
	glUnmapNamedBuffer(_AtomicCountersBuffer);

	//unsigned int vertex_counter;
	//glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 4, 4, &vertex_counter);
#ifdef TIMER_MODE
	_StopWatch.StopTimer("getvertexcounter");
	_StopWatch.StartTimer("allocatevertbuffer");
#endif
	/*
	*  Stage 2.5 - allocate the buffer that vertices will go into
	*/
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, VBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int) + (vertex_counter * 8 * sizeof(float)), NULL, GL_DYNAMIC_DRAW);
#ifdef TIMER_MODE
	_StopWatch.StopTimer("allocatevertbuffer");
	_StopWatch.StartTimer("stage3");
#endif
	/*
	*  Stage 3 - generate the vertices - setting _VAO so it can be used to render
	*/
	_GenerateVertices.Use();
	_GenerateVertices.SetIvec3("ChunkSize", voxelDim);
	_GenerateVertices.SetVec3("CellSize", voxelCubeDims);
	_GenerateVertices.SetVec3("StartPos", chunkPosLL);
	_GenerateVertices.SetFloat("IsoLevel", isoValue);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, VBO);
	glDispatchCompute(index_counter, 1, 1);
	glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

	//printStage3(_VertexSSBO,vertex_counter);



	//unsigned int pxl = 0;
	//glClearNamedBufferSubData(_IntermediateDataSSBO, GL_R32UI, 0, 2 * sizeof(unsigned int), GL_RED, GL_FLOAT, &pxl);
#ifdef TIMER_MODE
	_StopWatch.StopTimer("stage3");
#endif

	

	glBindTexture(GL_TEXTURE_3D, 0);
	_StopWatch.StopTimer("Evaluate");
	return vertex_counter;
}

