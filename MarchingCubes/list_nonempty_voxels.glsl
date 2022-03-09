#version 460 core
#extension GL_ARB_shader_bit_encoding:enable
#extension GL_ARB_compute_shader:enable
#extension GL_ARB_shader_storage_buffer_object:enable
#extension GL_ARB_shader_atomic_counter_ops:enable
#extension GL_ARB_shader_atomic_counters:enable
#define LOCAL_THREADS_DIMS 4
layout(local_size_x = LOCAL_THREADS_DIMS,local_size_y = LOCAL_THREADS_DIMS,local_size_z = LOCAL_THREADS_DIMS)in;

layout( packed, binding=3 ) buffer Values   // outputted values 
{
	uint nonemptyList[];
};
layout(binding = 0) uniform atomic_uint indexCounter;
layout(binding = 0) uniform atomic_uint vertexCounter;



uniform float IsoLevel;

layout(r32f,binding = 2) readonly uniform image3D densityTexture;

const uint NUMVERTICES[256] = {
        0 , 3 , 3 , 6 , 3 , 6 , 6 , 9 , 3 , 6 , 6 , 9 , 6 , 9 , 9 , 6 ,
        3 , 6 , 6 , 9 , 6 , 9 , 9 , 12, 6 , 9 , 9 , 12, 9 , 12, 12, 9 ,
        3 , 6 , 6 , 9 , 6 , 9 , 9 , 12, 6 , 9 , 9 , 12, 9 , 12, 12, 9 ,
        6 , 9 , 9 , 6 , 9 , 12, 12, 9 , 9 , 12, 12, 9 , 12, 15, 15, 6 ,
        3 , 6 , 6 , 9 , 6 , 9 , 9 , 12, 6 , 9 , 9 , 12, 9 , 12, 12, 9 ,
        6 , 9 , 9 , 12, 9 , 12, 12, 15, 9 , 12, 12, 15, 12, 15, 15, 12,
        6 , 9 , 9 , 12, 9 , 12, 6 , 9 , 9 , 12, 12, 15, 12, 15, 9 , 6 ,
        9 , 12, 12, 9 , 12, 15, 9 , 6 , 12, 15, 15, 12, 15, 6 , 12, 3 ,
        3 , 6 , 6 , 9 , 6 , 9 , 9 , 12, 6 , 9 , 9 , 12, 9 , 12, 12, 9 ,
        6 , 9 , 9 , 12, 9 , 12, 12, 15, 9 , 6 , 12, 9 , 12, 9 , 15, 6 ,
        6 , 9 , 9 , 12, 9 , 12, 12, 15, 9 , 12, 12, 15, 12, 15, 15, 12,
        9 , 12, 12, 9 , 12, 15, 15, 12, 12, 9 , 15, 6 , 15, 12, 6 , 3 ,
        6 , 9 , 9 , 12, 9 , 12, 12, 15, 9 , 12, 12, 15, 6 , 9 , 9 , 6 ,
        9 , 12, 12, 15, 12, 15, 15, 6 , 12, 9 , 15, 12, 9 , 6 , 12, 3 ,
        9 , 12, 12, 15, 12, 15, 9 , 12, 12, 15, 15, 6 , 9 , 12, 6 , 3 ,
        6 , 9 , 9 , 6 , 9 , 12, 6 , 3 , 9 , 6 , 12, 3 , 6 , 3 , 3 , 0
};

uint CalculateNumVerts(uint cubecase){
	return NUMVERTICES[cubecase];
}

