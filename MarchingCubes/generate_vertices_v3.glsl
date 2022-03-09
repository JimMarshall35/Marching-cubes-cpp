#version 460 core
#extension GL_ARB_shader_bit_encoding:enable
#extension GL_ARB_compute_shader:enable
#extension GL_ARB_shader_storage_buffer_object:enable
#define NUM_LOCAL_THREADS 32
layout(local_size_x = NUM_LOCAL_THREADS,local_size_y = 1,local_size_z = 1)in;

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

const int TRIANGLES[4096] = {
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        0 , 8 , 3 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        0 , 1 , 9 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        1 , 8 , 3 , 9 , 8 , 1 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        1 , 2 , 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        0 , 8 , 3 , 1 , 2 , 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        9 , 2 , 10, 0 , 2 , 9 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        2 , 8 , 3 , 2 , 10, 8 , 10, 9 , 8 , -1, -1, -1, -1, -1, -1, -1,
        3 , 11, 2 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        0 , 11, 2 , 8 , 11, 0 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        1 , 9 , 0 , 2 , 3 , 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        1 , 11, 2 , 1 , 9 , 11, 9 , 8 , 11, -1, -1, -1, -1, -1, -1, -1,
        3 , 10, 1 , 11, 10, 3 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        0 , 10, 1 , 0 , 8 , 10, 8 , 11, 10, -1, -1, -1, -1, -1, -1, -1,
        3 , 9 , 0 , 3 , 11, 9 , 11, 10, 9 , -1, -1, -1, -1, -1, -1, -1,
        9 , 8 , 10, 10, 8 , 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        4 , 7 , 8 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        4 , 3 , 0 , 7 , 3 , 4 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        0 , 1 , 9 , 8 , 4 , 7 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        4 , 1 , 9 , 4 , 7 , 1 , 7 , 3 , 1 , -1, -1, -1, -1, -1, -1, -1,
        1 , 2 , 10, 8 , 4 , 7 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        3 , 4 , 7 , 3 , 0 , 4 , 1 , 2 , 10, -1, -1, -1, -1, -1, -1, -1,
        9 , 2 , 10, 9 , 0 , 2 , 8 , 4 , 7 , -1, -1, -1, -1, -1, -1, -1,
        2 , 10, 9 , 2 , 9 , 7 , 2 , 7 , 3 , 7 , 9 , 4 , -1, -1, -1, -1,
        8 , 4 , 7 , 3 , 11, 2 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        11, 4 , 7 , 11, 2 , 4 , 2 , 0 , 4 , -1, -1, -1, -1, -1, -1, -1,
        9 , 0 , 1 , 8 , 4 , 7 , 2 , 3 , 11, -1, -1, -1, -1, -1, -1, -1,
        4 , 7 , 11, 9 , 4 , 11, 9 , 11, 2 , 9 , 2 , 1 , -1, -1, -1, -1,
        3 , 10, 1 , 3 , 11, 10, 7 , 8 , 4 , -1, -1, -1, -1, -1, -1, -1,
        1 , 11, 10, 1 , 4 , 11, 1 , 0 , 4 , 7 , 11, 4 , -1, -1, -1, -1,
        4 , 7 , 8 , 9 , 0 , 11, 9 , 11, 10, 11, 0 , 3 , -1, -1, -1, -1,
        4 , 7 , 11, 4 , 11, 9 , 9 , 11, 10, -1, -1, -1, -1, -1, -1, -1,
        9 , 5 , 4 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        9 , 5 , 4 , 0 , 8 , 3 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        0 , 5 , 4 , 1 , 5 , 0 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        8 , 5 , 4 , 8 , 3 , 5 , 3 , 1 , 5 , -1, -1, -1, -1, -1, -1, -1,
        1 , 2 , 10, 9 , 5 , 4 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        3 , 0 , 8 , 1 , 2 , 10, 4 , 9 , 5 , -1, -1, -1, -1, -1, -1, -1,
        5 , 2 , 10, 5 , 4 , 2 , 4 , 0 , 2 , -1, -1, -1, -1, -1, -1, -1,
        2 , 10, 5 , 3 , 2 , 5 , 3 , 5 , 4 , 3 , 4 , 8 , -1, -1, -1, -1,
        9 , 5 , 4 , 2 , 3 , 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        0 , 11, 2 , 0 , 8 , 11, 4 , 9 , 5 , -1, -1, -1, -1, -1, -1, -1,
        0 , 5 , 4 , 0 , 1 , 5 , 2 , 3 , 11, -1, -1, -1, -1, -1, -1, -1,
        2 , 1 , 5 , 2 , 5 , 8 , 2 , 8 , 11, 4 , 8 , 5 , -1, -1, -1, -1,
        10, 3 , 11, 10, 1 , 3 , 9 , 5 , 4 , -1, -1, -1, -1, -1, -1, -1,
        4 , 9 , 5 , 0 , 8 , 1 , 8 , 10, 1 , 8 , 11, 10, -1, -1, -1, -1,
        5 , 4 , 0 , 5 , 0 , 11, 5 , 11, 10, 11, 0 , 3 , -1, -1, -1, -1,
        5 , 4 , 8 , 5 , 8 , 10, 10, 8 , 11, -1, -1, -1, -1, -1, -1, -1,
        9 , 7 , 8 , 5 , 7 , 9 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        9 , 3 , 0 , 9 , 5 , 3 , 5 , 7 , 3 , -1, -1, -1, -1, -1, -1, -1,
        0 , 7 , 8 , 0 , 1 , 7 , 1 , 5 , 7 , -1, -1, -1, -1, -1, -1, -1,
        1 , 5 , 3 , 3 , 5 , 7 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        9 , 7 , 8 , 9 , 5 , 7 , 10, 1 , 2 , -1, -1, -1, -1, -1, -1, -1,
        10, 1 , 2 , 9 , 5 , 0 , 5 , 3 , 0 , 5 , 7 , 3 , -1, -1, -1, -1,
        8 , 0 , 2 , 8 , 2 , 5 , 8 , 5 , 7 , 10, 5 , 2 , -1, -1, -1, -1,
        2 , 10, 5 , 2 , 5 , 3 , 3 , 5 , 7 , -1, -1, -1, -1, -1, -1, -1,
        7 , 9 , 5 , 7 , 8 , 9 , 3 , 11, 2 , -1, -1, -1, -1, -1, -1, -1,
        9 , 5 , 7 , 9 , 7 , 2 , 9 , 2 , 0 , 2 , 7 , 11, -1, -1, -1, -1,
        2 , 3 , 11, 0 , 1 , 8 , 1 , 7 , 8 , 1 , 5 , 7 , -1, -1, -1, -1,
        11, 2 , 1 , 11, 1 , 7 , 7 , 1 , 5 , -1, -1, -1, -1, -1, -1, -1,
        9 , 5 , 8 , 8 , 5 , 7 , 10, 1 , 3 , 10, 3 , 11, -1, -1, -1, -1,
        5 , 7 , 0 , 5 , 0 , 9 , 7 , 11, 0 , 1 , 0 , 10, 11, 10, 0 , -1,
        11, 10, 0 , 11, 0 , 3 , 10, 5 , 0 , 8 , 0 , 7 , 5 , 7 , 0 , -1,
        11, 10, 5 , 7 , 11, 5 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        10, 6 , 5 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        0 , 8 , 3 , 5 , 10, 6 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        9 , 0 , 1 , 5 , 10, 6 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        1 , 8 , 3 , 1 , 9 , 8 , 5 , 10, 6 , -1, -1, -1, -1, -1, -1, -1,
        1 , 6 , 5 , 2 , 6 , 1 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        1 , 6 , 5 , 1 , 2 , 6 , 3 , 0 , 8 , -1, -1, -1, -1, -1, -1, -1,
        9 , 6 , 5 , 9 , 0 , 6 , 0 , 2 , 6 , -1, -1, -1, -1, -1, -1, -1,
        5 , 9 , 8 , 5 , 8 , 2 , 5 , 2 , 6 , 3 , 2 , 8 , -1, -1, -1, -1,
        2 , 3 , 11, 10, 6 , 5 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        11, 0 , 8 , 11, 2 , 0 , 10, 6 , 5 , -1, -1, -1, -1, -1, -1, -1,
        0 , 1 , 9 , 2 , 3 , 11, 5 , 10, 6 , -1, -1, -1, -1, -1, -1, -1,
        5 , 10, 6 , 1 , 9 , 2 , 9 , 11, 2 , 9 , 8 , 11, -1, -1, -1, -1,
        6 , 3 , 11, 6 , 5 , 3 , 5 , 1 , 3 , -1, -1, -1, -1, -1, -1, -1,
        0 , 8 , 11, 0 , 11, 5 , 0 , 5 , 1 , 5 , 11, 6 , -1, -1, -1, -1,
        3 , 11, 6 , 0 , 3 , 6 , 0 , 6 , 5 , 0 , 5 , 9 , -1, -1, -1, -1,
        6 , 5 , 9 , 6 , 9 , 11, 11, 9 , 8 , -1, -1, -1, -1, -1, -1, -1,
        5 , 10, 6 , 4 , 7 , 8 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        4 , 3 , 0 , 4 , 7 , 3 , 6 , 5 , 10, -1, -1, -1, -1, -1, -1, -1,
        1 , 9 , 0 , 5 , 10, 6 , 8 , 4 , 7 , -1, -1, -1, -1, -1, -1, -1,
        10, 6 , 5 , 1 , 9 , 7 , 1 , 7 , 3 , 7 , 9 , 4 , -1, -1, -1, -1,
        6 , 1 , 2 , 6 , 5 , 1 , 4 , 7 , 8 , -1, -1, -1, -1, -1, -1, -1,
        1 , 2 , 5 , 5 , 2 , 6 , 3 , 0 , 4 , 3 , 4 , 7 , -1, -1, -1, -1,
        8 , 4 , 7 , 9 , 0 , 5 , 0 , 6 , 5 , 0 , 2 , 6 , -1, -1, -1, -1,
        7 , 3 , 9 , 7 , 9 , 4 , 3 , 2 , 9 , 5 , 9 , 6 , 2 , 6 , 9 , -1,
        3 , 11, 2 , 7 , 8 , 4 , 10, 6 , 5 , -1, -1, -1, -1, -1, -1, -1,
        5 , 10, 6 , 4 , 7 , 2 , 4 , 2 , 0 , 2 , 7 , 11, -1, -1, -1, -1,
        0 , 1 , 9 , 4 , 7 , 8 , 2 , 3 , 11, 5 , 10, 6 , -1, -1, -1, -1,
        9 , 2 , 1 , 9 , 11, 2 , 9 , 4 , 11, 7 , 11, 4 , 5 , 10, 6 , -1,
        8 , 4 , 7 , 3 , 11, 5 , 3 , 5 , 1 , 5 , 11, 6 , -1, -1, -1, -1,
        5 , 1 , 11, 5 , 11, 6 , 1 , 0 , 11, 7 , 11, 4 , 0 , 4 , 11, -1,
        0 , 5 , 9 , 0 , 6 , 5 , 0 , 3 , 6 , 11, 6 , 3 , 8 , 4 , 7 , -1,
        6 , 5 , 9 , 6 , 9 , 11, 4 , 7 , 9 , 7 , 11, 9 , -1, -1, -1, -1,
        10, 4 , 9 , 6 , 4 , 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        4 , 10, 6 , 4 , 9 , 10, 0 , 8 , 3 , -1, -1, -1, -1, -1, -1, -1,
        10, 0 , 1 , 10, 6 , 0 , 6 , 4 , 0 , -1, -1, -1, -1, -1, -1, -1,
        8 , 3 , 1 , 8 , 1 , 6 , 8 , 6 , 4 , 6 , 1 , 10, -1, -1, -1, -1,
        1 , 4 , 9 , 1 , 2 , 4 , 2 , 6 , 4 , -1, -1, -1, -1, -1, -1, -1,
        3 , 0 , 8 , 1 , 2 , 9 , 2 , 4 , 9 , 2 , 6 , 4 , -1, -1, -1, -1,
        0 , 2 , 4 , 4 , 2 , 6 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        8 , 3 , 2 , 8 , 2 , 4 , 4 , 2 , 6 , -1, -1, -1, -1, -1, -1, -1,
        10, 4 , 9 , 10, 6 , 4 , 11, 2 , 3 , -1, -1, -1, -1, -1, -1, -1,
        0 , 8 , 2 , 2 , 8 , 11, 4 , 9 , 10, 4 , 10, 6 , -1, -1, -1, -1,
        3 , 11, 2 , 0 , 1 , 6 , 0 , 6 , 4 , 6 , 1 , 10, -1, -1, -1, -1,
        6 , 4 , 1 , 6 , 1 , 10, 4 , 8 , 1 , 2 , 1 , 11, 8 , 11, 1 , -1,
        9 , 6 , 4 , 9 , 3 , 6 , 9 , 1 , 3 , 11, 6 , 3 , -1, -1, -1, -1,
        8 , 11, 1 , 8 , 1 , 0 , 11, 6 , 1 , 9 , 1 , 4 , 6 , 4 , 1 , -1,
        3 , 11, 6 , 3 , 6 , 0 , 0 , 6 , 4 , -1, -1, -1, -1, -1, -1, -1,
        6 , 4 , 8 , 11, 6 , 8 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        7 , 10, 6 , 7 , 8 , 10, 8 , 9 , 10, -1, -1, -1, -1, -1, -1, -1,
        0 , 7 , 3 , 0 , 10, 7 , 0 , 9 , 10, 6 , 7 , 10, -1, -1, -1, -1,
        10, 6 , 7 , 1 , 10, 7 , 1 , 7 , 8 , 1 , 8 , 0 , -1, -1, -1, -1,
        10, 6 , 7 , 10, 7 , 1 , 1 , 7 , 3 , -1, -1, -1, -1, -1, -1, -1,
        1 , 2 , 6 , 1 , 6 , 8 , 1 , 8 , 9 , 8 , 6 , 7 , -1, -1, -1, -1,
        2 , 6 , 9 , 2 , 9 , 1 , 6 , 7 , 9 , 0 , 9 , 3 , 7 , 3 , 9 , -1,
        7 , 8 , 0 , 7 , 0 , 6 , 6 , 0 , 2 , -1, -1, -1, -1, -1, -1, -1,
        7 , 3 , 2 , 6 , 7 , 2 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        2 , 3 , 11, 10, 6 , 8 , 10, 8 , 9 , 8 , 6 , 7 , -1, -1, -1, -1,
        2 , 0 , 7 , 2 , 7 , 11, 0 , 9 , 7 , 6 , 7 , 10, 9 , 10, 7 , -1,
        1 , 8 , 0 , 1 , 7 , 8 , 1 , 10, 7 , 6 , 7 , 10, 2 , 3 , 11, -1,
        11, 2 , 1 , 11, 1 , 7 , 10, 6 , 1 , 6 , 7 , 1 , -1, -1, -1, -1,
        8 , 9 , 6 , 8 , 6 , 7 , 9 , 1 , 6 , 11, 6 , 3 , 1 , 3 , 6 , -1,
        0 , 9 , 1 , 11, 6 , 7 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        7 , 8 , 0 , 7 , 0 , 6 , 3 , 11, 0 , 11, 6 , 0 , -1, -1, -1, -1,
        7 , 11, 6 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        7 , 6 , 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        3 , 0 , 8 , 11, 7 , 6 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        0 , 1 , 9 , 11, 7 , 6 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        8 , 1 , 9 , 8 , 3 , 1 , 11, 7 , 6 , -1, -1, -1, -1, -1, -1, -1,
        10, 1 , 2 , 6 , 11, 7 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        1 , 2 , 10, 3 , 0 , 8 , 6 , 11, 7 , -1, -1, -1, -1, -1, -1, -1,
        2 , 9 , 0 , 2 , 10, 9 , 6 , 11, 7 , -1, -1, -1, -1, -1, -1, -1,
        6 , 11, 7 , 2 , 10, 3 , 10, 8 , 3 , 10, 9 , 8 , -1, -1, -1, -1,
        7 , 2 , 3 , 6 , 2 , 7 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        7 , 0 , 8 , 7 , 6 , 0 , 6 , 2 , 0 , -1, -1, -1, -1, -1, -1, -1,
        2 , 7 , 6 , 2 , 3 , 7 , 0 , 1 , 9 , -1, -1, -1, -1, -1, -1, -1,
        1 , 6 , 2 , 1 , 8 , 6 , 1 , 9 , 8 , 8 , 7 , 6 , -1, -1, -1, -1,
        10, 7 , 6 , 10, 1 , 7 , 1 , 3 , 7 , -1, -1, -1, -1, -1, -1, -1,
        10, 7 , 6 , 1 , 7 , 10, 1 , 8 , 7 , 1 , 0 , 8 , -1, -1, -1, -1,
        0 , 3 , 7 , 0 , 7 , 10, 0 , 10, 9 , 6 , 10, 7 , -1, -1, -1, -1,
        7 , 6 , 10, 7 , 10, 8 , 8 , 10, 9 , -1, -1, -1, -1, -1, -1, -1,
        6 , 8 , 4 , 11, 8 , 6 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        3 , 6 , 11, 3 , 0 , 6 , 0 , 4 , 6 , -1, -1, -1, -1, -1, -1, -1,
        8 , 6 , 11, 8 , 4 , 6 , 9 , 0 , 1 , -1, -1, -1, -1, -1, -1, -1,
        9 , 4 , 6 , 9 , 6 , 3 , 9 , 3 , 1 , 11, 3 , 6 , -1, -1, -1, -1,
        6 , 8 , 4 , 6 , 11, 8 , 2 , 10, 1 , -1, -1, -1, -1, -1, -1, -1,
        1 , 2 , 10, 3 , 0 , 11, 0 , 6 , 11, 0 , 4 , 6 , -1, -1, -1, -1,
        4 , 11, 8 , 4 , 6 , 11, 0 , 2 , 9 , 2 , 10, 9 , -1, -1, -1, -1,
        10, 9 , 3 , 10, 3 , 2 , 9 , 4 , 3 , 11, 3 , 6 , 4 , 6 , 3 , -1,
        8 , 2 , 3 , 8 , 4 , 2 , 4 , 6 , 2 , -1, -1, -1, -1, -1, -1, -1,
        0 , 4 , 2 , 4 , 6 , 2 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        1 , 9 , 0 , 2 , 3 , 4 , 2 , 4 , 6 , 4 , 3 , 8 , -1, -1, -1, -1,
        1 , 9 , 4 , 1 , 4 , 2 , 2 , 4 , 6 , -1, -1, -1, -1, -1, -1, -1,
        8 , 1 , 3 , 8 , 6 , 1 , 8 , 4 , 6 , 6 , 10, 1 , -1, -1, -1, -1,
        10, 1 , 0 , 10, 0 , 6 , 6 , 0 , 4 , -1, -1, -1, -1, -1, -1, -1,
        4 , 6 , 3 , 4 , 3 , 8 , 6 , 10, 3 , 0 , 3 , 9 , 10, 9 , 3 , -1,
        10, 9 , 4 , 6 , 10, 4 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        4 , 9 , 5 , 7 , 6 , 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        0 , 8 , 3 , 4 , 9 , 5 , 11, 7 , 6 , -1, -1, -1, -1, -1, -1, -1,
        5 , 0 , 1 , 5 , 4 , 0 , 7 , 6 , 11, -1, -1, -1, -1, -1, -1, -1,
        11, 7 , 6 , 8 , 3 , 4 , 3 , 5 , 4 , 3 , 1 , 5 , -1, -1, -1, -1,
        9 , 5 , 4 , 10, 1 , 2 , 7 , 6 , 11, -1, -1, -1, -1, -1, -1, -1,
        6 , 11, 7 , 1 , 2 , 10, 0 , 8 , 3 , 4 , 9 , 5 , -1, -1, -1, -1,
        7 , 6 , 11, 5 , 4 , 10, 4 , 2 , 10, 4 , 0 , 2 , -1, -1, -1, -1,
        3 , 4 , 8 , 3 , 5 , 4 , 3 , 2 , 5 , 10, 5 , 2 , 11, 7 , 6 , -1,
        7 , 2 , 3 , 7 , 6 , 2 , 5 , 4 , 9 , -1, -1, -1, -1, -1, -1, -1,
        9 , 5 , 4 , 0 , 8 , 6 , 0 , 6 , 2 , 6 , 8 , 7 , -1, -1, -1, -1,
        3 , 6 , 2 , 3 , 7 , 6 , 1 , 5 , 0 , 5 , 4 , 0 , -1, -1, -1, -1,
        6 , 2 , 8 , 6 , 8 , 7 , 2 , 1 , 8 , 4 , 8 , 5 , 1 , 5 , 8 , -1,
        9 , 5 , 4 , 10, 1 , 6 , 1 , 7 , 6 , 1 , 3 , 7 , -1, -1, -1, -1,
        1 , 6 , 10, 1 , 7 , 6 , 1 , 0 , 7 , 8 , 7 , 0 , 9 , 5 , 4 , -1,
        4 , 0 , 10, 4 , 10, 5 , 0 , 3 , 10, 6 , 10, 7 , 3 , 7 , 10, -1,
        7 , 6 , 10, 7 , 10, 8 , 5 , 4 , 10, 4 , 8 , 10, -1, -1, -1, -1,
        6 , 9 , 5 , 6 , 11, 9 , 11, 8 , 9 , -1, -1, -1, -1, -1, -1, -1,
        3 , 6 , 11, 0 , 6 , 3 , 0 , 5 , 6 , 0 , 9 , 5 , -1, -1, -1, -1,
        0 , 11, 8 , 0 , 5 , 11, 0 , 1 , 5 , 5 , 6 , 11, -1, -1, -1, -1,
        6 , 11, 3 , 6 , 3 , 5 , 5 , 3 , 1 , -1, -1, -1, -1, -1, -1, -1,
        1 , 2 , 10, 9 , 5 , 11, 9 , 11, 8 , 11, 5 , 6 , -1, -1, -1, -1,
        0 , 11, 3 , 0 , 6 , 11, 0 , 9 , 6 , 5 , 6 , 9 , 1 , 2 , 10, -1,
        11, 8 , 5 , 11, 5 , 6 , 8 , 0 , 5 , 10, 5 , 2 , 0 , 2 , 5 , -1,
        6 , 11, 3 , 6 , 3 , 5 , 2 , 10, 3 , 10, 5 , 3 , -1, -1, -1, -1,
        5 , 8 , 9 , 5 , 2 , 8 , 5 , 6 , 2 , 3 , 8 , 2 , -1, -1, -1, -1,
        9 , 5 , 6 , 9 , 6 , 0 , 0 , 6 , 2 , -1, -1, -1, -1, -1, -1, -1,
        1 , 5 , 8 , 1 , 8 , 0 , 5 , 6 , 8 , 3 , 8 , 2 , 6 , 2 , 8 , -1,
        1 , 5 , 6 , 2 , 1 , 6 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        1 , 3 , 6 , 1 , 6 , 10, 3 , 8 , 6 , 5 , 6 , 9 , 8 , 9 , 6 , -1,
        10, 1 , 0 , 10, 0 , 6 , 9 , 5 , 0 , 5 , 6 , 0 , -1, -1, -1, -1,
        0 , 3 , 8 , 5 , 6 , 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        10, 5 , 6 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        11, 5 , 10, 7 , 5 , 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        11, 5 , 10, 11, 7 , 5 , 8 , 3 , 0 , -1, -1, -1, -1, -1, -1, -1,
        5 , 11, 7 , 5 , 10, 11, 1 , 9 , 0 , -1, -1, -1, -1, -1, -1, -1,
        10, 7 , 5 , 10, 11, 7 , 9 , 8 , 1 , 8 , 3 , 1 , -1, -1, -1, -1,
        11, 1 , 2 , 11, 7 , 1 , 7 , 5 , 1 , -1, -1, -1, -1, -1, -1, -1,
        0 , 8 , 3 , 1 , 2 , 7 , 1 , 7 , 5 , 7 , 2 , 11, -1, -1, -1, -1,
        9 , 7 , 5 , 9 , 2 , 7 , 9 , 0 , 2 , 2 , 11, 7 , -1, -1, -1, -1,
        7 , 5 , 2 , 7 , 2 , 11, 5 , 9 , 2 , 3 , 2 , 8 , 9 , 8 , 2 , -1,
        2 , 5 , 10, 2 , 3 , 5 , 3 , 7 , 5 , -1, -1, -1, -1, -1, -1, -1,
        8 , 2 , 0 , 8 , 5 , 2 , 8 , 7 , 5 , 10, 2 , 5 , -1, -1, -1, -1,
        9 , 0 , 1 , 5 , 10, 3 , 5 , 3 , 7 , 3 , 10, 2 , -1, -1, -1, -1,
        9 , 8 , 2 , 9 , 2 , 1 , 8 , 7 , 2 , 10, 2 , 5 , 7 , 5 , 2 , -1,
        1 , 3 , 5 , 3 , 7 , 5 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        0 , 8 , 7 , 0 , 7 , 1 , 1 , 7 , 5 , -1, -1, -1, -1, -1, -1, -1,
        9 , 0 , 3 , 9 , 3 , 5 , 5 , 3 , 7 , -1, -1, -1, -1, -1, -1, -1,
        9 , 8 , 7 , 5 , 9 , 7 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        5 , 8 , 4 , 5 , 10, 8 , 10, 11, 8 , -1, -1, -1, -1, -1, -1, -1,
        5 , 0 , 4 , 5 , 11, 0 , 5 , 10, 11, 11, 3 , 0 , -1, -1, -1, -1,
        0 , 1 , 9 , 8 , 4 , 10, 8 , 10, 11, 10, 4 , 5 , -1, -1, -1, -1,
        10, 11, 4 , 10, 4 , 5 , 11, 3 , 4 , 9 , 4 , 1 , 3 , 1 , 4 , -1,
        2 , 5 , 1 , 2 , 8 , 5 , 2 , 11, 8 , 4 , 5 , 8 , -1, -1, -1, -1,
        0 , 4 , 11, 0 , 11, 3 , 4 , 5 , 11, 2 , 11, 1 , 5 , 1 , 11, -1,
        0 , 2 , 5 , 0 , 5 , 9 , 2 , 11, 5 , 4 , 5 , 8 , 11, 8 , 5 , -1,
        9 , 4 , 5 , 2 , 11, 3 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        2 , 5 , 10, 3 , 5 , 2 , 3 , 4 , 5 , 3 , 8 , 4 , -1, -1, -1, -1,
        5 , 10, 2 , 5 , 2 , 4 , 4 , 2 , 0 , -1, -1, -1, -1, -1, -1, -1,
        3 , 10, 2 , 3 , 5 , 10, 3 , 8 , 5 , 4 , 5 , 8 , 0 , 1 , 9 , -1,
        5 , 10, 2 , 5 , 2 , 4 , 1 , 9 , 2 , 9 , 4 , 2 , -1, -1, -1, -1,
        8 , 4 , 5 , 8 , 5 , 3 , 3 , 5 , 1 , -1, -1, -1, -1, -1, -1, -1,
        0 , 4 , 5 , 1 , 0 , 5 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        8 , 4 , 5 , 8 , 5 , 3 , 9 , 0 , 5 , 0 , 3 , 5 , -1, -1, -1, -1,
        9 , 4 , 5 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        4 , 11, 7 , 4 , 9 , 11, 9 , 10, 11, -1, -1, -1, -1, -1, -1, -1,
        0 , 8 , 3 , 4 , 9 , 7 , 9 , 11, 7 , 9 , 10, 11, -1, -1, -1, -1,
        1 , 10, 11, 1 , 11, 4 , 1 , 4 , 0 , 7 , 4 , 11, -1, -1, -1, -1,
        3 , 1 , 4 , 3 , 4 , 8 , 1 , 10, 4 , 7 , 4 , 11, 10, 11, 4 , -1,
        4 , 11, 7 , 9 , 11, 4 , 9 , 2 , 11, 9 , 1 , 2 , -1, -1, -1, -1,
        9 , 7 , 4 , 9 , 11, 7 , 9 , 1 , 11, 2 , 11, 1 , 0 , 8 , 3 , -1,
        11, 7 , 4 , 11, 4 , 2 , 2 , 4 , 0 , -1, -1, -1, -1, -1, -1, -1,
        11, 7 , 4 , 11, 4 , 2 , 8 , 3 , 4 , 3 , 2 , 4 , -1, -1, -1, -1,
        2 , 9 , 10, 2 , 7 , 9 , 2 , 3 , 7 , 7 , 4 , 9 , -1, -1, -1, -1,
        9 , 10, 7 , 9 , 7 , 4 , 10, 2 , 7 , 8 , 7 , 0 , 2 , 0 , 7 , -1,
        3 , 7 , 10, 3 , 10, 2 , 7 , 4 , 10, 1 , 10, 0 , 4 , 0 , 10, -1,
        1 , 10, 2 , 8 , 7 , 4 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        4 , 9 , 1 , 4 , 1 , 7 , 7 , 1 , 3 , -1, -1, -1, -1, -1, -1, -1,
        4 , 9 , 1 , 4 , 1 , 7 , 0 , 8 , 1 , 8 , 7 , 1 , -1, -1, -1, -1,
        4 , 0 , 3 , 7 , 4 , 3 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        4 , 8 , 7 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        9 , 10, 8 , 10, 11, 8 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        3 , 0 , 9 , 3 , 9 , 11, 11, 9 , 10, -1, -1, -1, -1, -1, -1, -1,
        0 , 1 , 10, 0 , 10, 8 , 8 , 10, 11, -1, -1, -1, -1, -1, -1, -1,
        3 , 1 , 10, 11, 3 , 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        1 , 2 , 11, 1 , 11, 9 , 9 , 11, 8 , -1, -1, -1, -1, -1, -1, -1,
        3 , 0 , 9 , 3 , 9 , 11, 1 , 2 , 9 , 2 , 11, 9 , -1, -1, -1, -1,
        0 , 2 , 11, 8 , 0 , 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        3 , 2 , 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        2 , 3 , 8 , 2 , 8 , 10, 10, 8 , 9 , -1, -1, -1, -1, -1, -1, -1,
        9 , 10, 2 , 0 , 9 , 2 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        2 , 3 , 8 , 2 , 8 , 10, 0 , 1 , 8 , 1 , 10, 8 , -1, -1, -1, -1,
        1 , 10, 2 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        1 , 3 , 8 , 9 , 1 , 8 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        0 , 9 , 1 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        0 , 3 , 8 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};


