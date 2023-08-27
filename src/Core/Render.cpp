#include "../Platform/Platform.h"
#include "../Math/Mat4x4.h"
#include "Camera.h"
#include "Render.h"


void Render::initialize()
{
	clearScreen();
}

void Render::drawPoint(uint16_t x, uint16_t y, uint8_t color)
{
//	if (x >= RENDER_W) return;
//	if (y >= RENDER_H) return;
//
//	if (x < 0) return;
//	if (y < 0) return;

	frameBuffer[y * RENDER_W + x] = color;
}

void Render::drawLineFast(Vec2i p0, Vec2i p1, uint8_t color)
{
	bool yLonger = false;
	int incrementVal, endVal;
	int shortLen = p1.y - p0.y;
	int longLen = p1.x - p0.x;

	if (abs(shortLen) > abs(longLen))
	{
		int swap = shortLen;
		shortLen = longLen;
		longLen = swap;
		yLonger = true;
	}

	endVal = longLen;

	if (longLen < 0)
	{
		incrementVal = -1;
		longLen = -longLen;
	}
	else incrementVal = 1;

	int decInc;
	if (longLen == 0) decInc = 0;
	else decInc = (shortLen << 16) / longLen;

	int j = 0;
	if (yLonger)
	{
		for (int i = 0; i != endVal; i += incrementVal)
		{
			drawPoint(p0.x + (j >> 16),p0.y + i, color);
			j += decInc;
		}
	}
	else
	{
		for (int i = 0; i != endVal; i += incrementVal)
		{
			drawPoint(p0.x + i,p0.y + (j >> 16), color);
			j += decInc;
		}
	}
}

void Render::drawTriangleWire(Vec2i t0, Vec2i t1, Vec2i t2, uint8_t color)
{
	drawLineFast(t0, t1, color);
	drawLineFast(t1, t2, color);
	drawLineFast(t0, t2, color);
}

void Render::drawTriangleFill(Vec3i t0, Vec3i t1, Vec3i t2, uint8_t color)
{
	if (t1.y < t0.y) std::swap(t1, t0);
	if (t2.y < t0.y) std::swap(t2, t0);
	if (t2.y < t1.y) std::swap(t2, t1);

	int count;

	interpolate(t0.y, t0.x, t1.y, t1.x, x012, &count);
	interpolate(t1.y, t1.x, t2.y, t2.x, x012 + count - 1, &count);
	interpolate(t0.y, t0.x, t2.y, t2.x, x02, &count);

	uint16_t* x_left;
	uint16_t* x_right;

	int m = count / 2;

	if (x02[m] < x012[m])
	{
		x_left = x02;
		x_right = x012;
	}
	else
	{
		x_left = x012;
		x_right = x02;
	}

	Vec3f s1 = t1 - t0;
	Vec3f s2 = t2 - t1;
	Vec3f s = Vec3f::crossProduct(s1, s2);
	float d = -(s.x * t0.x + s.y * t0.y + s.z * t0.z);

	for (int y = t0.y; y < t2.y; y++)
	{
		int start = x_left[y - t0.y];
		int end = x_right[y - t0.y];

		for (int x = start; x < end; x++)
		{
			int z = std::round(-(s.x * x + s.y * y + d) / s.z);

			if (zBuffer[y * RENDER_W + x] > z)
			{
				zBuffer[y * RENDER_W + x] = z;

				drawPoint(x, y, color);
			}
		}

		//std::memset(frameBuffer + (y * RENDER_W + start), color, sizeof(frameBuffer[0]) * abs(end - start));
	}
}

