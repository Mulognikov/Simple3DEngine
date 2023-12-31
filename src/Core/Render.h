#pragma once
#include "../Math/Vec2i.h"
#include "../Math/Vec3i.h"
#include "../Math/Vec3f.h"
#include "Mesh.h"
#include "Transform.h"
#include "ScreenTriangle.h"
#include "Texture.h"

#define RENDER_W 320
#define RENDER_H 240
#define PIXELS_COUNT RENDER_W * RENDER_H
#define ASPECT_RATIO ((float)RENDER_H / (float)RENDER_W)
#define Z_BUFFER_DEPTH float
#define MAX_VERTICES 1000000

class Render
{
public:
	static void initialize();
	static void drawPoint(uint16_t x, uint16_t y, uint8_t color);
	static void drawLineFast(ScreenTriangle p0, ScreenTriangle p1, uint8_t color);
	static void drawTriangleWire(ScreenTriangle t0, ScreenTriangle t1, ScreenTriangle t2, uint8_t color);
	static void drawTriangleFill(ScreenTriangle t0, ScreenTriangle t1, ScreenTriangle t2, uint8_t color);
	static void drawTriangleTextured(ScreenTriangle t0, ScreenTriangle t1, ScreenTriangle t2, Texture &texture);
	static void drawMesh(Mesh &mesh, Transform &transform);
	static void drawMesh2(Mesh &mesh, Transform &transform, Texture &texture);
	static void render();

private:
	Render() = default;
	static inline uint8_t frameBuffer[PIXELS_COUNT];
	static inline Z_BUFFER_DEPTH zBuffer[PIXELS_COUNT];
	static inline float x02[RENDER_H];
	static inline float x012[RENDER_H];
	static inline float u02[RENDER_H];
	static inline float u012[RENDER_H];
	static inline float uValues[RENDER_W];
	static inline float v02[RENDER_H];
	static inline float v012[RENDER_H];
	static inline float vValues[RENDER_W];
	static inline Z_BUFFER_DEPTH z02[RENDER_H];
	static inline Z_BUFFER_DEPTH z012[RENDER_H];
	static inline Z_BUFFER_DEPTH zValues[RENDER_W];
	static inline Vec4f projectedVertices[MAX_VERTICES];


	static inline Vec4f viewVertices[10000000];
	static inline Triangle cullFaces[10000000];
	static inline ScreenTriangle screenPositions[10000000];
	static inline Triangle newTriangles[10000000];


	static void verticesWorldToView(Vec3f *inVertices, uint32_t &verticesCount, Transform &transform, Vec4f *outVertices);
	static void verticesViewToScreen(Vec4f *inVertices, uint32_t &verticesCount, ScreenTriangle *outVertices);
	static void cullBackFaces(Vec4f *inVertices, Triangle *inTriangles, uint32_t &trianglesCount, Triangle *outTriangles, uint32_t &outCount);
	static void clipFaces(Vec4f *inVertices, Triangle *inTriangles, uint32_t &verticesCount, uint32_t &trianglesCount, Triangle *outTriangles, uint32_t &outTrianglesCount);
	static void clipFaceZ(Vec4f *inVertices, Vec3f *inUV, uint32_t &verticesCount, uint32_t &uvCount, Triangle &inTriangle, Triangle *outTriangle, uint8_t &outTrianglesCount);
	static void clearScreen();
	static void interpolate(int i0, float d0, int i1, float d1, float *values, int& count);
	static void interpolateInScreenBounds(int i0, float d0, int i1, float d1, float *values, int& count, int offset, int max);
	static void intersectionWithPlane(Vec3f &startPositive, Vec3f &endNegative, Vec3f &planeNormal, float &planeD, Vec3f &intersectPoint);
	static void intersectionWithPlane(Vec4f &startPositive,
									  Vec4f &endNegative,
									  Vec3f &uvStart,
									  Vec3f &uvEnd,
									  Vec3f &planeNormal,
									  float &planeD,
									  Vec4f &intersectPoint,
									  Vec3f &uvIntersect);


	static inline float edgeFunction(const Vec3f &a, const Vec3f &b, const Vec3f &c)
	{
		return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
	}

	static inline int clamp(int d, int min, int max) {
		const int t = d < min ? min : d;
		return t > max ? max : t;
	}
};
