/********************************
 *
 *           texture.cpp
 *
 *  Simple texture loader
 *
 *****************************/
#define _CRT_SECURE_NO_WARNINGS
#include <FreeImage.h>
#include "texture.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct Texture* loadTexture(const char* filename) {
	int i, j;
	int k;
	FIBITMAP* bitmap;
	BYTE* bits;
	unsigned char* data;
	struct Texture* result;
	int size;

	bitmap = FreeImage_Load(FIF_JPEG, filename, JPEG_DEFAULT);
	result = new Texture;

	result->width = FreeImage_GetWidth(bitmap);
	result->height = FreeImage_GetHeight(bitmap);
	result->depth = FreeImage_GetBPP(bitmap) / 8;
	size = result->width * result->height * result->depth;
	result->size = size;
	data = (unsigned char*)malloc(size);
	result->data = data;

	k = 0;
	for (j = 0; j < result->height; j++) {
		bits = FreeImage_GetScanLine(bitmap, j);
		for (i = 0; i < result->width; i++) {
			data[k++] = bits[FI_RGBA_RED];
			data[k++] = bits[FI_RGBA_GREEN];
			data[k++] = bits[FI_RGBA_BLUE];
			bits += 3;
		}
	}

	return(result);

}

struct Cube* loadCube(const char* basename) {
	char filename[256];
	const char* extensions[] = { "posx.jpg", "negx.jpg", "posz.jpg", "negz.jpg", "posy.jpg", "negy.jpg" };
	struct Cube* result;
	struct Texture* tex;
	int i;

	result = new Cube();
	strcpy(filename, basename);
	strcat(filename, "/");
	strcat(filename, extensions[0]);

	tex = loadTexture(filename);
	result->width = tex->width;
	result->height = tex->height;
	result->depth = tex->depth;
	result->data[0] = tex->data;
	for (i = 1; i < 6; i++) {
		strcpy(filename, basename);
		strcat(filename, "/");
		strcat(filename, extensions[i]);
		tex = loadTexture(filename);
		result->data[i] = tex->data;
	}

	return(result);

}