#pragma once
#include "../Math/Vec2i.h"
#include "../Math/Vec3i.h"
#include "../Math/Vec3f.h"
#include "Mesh.h"
#include "Transform.h"
#include "ScreenTriangle.h"

#define RENDER_W 320
#define RENDER_H 240
#define PIXELS_COUNT RENDER_W * RENDER_H
#define ASPECT_RATIO ((float)RENDER_H / (float)RENDER_W)
#define Z_BUFFER_DEPTH float

class Render
{
public:
	static void initialize();
	static void drawPoint(uint16_t x, uint16_t y, uint8_t color);
	static void drawLineFast(ScreenTriangle p0, ScreenTriangle p1, uint8_t color);
	static void drawTriangleWire(ScreenTriangle t0, ScreenTriangle t1, ScreenTriangle t2, uint8_t color);
	static void drawTriangleFill(ScreenTriangle t0, ScreenTriangle t1, ScreenTriangle t2, uint8_t color);
	static void drawMesh(Mesh &mesh, Transform &transform);
	static void render();

private:
	Render() = default;
	static inline uint8_t frameBuffer[PIXELS_COUNT];
	static inline Z_BUFFER_DEPTH zBuffer[PIXELS_COUNT];
	static inline float x02[RENDER_H];
	static inline float x012[RENDER_H];
	static inline Z_BUFFER_DEPTH z02[RENDER_H];
	static inline Z_BUFFER_DEPTH z012[RENDER_H];
	static inline Z_BUFFER_DEPTH zValues[RENDER_W];

	static void verticesWorldToView(Vec3f *inVertices, uint32_t &verticesCount, Transform &transform, Vec4f *outVertices);
	static void verticesViewToScreen(Vec4f *inVertices, uint32_t &verticesCount, ScreenTriangle *outVertices);
	static void cullBackFaces(Vec4f *inVertices, Triangle *inTriangles, uint32_t &trianglesCount, Triangle *outTriangles, uint32_t &outCount);
	static void clipFaces(Vec4f *inVertices, Triangle *inTriangles, uint32_t &verticesCount, uint32_t &trianglesCount, Triangle *outTriangles, uint32_t &outTrianglesCount);
	static void clearScreen();
	static void interpolate(int i0, float d0, int i1, float d1, float *values, int& count);
	static void intersectionWithPlane(Vec3f &startPositive, Vec3f &endNegative, Vec3f &planeNormal, float &planeD, Vec3f &intersectPoint);


	static inline float edgeFunction(const Vec3f &a, const Vec3f &b, const Vec3f &c)
	{
		return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
	}
};
