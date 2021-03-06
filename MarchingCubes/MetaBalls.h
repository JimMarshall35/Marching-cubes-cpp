#pragma once
#include <vector>
#include "typedefs.h"
#include <Rpc.h>
#include <glm/glm.hpp>

enum class PHASE {
	POSITIVE = 1,
	NEGATIVE = -1
};
struct MetaBall {
	f32 radius;
	glm::vec3 position;
	GUID id;
	PHASE phase;
};
static class MetaBalls {
private:
	
	
public:
	static std::vector<MetaBall> _MetaBalls;
	static GUID addMetaball(f64 radius, glm::vec3 pos, PHASE phase); // add a metaball, returning a GUID handle to it
	static void removeMetaball(GUID id);           // remove a metaball
	static f64 getValueAtPoint(glm::vec3 point);        // pass this function to CubeMarcher.March to polygonise the meta balls
	static MetaBall* getPointer(GUID id);          // get pointer to change metball that already exists
};