const ivec2 EDGELIST[12] =
{
    ivec2(0,1), ivec2(1,2), ivec2(2,3), ivec2(3,0), ivec2(4,5), ivec2(5,6), ivec2(6,7), ivec2(7,4), ivec2(0,4), ivec2(1,5), ivec2(2,6), ivec2(3,7)
};




struct Vertex{
	vec4 position;
	vec4 normal;
};

struct Cube{
	vec3 positions[8];
	float values[8];
	ivec3 global_indices[8];
};

layout( packed, binding=3 ) buffer Values   // inputted values 
{
	uint nonemptyList[];
};

layout( packed, binding=2 ) buffer Vertices   // outputted values 
{
	uint vertexOutputCounter;  // The current end of the vertices list
	Vertex verticesOutput[];
};

uniform vec3 CellSize;
uniform vec3 StartPos;
uniform float IsoLevel;
uniform ivec3 ChunkSize;
uniform int IndexCount;
layout(binding=0) uniform sampler3D SeedTexture0;
layout(binding=1) uniform sampler3D SeedTexture1;
layout(binding=2) uniform sampler3D SeedTexture2;
layout(binding=3) uniform sampler3D SeedTexture3;
layout(binding=4) uniform sampler3D SeedTexture4;
layout(binding=5) uniform sampler3D SeedTexture5;
layout(binding=6) uniform sampler3D SeedTexture6;
layout(binding=7) uniform sampler3D SeedTexture7;
layout(binding=8) uniform sampler3D SeedTexture8;
layout(r32f,binding = 2) readonly uniform image3D densityTexture;



