# Marching-cubes-cpp
- Multi-threaded implementation of the marching cubes algorithm in C++
- Generates meshes and renders them with Opengl
- Mostly fully working - there might be something wrong with the normals it generates
- relatively fast
- uses this MIT licenced thread pool implementation: https://github.com/bshoshany/thread-pool

- working GPGPU implementation included - still needs work, but runs about 5 or 6 times faster than the cpu implementation. Vertex buffers are generated on the gpu where they stay until rendering - Shader storage buffer objects used. intermediate stage creates list of non empty voxels and calculates number of vertices so that they can be outputted into a compact buffer. atomicAdd used to synchronise writes to the buffer accross multiple threads
- 
https://www.youtube.com/watch?v=_o1Ad-hlu7c
