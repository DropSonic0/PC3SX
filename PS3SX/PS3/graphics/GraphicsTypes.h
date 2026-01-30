/*
 * GraphicsTypes.h
 *
 *  Created on: Nov 10, 2010
 *      Author: halsafar
 */

#ifndef GRAPHICSTYPES_H_
#define GRAPHICSTYPES_H_

typedef struct _Vertex
{
        float x;
        float y;
        float z;
} Vertex;

typedef struct _TextureCoord
{
        float u;
        float v;
} TextureCoord;

typedef struct _Quad
{
        Vertex v1;
        Vertex v2;
        Vertex v3;
        Vertex v4;
        TextureCoord t1;
        TextureCoord t2;
        TextureCoord t3;
        TextureCoord t4;
} Quad;

typedef struct _Rect
{
	float x;
	float y;
	float w;
	float h;
} Rect;


#endif /* GRAPHICSTYPES_H_ */