vec3 IndexToWorldPosition(in ivec3 index){
	return StartPos + (vec3(index) * CellSize);
}

float GetValueAtPoint(in ivec3 index){
	vec3 worldPos = IndexToWorldPosition(index);
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

vec3 GetNormal(in ivec3 global_index){
	/*
		get differential of the field at global_index within the chunk,
		using the neighbouring values stored in the texture where possible - 
		for the voxels on the outer edges of the cube this will have to be calculated afresh 
		
		TODO: Implement this properly
	*/
	float xMinus, xPlus, yMinus, yPlus, zMinus, zPlus;
	xMinus = imageLoad(densityTexture, global_index + ivec3(-1, 0, 0)).x;//((global_index.x - 1) >= 0)         ? imageLoad(densityTexture, global_index + ivec3(-1, 0, 0)).x : GetValueAtPoint(global_index + ivec3(-1, 0, 0));
	xPlus =  imageLoad(densityTexture, global_index + ivec3( 1, 0, 0)).x;//((global_index.x + 1) <  ChunkSize.x) ? imageLoad(densityTexture, global_index + ivec3( 1, 0, 0)).x : GetValueAtPoint(global_index + ivec3( 1, 0, 0));
	
	yMinus = imageLoad(densityTexture, global_index + ivec3( 0,-1, 0)).x;//((global_index.y - 1) >= 0)         ? imageLoad(densityTexture, global_index + ivec3( 0,-1, 0)).x : GetValueAtPoint(global_index + ivec3( 0,-1, 0));
	yPlus =  imageLoad(densityTexture, global_index + ivec3( 0, 1, 0)).x;//((global_index.y + 1) <  ChunkSize.y) ? imageLoad(densityTexture, global_index + ivec3( 0, 1, 0)).x : GetValueAtPoint(global_index + ivec3( 0, 1, 0));
	
	zMinus = imageLoad(densityTexture, global_index + ivec3( 0, 0,-1)).x;//((global_index.z - 1) >= 0)         ? imageLoad(densityTexture, global_index + ivec3( 0, 0,-1)).x : GetValueAtPoint(global_index + ivec3( 0, 0,-1));
	zPlus =  imageLoad(densityTexture, global_index + ivec3( 0, 0, 1)).x;//((global_index.z + 1) <  ChunkSize.z) ? imageLoad(densityTexture, global_index + ivec3( 0, 0, 1)).x : GetValueAtPoint(global_index + ivec3( 0, 0, 1));
	
	return vec3(
		xMinus - xPlus,
		yMinus - yPlus,
		zMinus - zPlus
	);
}

Vertex VertexInterpolation(in Cube cube, int index1, int index2){
	float val1 = cube.values[index1];
	float val2 = cube.values[index2];
	
	vec3 pos1 = cube.positions[index1];
	vec3 pos2 = cube.positions[index2];
	
	ivec3 ind1 = cube.global_indices[index1];
	ivec3 ind2 = cube.global_indices[index2];
	
	vec3 norm1 = normalize(GetNormal(ind1));
	vec3 norm2 = normalize(GetNormal(ind2));
	
	float t = (IsoLevel - val1) / (val2 - val1);
	Vertex v;
	v.position = vec4(mix(pos1,pos2,t),0);
	v.normal = vec4(mix(norm1,norm2,t),0);
	return v;
}

void main(){
	/* 
		 shader is dispatched 1 thread per non empty cube
	*/
	uint ind = gl_WorkGroupID.x * NUM_LOCAL_THREADS + gl_LocalInvocationID.x;
	if(ind >= IndexCount){
		return;
	}
	// get a 32 bit value describing x y and z coordinates and marching cubes case of a non empty cube
	uint myCaseAndIndex = nonemptyList[ind];
	// the marching cubes value describing which corners of the cube are in the field
	uint myCase = myCaseAndIndex & 0x000000ff;
	// number of vertices this cube will produce
	uint myNumVertices = NUMVERTICES[myCase];
	// grab space in the output buffer
	uint myVerticesWritePtr = atomicAdd(vertexOutputCounter, myNumVertices);
	// get the cube's index as an ivec3
	ivec3 myIndex = ivec3(
		(myCaseAndIndex >> 8) & 0x000000ff,
		(myCaseAndIndex >> 16) & 0x000000ff,
		(myCaseAndIndex >> 24) & 0x000000ff
	);
	ivec3 cornerindex;
	// load up a cube struct with position, field value and global index within the chunk for each of the cubes 8 corners
	Cube cube;
	cornerindex = myIndex;
	cube.positions[0] = IndexToWorldPosition(cornerindex);
	cube.values[0] = imageLoad(densityTexture,cornerindex).x;
	cube.global_indices[0] = cornerindex;
	
	cornerindex = myIndex + ivec3(1,0,0);
	cube.positions[1] = IndexToWorldPosition(cornerindex);
	cube.values[1] = imageLoad(densityTexture,cornerindex).x;
	cube.global_indices[1] = cornerindex;
	
	cornerindex = myIndex + ivec3(1,1,0);
	cube.positions[2] = IndexToWorldPosition(cornerindex);
	cube.values[2] = imageLoad(densityTexture,cornerindex).x;
	cube.global_indices[2] = cornerindex;
	
	cornerindex = myIndex + ivec3(0,1,0);
	cube.positions[3] = IndexToWorldPosition(cornerindex);
	cube.values[3] = imageLoad(densityTexture,cornerindex).x;
	cube.global_indices[3] = cornerindex;
	
	cornerindex = myIndex + ivec3(0,0,1);
	cube.positions[4] = IndexToWorldPosition(cornerindex);
	cube.values[4] = imageLoad(densityTexture,cornerindex).x;
	cube.global_indices[4] = cornerindex;
	
	cornerindex = myIndex + ivec3(1,0,1);
	cube.positions[5] = IndexToWorldPosition(cornerindex);
	cube.values[5] = imageLoad(densityTexture,cornerindex).x;
	cube.global_indices[5] = cornerindex;
	
	cornerindex = myIndex + ivec3(1,1,1);
	cube.positions[6] = IndexToWorldPosition(cornerindex);
	cube.values[6] = imageLoad(densityTexture,cornerindex).x;
	cube.global_indices[6] = cornerindex;
	
	cornerindex = myIndex + ivec3(0,1,1);
	cube.positions[7] = IndexToWorldPosition(cornerindex);
	cube.values[7] = imageLoad(densityTexture,cornerindex).x;
	cube.global_indices[7] = cornerindex;
	
	uint numPolys = myNumVertices / 3;
	uint triTableBase = myCase * 16;
	for(int i=0; i<numPolys; i++){
		for(int j=0; j<3; j++){
			int edgeNum = TRIANGLES[triTableBase + 3*i + j];
			int v1 = EDGELIST[edgeNum].x;
			int v2 = EDGELIST[edgeNum].y;
			verticesOutput[myVerticesWritePtr++] = VertexInterpolation(cube,v1,v2);
		}
	}
	// get a vertex by interpolation for any edge that needs one
	/*
	Vertex vertList[12];
	uint edge = EDGES[myCase];
	if ((edge & 1) != 0)
		vertList[0] = VertexInterpolation(cube, 0, 1);
	if ((edge & 2) != 0)
		vertList[1] = VertexInterpolation(cube, 1, 2);
	if ((edge & 4) != 0)
		vertList[2] = VertexInterpolation(cube, 2, 3);
	if ((edge & 8) != 0)
		vertList[3] = VertexInterpolation(cube, 3, 0);
	if ((edge & 16) != 0)
		vertList[4] = VertexInterpolation(cube, 4, 5);
	if ((edge & 32) != 0)
		vertList[5] = VertexInterpolation(cube, 5, 6);
	if ((edge & 64) != 0)
		vertList[6] = VertexInterpolation(cube, 6, 7);
	if ((edge & 128) != 0)
		vertList[7] = VertexInterpolation(cube, 7, 4);
	if ((edge & 256) != 0)
		vertList[8] = VertexInterpolation(cube, 0, 4);
	if ((edge & 512) != 0)
		vertList[9] = VertexInterpolation(cube, 1, 5);
	if ((edge & 1024) != 0)
		vertList[10] = VertexInterpolation(cube, 2, 6);
	if ((edge & 2048) != 0)
		vertList[11] = VertexInterpolation(cube, 3, 7);
	*/
	
	/* 
		add the vertices to the output buffer using TRIANGLES table to find the ones that
		have been set and add them in the right order 
	*/
	/*
	uint triTableBase = myCase * 16;
	for (int j = 0; TRIANGLES[triTableBase + j] != -1; j += 3){
		verticesOutput[myVerticesWritePtr++] = vertList[TRIANGLES[triTableBase + j + 2]];
		verticesOutput[myVerticesWritePtr++] = vertList[TRIANGLES[triTableBase + j + 1]];
		verticesOutput[myVerticesWritePtr++] = vertList[TRIANGLES[triTableBase + j]];
	}
	*/
}