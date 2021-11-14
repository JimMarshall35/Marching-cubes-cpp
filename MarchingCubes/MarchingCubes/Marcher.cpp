#pragma once
#include "Marcher.h"
#include <autodiff/forward/real.hpp>


void CubeMarcher::SetGridDims(u32 cubes_width, u32 cubes_height, u32 cubes_depth)
{
	_GridDims.w = cubes_width;
	_GridDims.h = cubes_height;
	_GridDims.d = cubes_depth;
	_GridSize = cubes_width * cubes_depth * cubes_height;
#ifdef PRE_ALLOCATE_VECTOR
	Vertices.resize(cubes_width * cubes_depth * cubes_height * MAX_VERTS_PER_CUBE);
#endif
}

void CubeMarcher::SetCubeDims(f32 cube_width, f32 cube_height, f32 cube_depth)
{
	_CubeDims.w = cube_width;
	_CubeDims.h = cube_height;
	_CubeDims.d = cube_depth;
}

void CubeMarcher::SetStartPoint(f32 x, f32 y, f32 z)
{
	_StartPoint.x = x;
	_StartPoint.y = y;
	_StartPoint.z = z;

	
}





CubeMarcher::CubeMarcher()
{
	_HardwareThreads = std::thread::hardware_concurrency();
#ifdef USE_THREAD_POOL
	_ThreadPool = new ThreadPool(_HardwareThreads - THREADS_TO_SKIP);
#endif // THREAD_POOL
}

CubeMarcher::~CubeMarcher()
{
#ifdef USE_THREAD_POOL
	delete _ThreadPool;
#endif // THREAD_POOL

}

void CubeMarcher::SetIsoLevel(f32 level)
{
	_IsoLevel = level;
}

#ifndef USE_THREAD_POOL
void CubeMarcher::March(const SurfaceFunc3D& getValAtPoint)
{
#ifdef PRE_ALLOCATE_VECTOR
	_NumVerts = 0;
#else
	Vertices.clear();
#endif
	SingleWorkerMarch(ivec3(0, 0, 0), _GridSize, getValAtPoint);
}


#endif // !THREAD_POOL


#ifdef USE_THREAD_POOL
void CubeMarcher::March(const SurfaceFunc3D& getValAtPoint)
{
#ifdef PRE_ALLOCATE_VECTOR
	_NumVerts = 0;
#else
	Vertices.clear();
#endif
	u32 threads_used = _HardwareThreads - THREADS_TO_SKIP;
	u32 remainder = _GridSize % threads_used;
	u32 cells_per_thread = _GridSize / threads_used;

	i32 zval = 0;
	i32 zincr = _GridDims.d / threads_used;
	std::vector<std::future<void>> futures;
	futures.resize(threads_used);
	for (u32 i = 0; i < threads_used; i++) {
		u32 numcells = cells_per_thread;
		if (i == threads_used - 1) {
			numcells += remainder * _GridDims.w * _GridDims.h;
		}

		futures[i] = _ThreadPool->enqueue([this, numcells, getValAtPoint, zval]() {
			SingleWorkerMarch(
				ivec3(0, 0, zval),
				numcells,
				getValAtPoint
			);
		});
		zval += zincr;
		
	}
	for (u32 i = 0; i < threads_used; i++) {
		futures[i].wait();
	}
}
#endif // THREADPOOL
inline void CubeMarcher::ResizeVectors()
{
}
u32 CubeMarcher::GetNumVerts() const
{
#ifdef PRE_ALLOCATE_VECTOR
	return _NumVerts; 
#else
	return Vertices.size();
#endif
}


