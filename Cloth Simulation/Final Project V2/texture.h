#pragma once
/*************************************************
 *
 *                 texture.h
 *
 *  Simple texture loader.  Data structure and
 *  procedure declarations
 *
 *************************************************/

struct Texture {
	int width;
	int height;
	int depth;
	int size;
	unsigned char* data;
};

struct Cube {
	int width;
	int height;
	int depth;
	unsigned char* data[6];
};

struct Texture* loadTexture(const char* filename);
struct Cube* loadCube(const char* basename);