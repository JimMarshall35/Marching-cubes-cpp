#pragma once


#include <vector>


//#include "glm/glm.hpp"
#include <functional>
#include "vectors.h"
#include "thread_pool.h"
#define USE_THREAD_POOL
#define PRE_ALLOCATE_VECTOR
#define THREADS_TO_SKIP 3
#define MAX_VERTS_PER_CUBE 12

#include "Array3D.h"



typedef std::function<f64(vec3)> SurfaceFunc3D;

struct ValueAtPoint {
	vec3 point;
	f64 value;
};

struct GridCell {
	vec3 positions[8];
	f64 values[8];
	vec3 normals[8];
};

struct Vertex {
	Vertex(): pos(vec3()), normal(vec3()){}
	Vertex(vec3 pos, vec3 norm): pos(pos), normal(norm){}
	vec3 pos;
	vec3 normal;
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
public:
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

	vec3 _StartPoint;

	f32 _IsoLevel;

	Vertex VertexInterpolation(const GridCell& cell, u32 index1, u32 index2) const;
	void SingleWorkerMarch(ivec3 cube_grid_coords, u32 numcells, const SurfaceFunc3D& getValAtPoint);

	void SingleWorkerMarch(ivec3 cube_grid_coords, u32 numcells, const SurfaceFunc3D& getValAtPoint, Array3D<ValueAtPoint> arr);

	void SetGridCellNormals(GridCell& cell, const SurfaceFunc3D& f);
};

static class Table {
public:
	static const int EDGES[256];
	static const int TRIANGLES[256][16];
};