void CubeMarcher::SingleWorkerMarch(ivec3 cube_grid_coords, u32 numcells, const SurfaceFunc3D& getValAtPoint) {
	vec3 cursor_pos = _StartPoint;
	Vertex vertList[MAX_VERTS_PER_CUBE];
	for (u32 i = 0; i < numcells; i++) {
		GridCell gridcell;
		cursor_pos = _StartPoint + vec3(
			cube_grid_coords.x * _CubeDims.w,
			cube_grid_coords.y * _CubeDims.h,
			cube_grid_coords.z * _CubeDims.d
		);
		
		gridcell.positions[0].x = cursor_pos.x;
		gridcell.positions[0].y = cursor_pos.y;
		gridcell.positions[0].z = cursor_pos.z;
		gridcell.values[0] = getValAtPoint(cursor_pos);

		gridcell.positions[1].x = cursor_pos.x + _CubeDims.w;
		gridcell.positions[1].y = cursor_pos.y;
		gridcell.positions[1].z = cursor_pos.z;
		gridcell.values[1] = getValAtPoint(gridcell.positions[1]);

		gridcell.positions[2].x = cursor_pos.x + _CubeDims.w;
		gridcell.positions[2].y = cursor_pos.y + _CubeDims.h;
		gridcell.positions[2].z = cursor_pos.z;
		gridcell.values[2] = getValAtPoint(gridcell.positions[2]);

		gridcell.positions[3].x = cursor_pos.x;
		gridcell.positions[3].y = cursor_pos.y + _CubeDims.h;
		gridcell.positions[3].z = cursor_pos.z;
		gridcell.values[3] = getValAtPoint(gridcell.positions[3]);

		gridcell.positions[4].x = cursor_pos.x;
		gridcell.positions[4].y = cursor_pos.y;
		gridcell.positions[4].z = cursor_pos.z + _CubeDims.d;
		gridcell.values[4] = getValAtPoint(gridcell.positions[4]);

		gridcell.positions[5].x = cursor_pos.x + _CubeDims.w;
		gridcell.positions[5].y = cursor_pos.y;
		gridcell.positions[5].z = cursor_pos.z + _CubeDims.d;
		gridcell.values[5] = getValAtPoint(gridcell.positions[5]);

		gridcell.positions[6].x = cursor_pos.x + _CubeDims.w;
		gridcell.positions[6].y = cursor_pos.y + _CubeDims.h;
		gridcell.positions[6].z = cursor_pos.z + _CubeDims.d;
		gridcell.values[6] = getValAtPoint(gridcell.positions[6]);

		gridcell.positions[7].x = cursor_pos.x;
		gridcell.positions[7].y = cursor_pos.y + _CubeDims.h;
		gridcell.positions[7].z = cursor_pos.z + _CubeDims.d;
		gridcell.values[7] = getValAtPoint(gridcell.positions[7]);
		
		cube_grid_coords.x++;
		if (cube_grid_coords.x >= _GridDims.w) {
			cube_grid_coords.x = 0;
			cube_grid_coords.y++;
			if (cube_grid_coords.y >= _GridDims.h) {
				cube_grid_coords.y = 0;
				cube_grid_coords.z++;
				if (cube_grid_coords.z >= _GridDims.d) {
					cube_grid_coords.z = 0;
				}
			}
		}
		u8 cubeindex = 0;

		if (gridcell.values[0] > _IsoLevel) cubeindex |= 1;
		if (gridcell.values[1] > _IsoLevel) cubeindex |= 2;
		if (gridcell.values[2] > _IsoLevel) cubeindex |= 4;
		if (gridcell.values[3] > _IsoLevel) cubeindex |= 8;
		if (gridcell.values[4] > _IsoLevel) cubeindex |= 16;
		if (gridcell.values[5] > _IsoLevel) cubeindex |= 32;
		if (gridcell.values[6] > _IsoLevel) cubeindex |= 64;
		if (gridcell.values[7] > _IsoLevel) cubeindex |= 128;

		if (Table::EDGES[cubeindex] == 0 || Table::EDGES[cubeindex] == 255) continue; // cell entirely within or outside of the isosurface

		SetGridCellNormals(gridcell, getValAtPoint);


		if (Table::EDGES[cubeindex] & 1)
			vertList[0] = VertexInterpolation(gridcell, 0, 1);
		if (Table::EDGES[cubeindex] & 2)
			vertList[1] = VertexInterpolation(gridcell, 1, 2);
		if (Table::EDGES[cubeindex] & 4)
			vertList[2] = VertexInterpolation(gridcell, 2, 3);
		if (Table::EDGES[cubeindex] & 8)
			vertList[3] = VertexInterpolation(gridcell, 3, 0);
		if (Table::EDGES[cubeindex] & 16)
			vertList[4] = VertexInterpolation(gridcell, 4, 5);
		if (Table::EDGES[cubeindex] & 32)
			vertList[5] = VertexInterpolation(gridcell, 5, 6);
		if (Table::EDGES[cubeindex] & 64)
			vertList[6] = VertexInterpolation(gridcell, 6, 7);
		if (Table::EDGES[cubeindex] & 128)
			vertList[7] = VertexInterpolation(gridcell, 7, 4);
		if (Table::EDGES[cubeindex] & 256)
			vertList[8] = VertexInterpolation(gridcell, 0, 4);
		if (Table::EDGES[cubeindex] & 512)
			vertList[9] = VertexInterpolation(gridcell, 1, 5);
		if (Table::EDGES[cubeindex] & 1024)
			vertList[10] = VertexInterpolation(gridcell, 2, 6);
		if (Table::EDGES[cubeindex] & 2048)
			vertList[11] = VertexInterpolation(gridcell, 3, 7);

		for (int j = 0; Table::TRIANGLES[cubeindex][j] != -1; j += 3) {
			_VerticesMtx.lock();
#ifdef PRE_ALLOCATE_VECTOR
			Vertices[_NumVerts++] = vertList[Table::TRIANGLES[cubeindex][j + 2]];
			Vertices[_NumVerts++] = vertList[Table::TRIANGLES[cubeindex][j + 1]];
			Vertices[_NumVerts++] = vertList[Table::TRIANGLES[cubeindex][j]];
#else
			Vertices.emplace_back(vertList[Table::TRIANGLES[cubeindex][j + 2]]);
			Vertices.emplace_back(vertList[Table::TRIANGLES[cubeindex][j + 1]]);
			Vertices.emplace_back(vertList[Table::TRIANGLES[cubeindex][j]]);
#endif
			_VerticesMtx.unlock();
		}
	};
}
void CubeMarcher::SetGridCellNormals(GridCell& cell, const SurfaceFunc3D& f)
{
	/**/
	for (u32 i = 0; i < 8; i++) {
		cell.normals[i].x = f(cell.positions[i] + vec3(-_GridDims.w, 0, 0)) - f(cell.positions[i] + vec3(_GridDims.w, 0, 0));
		cell.normals[i].y = f(cell.positions[i] + vec3(0, -_GridDims.h, 0)) - f(cell.positions[i] + vec3(0, _GridDims.h, 0));
		cell.normals[i].z = f(cell.positions[i] + vec3(0, 0, -_GridDims.d)) - f(cell.positions[i] + vec3(0, 0, _GridDims.w));
		cell.normals[i] = cell.normals[i].normalize() ;
	}
	/*
		Grid[cx,cy,cz].Normal.X := (Grid[cx-1, cy, cz].Value-Grid[cx+1, cy, cz].Value)
		Grid[cx,cy,cz].Normal.Y := (Grid[cx, cy-1, cz].Value-Grid[cx, cy+1, cz].Value)
		Grid[cx,cy,cz].Normal.Z := (Grid[cx, cy, cz-1].Value-Grid[cx, cy, cz+1].Value)

	using namespace autodiff;

	auto wrapper = [f](real x, real y, real z) {
		real result = f(vec3((f32)x, (f32)y, (f32)z));
		return result;
	};
	for (u32 i = 0; i < 8; i++) {
		real x = cell.positions[i].x;
		real y = cell.positions[i].y;
		real z = cell.positions[i].z;

		cell.normals[i].x = derivative(wrapper, wrt(x), at(x,y,z));   // wrt == "with respect to" 
		cell.normals[i].y = derivative(wrapper, wrt(y), at(x, y, z));
		cell.normals[i].z = derivative(wrapper, wrt(z), at(x, y, z));
		cell.normals[i] = cell.normals[i].normalize();

	}
	*/
}


Vertex CubeMarcher::VertexInterpolation(const GridCell& cell, u32 index1, u32 index2) const
{
	vec3 n(0.0);
	vec3 p(0.0);

	f64 valp1 = cell.values[index1];
	f64 valp2 = cell.values[index2];

	vec3 p1 = cell.positions[index1];
	vec3 p2 = cell.positions[index2];

	vec3 n1 = cell.normals[index1];
	vec3 n2 = cell.normals[index2];

	f32 mu = (_IsoLevel - valp1) / (valp2 - valp1);
	p.x = p1.x + mu * (p2.x - p1.x);
	p.y = p1.y + mu * (p2.y - p1.y);
	p.z = p1.z + mu * (p2.z - p1.z);

	n.x = n1.x + mu * (n2.x - n1.x);
	n.y = n1.y + mu * (n2.y - n1.z);
	n.z = n1.z + mu * (n2.y - n1.z);

	return Vertex(p,n);

}