void main(){
	/*
		Jim Marshall - 2021
		this shader is dispatched once per cube of 8 voxels ie 32*32*32.
		
		Checks which corners of the cube are within the iso-surface ie its marching cubes case.
		
		Uses this case to look up how many vertices the cube will produce, the sum of all cubes is outputted in vertexCounter 
		which tells the next stage what size buffer to allocate for the vertices
		
		Sets nonemptyList, a packed buffer of uints that are encoded with information for the next stage like this:
		
		[first byte] [second byte] [third byte] [fourth byte]
			^              ^             ^             ^ 
			|              |             |             |
			|              |             |       the cubes z coord within the chunk
			|              |       the cubes y coord within the chunk
			|          the cubes x coord within the chunk
		the cubes marching cubes case 
		
		It also sets indexCounter which counts how big the size of nonemptyList
		
		based on this thesis:
		https://www.diva-portal.org/smash/get/diva2:846354/FULLTEXT01.pdf?fbclid=IwAR3qPermz_ymU4k6VsorsN3Wab5ucaO6oFEC7WmxO_z2SlENboDHfbOxlpw
		Volumetric Terrain Genereation on the GPU 
		A MODERN GPGPU APPROACH TO MARCHING CUBES 
		LUDWIG PETHRUS ENGSTRÖM

		
	*/
	
	float bottom_back_left   = imageLoad(densityTexture,ivec3(gl_WorkGroupID*LOCAL_THREADS_DIMS + gl_LocalInvocationID + uvec3(0,0,0))).x;
	float bottom_front_left  = imageLoad(densityTexture,ivec3(gl_WorkGroupID*LOCAL_THREADS_DIMS + gl_LocalInvocationID + uvec3(0,0,1))).x;
	float top_back_left      = imageLoad(densityTexture,ivec3(gl_WorkGroupID*LOCAL_THREADS_DIMS + gl_LocalInvocationID + uvec3(0,1,0))).x;
	float top_front_left     = imageLoad(densityTexture,ivec3(gl_WorkGroupID*LOCAL_THREADS_DIMS + gl_LocalInvocationID + uvec3(0,1,1))).x;
	float bottom_back_right  = imageLoad(densityTexture,ivec3(gl_WorkGroupID*LOCAL_THREADS_DIMS + gl_LocalInvocationID + uvec3(1,0,0))).x;
	float bottom_front_right = imageLoad(densityTexture,ivec3(gl_WorkGroupID*LOCAL_THREADS_DIMS + gl_LocalInvocationID + uvec3(1,0,1))).x;
	float top_back_right     = imageLoad(densityTexture,ivec3(gl_WorkGroupID*LOCAL_THREADS_DIMS + gl_LocalInvocationID + uvec3(1,1,0))).x;
	float top_front_right    = imageLoad(densityTexture,ivec3(gl_WorkGroupID*LOCAL_THREADS_DIMS + gl_LocalInvocationID + uvec3(1,1,1))).x;
	
	uint cubecase;
	if(bottom_back_left > IsoLevel)   cubecase |= 1;
	if(bottom_back_right > IsoLevel)  cubecase |= 2;
	if(top_back_right > IsoLevel)     cubecase |= 4;
	if(top_back_left > IsoLevel)      cubecase |= 8;
	if(bottom_front_left > IsoLevel)  cubecase |= 16;
	if(bottom_front_right > IsoLevel) cubecase |= 32;
	if(top_front_right > IsoLevel)    cubecase |= 64;
	if(top_front_left > IsoLevel)     cubecase |= 128;
	
	if(cubecase > 0 && cubecase < 255){
		uint numverts = CalculateNumVerts(cubecase);
		atomicCounterAdd(vertexCounter, numverts);
		uint index = atomicCounterIncrement(indexCounter);
		uint z8_y8_x8_case8 = 
		((gl_WorkGroupID.z*LOCAL_THREADS_DIMS + gl_LocalInvocationID.z) << 24) | 
		((gl_WorkGroupID.y*LOCAL_THREADS_DIMS + gl_LocalInvocationID.y) << 16) | 
		((gl_WorkGroupID.x*LOCAL_THREADS_DIMS + gl_LocalInvocationID.x) << 8) | 
		cubecase;
		nonemptyList[index] = z8_y8_x8_case8;
		
	}
}