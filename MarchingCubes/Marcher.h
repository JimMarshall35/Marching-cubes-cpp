#pragma once


#include <vector>


//#include "glm/glm.hpp"
#include "typedefs.h"
#include <functional>
#include "thread_pool.h"
#define USE_THREAD_POOL
#define PRE_ALLOCATE_VECTOR
#define THREADS_TO_SKIP 1
#define MAX_VERTS_PER_CUBE 12

#include "Array3D.h"
#include <glm/glm.hpp>



typedef std::function<f64(glm::vec3)> SurfaceFunc3D;

struct ValueAtPoint {
	glm::vec3 point;
	f64 value;
};

struct GridCell {
	glm::vec3 positions[8];
	f64 values[8];
	glm::vec3 normals[8];
	glm::ivec3 indices[8];
};

struct Vertex {
	Vertex(): pos(glm::vec3()), normal(glm::vec3()){}
	Vertex(glm::vec3 pos, glm::vec3 norm): pos(pos), normal(norm){}
	glm::vec3 pos;
	glm::vec3 normal;
};

class CubeMarcher {
public:
	CubeMarcher();
	~CubeMarcher();
	void SetGridDims(u32 cubes_width, u32 cubes_height, u32 cubes_depth);
	void SetCubeDims(f32 square_width, f32 square_height, f32 square_depth);
	void SetStartPoint(f32 x, f32 y, f32 z);
	void SetIsoLevel(f32 level);
	void March(const SurfaceFunc3D& getValAtPoint);
	u32 GetNumVerts() const;
	u32 GetGridSize() const { return _GridSize; }
	u32 GetHardwareThreads() { return _HardwareThreads; }
public:
	u32 ThreadsToUse;
	std::vector<Vertex> Vertices;
private:
	u32 _NumVerts;
	std::mutex _VerticesMtx;
	u32 _HardwareThreads;
	ThreadPool* _ThreadPool;
	inline void ResizeVectors();
	u32 _GridSize;
	struct {
		u32 w;
		u32 h;
		u32 d;
	} _GridDims;
	
	struct {
		f32 w;
		f32 h;
		f32 d;
	} _CubeDims;

	glm::vec3 _StartPoint;

	f32 _IsoLevel;

	Vertex VertexInterpolation(const GridCell& cell, u32 index1, u32 index2) const;
	void SingleWorkerMarch(glm::ivec3 cube_grid_coords, u32 numcells, const SurfaceFunc3D& getValAtPoint);

	void SingleWorkerMarch(glm::ivec3 cube_grid_coords, u32 numcells, const SurfaceFunc3D& getValAtPoint, Array3D<ValueAtPoint> arr);

	void SetGridCellNormals(GridCell& cell, const SurfaceFunc3D& f, Array3D<ValueAtPoint> arr);
};

static class Table {
public:
	static const int EDGES[256];
	static const int TRIANGLES[256][16];
};

void printNumVertsTable(void);
void printFlatGLSLTrisTable(void);