void Render::drawMesh(Mesh &mesh, Transform &transform)
{
	Mat4x4 translate = Mat4x4::translation(transform.position);
	Mat4x4 matRotX = Mat4x4::rotationMatrix_X(transform.rotation.x * 3.14159f / 180);
	Mat4x4 matRotY = Mat4x4::rotationMatrix_Y(transform.rotation.y * 3.14159f / 180);
	Mat4x4 matRotZ = Mat4x4::rotationMatrix_Z(transform.rotation.z * 3.14159f / 180);
	Mat4x4 scale = Mat4x4::scale(transform.scale);

	Mat4x4 transformed = scale * matRotX * matRotY * matRotZ * translate;

	for (int i = 0; i < mesh.faces.size(); i++)
	{
		Vec3f transformedTri[3];
		Vec3i screenPos[3];

		Vec4f worldPos;
		Vec4f transformedPos;
		Vec4f cameraView;
		Vec4f projectedView;

		bool draw = true;

		for (int j = 0; j < 3; j++)
		{
			//fastObjIndex face = mesh->indices[i * 3 + j];
			int face;

			switch (j)
			{
				case 0:
					face = mesh.faces[i].x;
					break;
				case 1:
					face = mesh.faces[i].y;
					break;
				case 2:
					face = mesh.faces[i].z;
					break;
			}

			worldPos.x = mesh.vertices[face].x;
			worldPos.y = mesh.vertices[face].y;
			worldPos.z = mesh.vertices[face].z;

			transformedPos = Mat4x4::multiplyVector(transformed, worldPos);
			cameraView = Mat4x4::multiplyVector(Camera::viewMatrix, transformedPos);
			projectedView = Mat4x4::multiplyVector(Camera::projectionMatrix, cameraView);

			transformedTri[j] = transformedPos;

			projectedView.x /= projectedView.w;
			projectedView.y /= projectedView.w;
			//projectedView.z /= projectedView.w;

			screenPos[j].x = std::round((projectedView.x + 1.0f) * RENDER_W * 0.5f);
			screenPos[j].y = std::round((projectedView.y + 1.0f) * RENDER_H * 0.5f);
			screenPos[j].z = std::round(((projectedView.z - Camera::near) / (Camera::far - Camera::near)) * std::numeric_limits<Z_BUFFER_DEPTH>::max());

			if (screenPos[j].x > RENDER_W * 1) draw = false;
			if (screenPos[j].x < RENDER_W - RENDER_W * 1) draw = false;
			if (screenPos[j].y > RENDER_H * 1) draw = false;
			if (screenPos[j].y < RENDER_H - RENDER_H * 1) draw = false;
			if (screenPos[j].z <= 0) draw = false;
		}

		if (!draw) continue;

		Vec3f v1 = (transformedTri[1] - transformedTri[0]);
		Vec3f v2 = (transformedTri[2] - transformedTri[0]);
		Vec3f n = Vec3f::crossProduct(v1, v2);
		n = Vec3f::normalize(n);

		Vec3f cameraDir = Camera::transform.position - transformedTri[0];
		Vec3f lightDir(-1, 1, -1);
		lightDir = Vec3f::normalize(lightDir);

		float dot = Vec3f::dotProduct(cameraDir, n);
		float lightDot = Vec3f::dotProduct(lightDir, n);

		uint8_t color = 0b00000100;
		if (lightDot > 0.143f)
		{
			color = 7 * lightDot;
			color = color << 2;
		}

		if (dot > 0)
		{
			drawTriangleFill(screenPos[0], screenPos[1], screenPos[2], color);
		}
		//drawTriangleWire(screenPos[0], screenPos[1], screenPos[2], 255);
	}
}

void Render::render()
{
	Platform::screen->sendToScreen(&frameBuffer[0]);
	clearScreen();
}

void Render::clearScreen()
{
	std::memset(frameBuffer, 0, sizeof(frameBuffer[0]) * PIXELS_COUNT);
	std::memset(zBuffer, std::numeric_limits<Z_BUFFER_DEPTH>::max(), sizeof(zBuffer[0]) * PIXELS_COUNT);
}

void Render::interpolate(int i0, float d0, int i1, float d1, uint16_t *values, int* count)
{
	if (i0 == i1)
	{
		values[0] = d0;
		*count = 1;
		return;
	}

	float a = (d1 - d0) / (i1 - i0);
	float d = d0;

	int index = 0;
	for (int i = i0; i <= i1; i++)
	{
		values[index] = d;
		index++;
		d = d + a;
	}

	*count = index;
}









