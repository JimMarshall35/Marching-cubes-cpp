#include <iterator>
#include <algorithm>
#include "MetaBalls.h"

std::vector<MetaBall> MetaBalls::_MetaBalls;
GUID MetaBalls::addMetaball(f64 radius, vec3 pos, PHASE phase)
{
	MetaBall m;
	m.radius = radius;
	m.position = pos;
	m.phase = phase;
	CoCreateGuid(&m.id);
	_MetaBalls.emplace_back(m);
	return m.id;
}

void MetaBalls::removeMetaball(GUID id)
{
	_MetaBalls.erase(
		std::find_if(begin(_MetaBalls), end(_MetaBalls), [id](const MetaBall& m) {
			return m.id == id;
			})
	);
}
#define CUTOFF_DISTANCE 3
f64 MetaBalls::getValueAtPoint(vec3 point)
{
	f64 sum = 0.0;
	for (MetaBall m : _MetaBalls) {
		f32 distance = vec3::distance(point, m.position);
		//if (distance > CUTOFF_DISTANCE) 
			//continue;
		f32 radius_squared = m.radius * m.radius;
		f32 distance_squared = distance * distance;
		sum += ((f32)m.phase)*radius_squared / distance_squared;
	}
	return abs(sum);
}

MetaBall* MetaBalls::getPointer(GUID id)
{
	auto it = std::find_if(begin(_MetaBalls), end(_MetaBalls), [id](const MetaBall& m) {
		return m.id == id;
	});
	return &_MetaBalls[it - _MetaBalls.begin()];
}
