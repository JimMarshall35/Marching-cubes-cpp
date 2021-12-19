#pragma once
#include "MetaBalls.h"
#include "OpenGLAdaptor.h"
#include "Stopwatch.h"

struct MovingMetaball {
	GUID id;
	glm::vec3 direction;
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
	static std::string march_timer_text;
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
	static void StartTimer(std::string name);
	static void StopTimer(std::string name);
	static void SetThreads(u32 numthreads);
	static u32 GetThreads();
	static u32 GetHardWareThreads();

	static void StopWatchesUpdateCallBack(double result, std::string timer_name);
	static void RegisterTimer(std::string name, duration d);
	static void RegisterTimer(std::string name, int d);
private:
	
private:
	static Stopwatch _stopwatch;
	static std::map<std::string, double> _timers_map;
};