//void Render::drawTriangleHalfSpace(Vec3i t0, Vec3i t1, Vec3i t2, uint8_t color)
//{
//	Vec2i min(RENDER_W, RENDER_H);
//	Vec2i max(0, 0);
//
//	min.x = std::max(0,        		std::min(min.x, (t0.x)));
//	max.x = std::min(RENDER_W, 	std::max(max.x, (t0.x)));
//	min.y = std::max(0,      	std::min(min.y, (t0.y)));
//	max.y = std::min(RENDER_H, 	std::max(max.y, (t0.y)));
//
//	min.x = std::max(0,      	std::min(min.x, (t1.x)));
//	max.x = std::min(RENDER_W, 	std::max(max.x, (t1.x)));
//	min.y = std::max(0,      	std::min(min.y, (t1.y)));
//	max.y = std::min(RENDER_H, 	std::max(max.y, (t1.y)));
//
//	min.x = std::max(0,      	std::min(min.x, (t2.x)));
//	max.x = std::min(RENDER_W, 	std::max(max.x, (t2.x)));
//	min.y = std::max(0,      	std::min(min.y, (t2.y)));
//	max.y = std::min(RENDER_H, 	std::max(max.y, (t2.y)));
//
//	Vec3f s1 = t1 - t0;
//	Vec3f s2 = t2 - t1;
//	Vec3f s = Vec3f::crossProduct(s1, s2);
//	float d = -(s.x * t0.x + s.y * t0.y + s.z * t0.z);
//
//	for (uint32_t j = min.y; j < max.y; ++j)
//	{
//		for (uint32_t i = min.x; i < max.x; ++i)
//		{
//			Vec3f p = {i + 0.5f, j + 0.5f};
//
//			float w0 = edgeFunction(t1, t2, p);
//			float w1 = edgeFunction(t2, t0, p);
//			float w2 = edgeFunction(t0, t1, p);
//
//			if (w0 >= 0 && w1 >= 0 && w2 >= 0)
//			{
//				//int z = std::round((t0.z + t1.z + t2.z) / 3.0f);
//				int z = std::round(-(s.x * i + s.y * j + d) / s.z);
//
//				if (zBuffer[j * RENDER_W + i] > z)
//				{
//					zBuffer[j * RENDER_W + i] = z;
//
//					drawPoint(i, j, color);
//				}
//
//			}
//		}
//	}
//}



//void Render::drawLine(Vec2i p0, Vec2i p1, uint8_t color)
//{
//	bool steep = false;
//	int dx = std::abs(p1.x - p0.x);
//	int dy = std::abs(p1.y - p0.y);
//
//	if (dx < dy)
//	{
//		std::swap(p0.x, p0.y);
//		std::swap(p1.x, p1.y);
//		steep = true;
//	}
//
//	if (p0.x > p1.x)
//	{
//		std::swap(p0.x, p1.x);
//		std::swap(p0.y, p1.y);
//	}
//
//	dx = std::abs(p1.x - p0.x);
//	dy = std::abs(p1.y - p0.y);
//
//	int step = dy * 2;
//	int y = p0.y;
//	int nextY = 0;
//
//	for (int x = p0.x; x <= p1.x; x++)
//	{
//		if (steep)
//		{
//			drawPoint(y, x, color);
//		}
//		else
//		{
//			drawPoint(x, y, color);
//		}
//
//		if (nextY > dx)
//		{
//			if (p1.y > p0.y) y++;
//			else y--;
//
//			nextY -= dx * 2;
//		}
//
//		nextY += step;
//	}
//}


//
//void Render::drawLine2(Vec2i p0, Vec2i p1, uint8_t color)
//{
//	uint16_t values[1000];
//	int count = 0;
//
//	if (std::abs(p1.x - p0.x) > std::abs(p1.y - p0.y))
//	{
//		if (p0.x > p1.x)
//		{
//			std::swap(p0, p1);
//		}
//
//		interpolate(p0.x, p0.y, p1.x, p1.y, values, &count);
//
//		for (int i = p0.x; i <= p1.x; i++)
//		{
//			drawPoint(i, values[i - p0.x], color);
//		}
//	}
//	else
//	{
//		if (p0.y > p1.y)
//		{
//			std::swap(p0, p1);
//		}
//
//		interpolate(p0.y, p0.x, p1.y, p1.x, values, &count);
//		for (int i = p0.y; i <= p1.y; i++)
//		{
//			drawPoint(values[i - p0.y], i, color);
//		}
//	}
//}



