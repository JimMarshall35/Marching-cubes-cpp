
#version 460 core
#extension GL_ARB_shader_bit_encoding:enable
#extension GL_ARB_compute_shader:enable
#extension GL_ARB_shader_storage_buffer_object:enable

layout(local_size_x = 1,local_size_y = 1,local_size_z = 1)in;

struct MetaBall{
	vec3 position;
	float radius;
	float phase;
};

#define MAX_METABALLS 20
uniform MetaBall Metaballs[MAX_METABALLS];
uniform vec3 CellSize;
uniform vec3 StartPos;
uniform int NumMetaballs;
layout(r32f,binding = 2) uniform image3D densityTexture;
//layout(r32f,binding = 1) uniform image3D seedTexture;
uniform sampler3D SeedTexture;
float GetValueAtPositionMetaballs(in vec3 worldPos){
	float sum = 0;
	for(int i=0; i<NumMetaballs; i++){
		MetaBall m = Metaballs[i];
		float distance = distance(worldPos, m.position);
		float radius_squared = m.radius * m.radius;
		float distance_squared = distance * distance;
		sum += m.phase*radius_squared / distance_squared;
	}
	return sum;
}

vec3 GetDifferential(in vec3 worldPos){
	float xMinus, xPlus, yMinus, yPlus, zMinus, zPlus;
	xMinus = GetValueAtPositionMetaballs(worldPos + vec3(-CellSize.x,0,0));
	xPlus = GetValueAtPositionMetaballs(worldPos + vec3(CellSize.x,0,0));
	yMinus = GetValueAtPositionMetaballs(worldPos + vec3(0,-CellSize.y,0));
	yPlus = GetValueAtPositionMetaballs(worldPos + vec3(0,CellSize.y,0));
	zMinus = GetValueAtPositionMetaballs(worldPos + vec3(0,0,-CellSize.z));
	zPlus = GetValueAtPositionMetaballs(worldPos + vec3(0,0,CellSize.z));
	return vec3(
		xMinus - xPlus,
		yMinus - yPlus,
		zMinus - zPlus
	);
}

void main(){
	vec3 globalIndex = vec3(gl_WorkGroupID); 
	// get world space position
	vec3 worldPos = StartPos + (globalIndex * CellSize);
	/*
	float sum = 0;
	for(int i=0; i<NumMetaballs; i++){
		MetaBall m = Metaballs[i];
		float distance = distance(worldPos, m.position);
		float radius_squared = m.radius * m.radius;
		float distance_squared = distance * distance;
		sum += m.phase*radius_squared / distance_squared;
	}
	*/
	float sum = -worldPos.y; 
	sum += texture(SeedTexture, worldPos*0.03).r * 0.7;
	sum += texture(SeedTexture, worldPos*0.04).r * 2;
	sum += texture(SeedTexture, worldPos*0.5).r * 0.07;

	imageStore(densityTexture, ivec3(gl_WorkGroupID), vec4(sum));
}