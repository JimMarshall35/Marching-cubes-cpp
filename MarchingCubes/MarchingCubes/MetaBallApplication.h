#pragma once
#include "MetaBalls.h"
#include "OpenGLAdaptor.h"

struct MovingMetaball {
	GUID id;
	vec3 direction;
	f32 speed;
	std::string phase_string = "positive";
};

enum class MOVEMENT_MODE{
	RANDOM_SPEEDS_AND_DIRECTIONS,
	COAGULATE
};
// provides openGL buffers to be rendered in main.cpp and contains the logic that generates them
static class MetaBallApplication
{
private:
	static CubeMarcher _Marcher;
	
private:
	static void ClearMetaBalls();
public:
	static bool paused;
	static bool wireframe_mode;
	static i32 number_to_spawn;
	static f32 cubes_iso_level;
	static std::vector<MovingMetaball> _MovingMetaballs;
	static MOVEMENT_MODE movement_mode;
public:
	static void SpawnRandomMovingMetaballs();
	static void MoveMetaBalls(f32 delta);
	static void InitMarcher();
	static void March();
	static u64 GetNumVertices();
	static VAO_VBO_Pair LoadVerticesIntial();
	static void UpdateVertices(GLuint vbo);
	static void UpdateMarcherIso();
	static MetaBall* GetMetaBallPtr(GUID id);
};

