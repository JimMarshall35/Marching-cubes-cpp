/*
		Jim Marshall - 2021
		this shader is dispatched once per cube of 8 voxels ie 32*32*32.
		
		consists of 8 invocations per work group 2*2*2
		
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
#version 460 core
#extension GL_ARB_shader_bit_encoding:enable
#extension GL_ARB_compute_shader:enable
#extension GL_ARB_shader_storage_buffer_object:enable
#extension GL_ARB_shader_atomic_counter_ops:enable
#extension GL_ARB_shader_atomic_counters:enable
#extension GL_ARB_arrays_of_arrays:enable

//layout(local_size_x = 1,local_size_y = 1,local_size_z = 1)in;
layout(local_size_x = 2,local_size_y = 2,local_size_z = 2)in;

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

const uint CornerBitTable[2][2][2] = uint[][][](uint[][](uint[](1,16),  // map gl_LocalInvocationID to a bit in cubecase  
														uint[](8,128)),
												uint[][](uint[](2,32),
														uint[](4,64)));

uint CalculateNumVerts(uint cubecase){
	return NUMVERTICES[cubecase];
}
shared uint cubecase;
void main(){

	float value = imageLoad(densityTexture,ivec3(gl_WorkGroupID + gl_LocalInvocationID)).x;
	// shared variable can't be initialised so we  must explicitly set or unset the bit and later mask off all but the first byte
	if(value > IsoLevel) atomicOr(cubecase,CornerBitTable[gl_LocalInvocationID.x][gl_LocalInvocationID.y][gl_LocalInvocationID.z]);
	else                 atomicAnd(cubecase,~CornerBitTable[gl_LocalInvocationID.x][gl_LocalInvocationID.y][gl_LocalInvocationID.z]);
	barrier();
	
	
	if(gl_LocalInvocationID == uvec3(0,0,0)){
		cubecase &= 0x000000ff;
		if(cubecase == 0 || cubecase == 255){
			return;
		}
		uint numverts = CalculateNumVerts(cubecase);
		atomicCounterAdd(vertexCounter, numverts);
		uint index = atomicCounterIncrement(indexCounter);
		uint z8_y8_x8_case8 = (gl_WorkGroupID.z << 24) | (gl_WorkGroupID.y << 16) | (gl_WorkGroupID.x << 8) | cubecase;
		nonemptyList[index] = z8_y8_x8_case8;
	}

}