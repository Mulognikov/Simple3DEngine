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
	static void drawLineFast(Vec2i p0, Vec2i p1, uint8_t color);
	static void drawTriangleWire(Vec2i t0, Vec2i t1, Vec2i t2, uint8_t color);
	static void drawTriangleFill(Vec3i t0, Vec3i t1, Vec3i t2, uint8_t color);
	static void drawMesh(Mesh &mesh, Transform &transform);
	static void render();

private:
	Render() = default;
	static inline uint8_t frameBuffer[PIXELS_COUNT];
	static inline Z_BUFFER_DEPTH zBuffer[PIXELS_COUNT];
	static inline uint16_t x02[RENDER_H + RENDER_W];
	static inline uint16_t x012[(RENDER_H + RENDER_W) * 2];

	static void clearScreen();
	static void interpolate(int i0, float d0, int i1, float d1, uint16_t *values, int* count);

	static inline float edgeFunction(const Vec3f &a, const Vec3f &b, const Vec3f &c)
	{
		return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
	}
};
