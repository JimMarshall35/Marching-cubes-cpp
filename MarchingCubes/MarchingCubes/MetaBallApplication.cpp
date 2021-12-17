#include "MetaBallApplication.h"

CubeMarcher MetaBallApplication::_Marcher;
std::vector<MovingMetaball> MetaBallApplication::_MovingMetaballs;
bool MetaBallApplication::paused = false;
i32 MetaBallApplication::number_to_spawn = 10;
f32 MetaBallApplication::cubes_iso_level = 0.3f;
bool MetaBallApplication::wireframe_mode = false;
std::string MetaBallApplication::march_timer_text = "";
Stopwatch MetaBallApplication::_stopwatch;
std::map<std::string, double> MetaBallApplication::_timers_map;

bool RandomBool() {
	return rand() % 2 == 1;
}

void MetaBallApplication::SpawnRandomMovingMetaballs()
{
	ClearMetaBalls();
	for (u32 i = 0; i < number_to_spawn; i++) {
		MovingMetaball m;
		f32 randx = ((f32)rand() / (f32)RAND_MAX) * 11;
		f32 randy = ((f32)rand() / (f32)RAND_MAX) * 11;
		f32 randz = ((f32)rand() / (f32)RAND_MAX) * 11;
		f64 randradius = 0.2 + ((f32)rand() / (f32)RAND_MAX) * 0.5f;
		PHASE phase = RandomBool() ? PHASE::POSITIVE : PHASE::NEGATIVE;

		m.id = MetaBalls::addMetaball(randradius, glm::vec3(5.5,5.5,5.5), PHASE::POSITIVE);
		m.direction = glm::normalize(glm::vec3(
			-1.0f + ((f32)rand() / (f32)RAND_MAX) * 2.0f,
			-1.0f + ((f32)rand() / (f32)RAND_MAX) * 2.0f,
			-1.0f + ((f32)rand() / (f32)RAND_MAX) * 2.0f
		));

		m.speed = ((f32)rand() / (f32)RAND_MAX) * 0.5f;
		_MovingMetaballs.push_back(m);
	}
}

void MetaBallApplication::MoveMetaBalls(f32 delta)
{
	if (paused)
		return;
	for (MovingMetaball m : _MovingMetaballs)
		MetaBalls::getPointer(m.id)->position += m.direction * m.speed * delta;
}

void MetaBallApplication::InitMarcher()
{
	_Marcher.SetCubeDims(0.2, 0.2, 0.2);
	_Marcher.SetGridDims(55, 55, 55);
	f64 varing = 0.5;
	_Marcher.SetIsoLevel(MetaBallApplication::cubes_iso_level);
	_Marcher.SetStartPoint(0, 0, 0);
}

void MetaBallApplication::March()
{
	_Marcher.March(MetaBalls::getValueAtPoint);
}

u64 MetaBallApplication::GetNumVertices()
{
	return _Marcher.GetNumVerts();
}

VAO_VBO_Pair MetaBallApplication::LoadVerticesIntial()
{
	return loadVerticesInitial(_Marcher);
}

void MetaBallApplication::UpdateVertices(GLuint vbo)
{
	updateVertices(vbo, _Marcher);
}

void MetaBallApplication::UpdateMarcherIso()
{
	_Marcher.SetIsoLevel(cubes_iso_level);
}

MetaBall* MetaBallApplication::GetMetaBallPtr(GUID id)
{
	return MetaBalls::getPointer(id);
}

void MetaBallApplication::StartTimer(std::string name)
{
	_stopwatch.StartTimer(name);
}

void MetaBallApplication::StopTimer(std::string name)
{	
	_stopwatch.StopTimer(name);
}

void MetaBallApplication::SetThreads(u32 numthreads)
{
	_Marcher.ThreadsToUse = numthreads;
}

u32 MetaBallApplication::GetThreads()
{
	return _Marcher.ThreadsToUse;
}

u32 MetaBallApplication::GetHardWareThreads()
{
	return _Marcher.GetHardwareThreads();
}

void MetaBallApplication::RegisterTimer(std::string name, duration d)
{
	_stopwatch.RegisterTimer(StopWatchesUpdateCallBack, name, d);
	_timers_map.emplace(std::pair<std::string, double>(name, 0.0));
}

void MetaBallApplication::RegisterTimer(std::string name, int d)
{
	_stopwatch.RegisterTimer(StopWatchesUpdateCallBack, name, d);
	_timers_map.emplace(std::pair<std::string, double>(name, 0.0));
}


#include <iostream>   // std::cout
#include <string>     // std::string, std::to_string


void MetaBallApplication::StopWatchesUpdateCallBack(double result, std::string timer_name)
{
	_timers_map[timer_name] = result;
	march_timer_text = "";
	for (auto [key, value] : _timers_map) {
		march_timer_text += key + std::to_string(value) + " ms\n";
	}
}

void MetaBallApplication::ClearMetaBalls()
{
	for (const MovingMetaball& m : _MovingMetaballs) {
		MetaBalls::removeMetaball(m.id);
	}
	_MovingMetaballs.clear();
}
