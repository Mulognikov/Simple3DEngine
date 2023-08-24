#pragma once
#include "../Math/Vec2i.h"
#include "../Math/Vec3i.h"
#include "Mesh.h"
#include "Transform.h"

#define RENDER_W 320
#define RENDER_H 240
#define PIXELS_COUNT RENDER_W * RENDER_H
#define ASPECT_RATIO ((float)RENDER_H / (float)RENDER_W)
#define Z_BUFFER_DEPTH uint32_t

class Render
{
public:
	static void initialize();
	static void drawPoint(uint16_t x, uint16_t y, uint8_t color);
	static void drawLine(Vec2i p0, Vec2i p1, uint8_t color);
	static void drawLineFast(Vec2i p0, Vec2i p1, uint8_t color);
	static void drawTriangleWire(Vec2i t0, Vec2i t1, Vec2i t2, uint8_t color);
	static void drawTriangleHalfSpace(Vec3i t0, Vec3i t1, Vec3i t2, uint8_t color);
	static void drawMesh(Mesh &mesh, Transform &transform);
	static void render();

private:
	Render() = default;
	static inline uint8_t* frameBuffer = nullptr;
	static inline Z_BUFFER_DEPTH * zBuffer = nullptr;

	static void clearScreen();

	static inline float edgeFunction(const Vec3f &a, const Vec3f &b, const Vec3f &c)
	{
		return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
	}
};
