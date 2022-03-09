
#version 460 core
#extension GL_ARB_shader_bit_encoding:enable
#extension GL_ARB_compute_shader:enable
#extension GL_ARB_shader_storage_buffer_object:enable

layout(local_size_x = 33,local_size_y = 1,local_size_z = 1)in;

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
layout(binding=0) uniform sampler3D SeedTexture0;
layout(binding=1) uniform sampler3D SeedTexture1;
layout(binding=2) uniform sampler3D SeedTexture2;
layout(binding=3) uniform sampler3D SeedTexture3;
layout(binding=4) uniform sampler3D SeedTexture4;
layout(binding=5) uniform sampler3D SeedTexture5;
layout(binding=6) uniform sampler3D SeedTexture6;
layout(binding=7) uniform sampler3D SeedTexture7;
layout(binding=8) uniform sampler3D SeedTexture8;

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
float GetValueAtPoint(in vec3 worldPos){
	float sum = -worldPos.y; 
	//float hard_floor_y = -13; density += saturate((hard_floor_y - ws_orig.y)*3)*40; 
	float hard_floor_y = -1;
	sum += clamp(((hard_floor_y - worldPos.y)*3),0.0,1.0)*40;
	vec3 warp = texture(SeedTexture8,worldPos *0.004).rgb;
	worldPos += warp*8;
	sum += texture(SeedTexture7, worldPos*2.01).r * 0.0312;
	sum += texture(SeedTexture6, worldPos*0.99).r * 0.0625;
	sum += texture(SeedTexture5, worldPos*0.51).r * 0.125;
	sum += texture(SeedTexture0, worldPos*0.25).r * 0.25;
	sum += texture(SeedTexture1, worldPos*0.123).r * 0.5;
	sum += texture(SeedTexture2, worldPos*0.063).r * 1.01;
	sum += texture(SeedTexture3, worldPos*0.0325).r * 2;
	sum += texture(SeedTexture4, worldPos*0.0165).r * 4;
	return sum;
}
void main(){
	vec3 globalIndex = vec3(gl_WorkGroupID); 
	// get world space position
	//vec3 worldPos = StartPos + (globalIndex * CellSize);
	vec3 worldPos = StartPos + vec3(gl_LocalInvocationID.x, globalIndex.y, globalIndex.z)*CellSize;
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
	float sum = GetValueAtPoint(worldPos);
	
	/*
	sum += texture(SeedTexture3, worldPos*0.4).r * 0.07;
	sum += texture(SeedTexture4, worldPos*0.4).r * 0.07;
	sum += texture(SeedTexture5, worldPos*0.4).r * 0.07;
	sum += texture(SeedTexture6, worldPos*0.4).r * 0.07;
	sum += texture(SeedTexture7, worldPos*0.4).r * 0.07;
	sum += texture(SeedTexture8, worldPos*0.4).r * 0.07;
	*/

	imageStore(densityTexture, ivec3(gl_LocalInvocationID.x, globalIndex.y, globalIndex.z), vec4(sum));
}