//old render
//auto elapsed = std::chrono::high_resolution_clock::now() - start;
//long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
//float time = microseconds / 1000000.0f;
//
////time = 0;
//
//Mat4x4 matRotX = Mat4x4::rotationMatrix_X(time);
//Mat4x4 matRotY = Mat4x4::rotationMatrix_Y(time);
//Mat4x4 matRotZ = Mat4x4::rotationMatrix_Z(time);
//
//Mat4x4 translate = Mat4x4::translation(0, 0, 2);
//Mat4x4 transformed = matRotX * matRotY * matRotZ * translate;
//
//for (int i = 0; i < mesh->face_count; i++)
//{
//Vec3f transformedTri[3];
//Vec3i screenPos[3];
//
//Vec4f worldPos;
//Vec4f transformedPos;
//Vec4f projectedPos;
//
//for (int j = 0; j < 3; j++)
//{
//fastObjIndex face = mesh->indices[i * 3 + j];
//
//worldPos.x = mesh->positions[face.p * 3];
//worldPos.y = mesh->positions[face.p * 3 + 1];
//worldPos.z = mesh->positions[face.p * 3 + 2];
//
//transformedPos = Mat4x4::multiplyVector(transformed, worldPos);
//transformedTri[j] = transformedPos;
//projectedPos = Mat4x4::multiplyVector(projectionMatrix,transformedPos);
//
//projectedPos.x /= projectedPos.w;
//projectedPos.y /= projectedPos.w;
////projectedPos.z /= projectedPos.w;
//
//screenPos[j].x = (projectedPos.x + 1.0f) * width * 0.5f;
//screenPos[j].y = (projectedPos.y + 1.0f) * height * 0.5f;
//screenPos[j].z = ((projectedPos.z - near) / (far - near)) * 255;
//
//if (screenPos[j].x > width - 1) screenPos[j].x = width - 1;
//if (screenPos[j].x < 0) screenPos[j].x = 0;
//if (screenPos[j].y > height - 1) screenPos[j].y = height - 1;
//if (screenPos[j].y < 0) screenPos[j].y = 0;
//}
//
//Vec3f v1 = (transformedTri[1] - transformedTri[0]);
//Vec3f v2 = (transformedTri[2] - transformedTri[0]);
//Vec3f n = Vec3f::crossProduct(v1, v2);
//n = Vec3f::normalize(n);
//
////			{	//draw normals
////				Vec3f t0 = (translatedTri[0] + translatedTri[1] + translatedTri[2]) * 0.33f;
////				Vec3f t1 = t0 + n * 0.1f;
////
////				Vec3f p0, p1;
////
////				Mat4x4::multiplyMatrixVector(projectionMatrix, t0, p0);
////				Mat4x4::multiplyMatrixVector(projectionMatrix, t1, p1);
////
////				Vec2i s0((p0.x + 1.0f) * width * 0.5f, (p0.y + 1.0f) * height  * 0.5f);
////				Vec2i s1((p1.x + 1.0f) * width  * 0.5f, (p1.y + 1.0f) * height * 0.5f);
////
////				renderer.drawLine(s0, s1, 224);
////			}
//
//
//Vec3f cameraDir = Vec3f() - transformedTri[0];
//Vec3f lightDir(-1, 1, -1);
//lightDir = Vec3f::normalize(lightDir);
//
//float dot = Vec3f::dotProduct(cameraDir, n);
//float lightDot = Vec3f::dotProduct(lightDir, n);
//
//uint8_t color = 0b00000100;
//if (lightDot > 0.143f)
//{
//color = 7 * lightDot;
//color = color << 2;
//}
//
//if (dot > 0)
//{
//renderer.drawTriangleHalfSpace(screenPos[0], screenPos[1], screenPos[2], color);
//}
////renderer.drawTriangleWire(screenPos[0], screenPos[1], screenPos[2], 255);
//}
//
//renderer.render();