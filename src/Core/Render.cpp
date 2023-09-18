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

void Render::drawLineFast(ScreenTriangle p0, ScreenTriangle p1, uint8_t color)
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

void Render::drawTriangleWire(ScreenTriangle t0, ScreenTriangle t1, ScreenTriangle t2, uint8_t color)
{
	drawLineFast(t0, t1, color);
	drawLineFast(t1, t2, color);
	drawLineFast(t0, t2, color);
}

void Render::drawTriangleFill(ScreenTriangle t0, ScreenTriangle t1, ScreenTriangle t2, uint8_t color)
{
	if (t1.y < t0.y) std::swap(t1, t0);
	if (t2.y < t0.y) std::swap(t2, t0);
	if (t2.y < t1.y) std::swap(t2, t1);

	int count;

	interpolateInScreenBounds(t0.y, t0.x, t1.y, t1.x, x012, count, 0);
	interpolateInScreenBounds(t1.y, t1.x, t2.y, t2.x, x012 + count - 1, count, 1);
	interpolateInScreenBounds(t0.y, t0.x, t2.y, t2.x, x02, count, 0);

	interpolateInScreenBounds(t0.y, 1.0f/t0.z, t1.y, 1.0f/t1.z, z012, count, 0);
	interpolateInScreenBounds(t1.y, 1.0f/t1.z, t2.y, 1.0f/t2.z, z012 + count - 1, count, 1);
	interpolateInScreenBounds(t0.y, 1.0f/t0.z, t2.y, 1.0f/t2.z, z02, count, 0);


	float* x_left;
	float* x_right;

	Z_BUFFER_DEPTH* z_left;
	Z_BUFFER_DEPTH* z_right;

	int m = count / 2;

	if (x02[m] < x012[m])
	{
		x_left = x02;
		x_right = x012;
		z_left = z02;
		z_right = z012;
	}
	else
	{
		x_left = x012;
		x_right = x02;
		z_left = z012;
		z_right = z02;
	}

	int yStart = clamp(t0.y, 0, RENDER_H);
	int yEnd = clamp(std::ceil(t2.y), 0, RENDER_H);

	for (int y = yStart; y < yEnd; y++)
	{
		int start = std::floor(x_left[y - yStart]);
		int end = std::ceil(x_right[y - yStart]);

		if (start > end) std::swap(start, end);

		start = clamp(start, 0, RENDER_W);
		end = clamp(end, 0, RENDER_W);

		Z_BUFFER_DEPTH startZ = z_left[y - yStart];
		Z_BUFFER_DEPTH endZ = z_right[y - yStart];

		int countZ;

		interpolate(start, startZ, end, endZ, zValues, countZ);

		for (int x = start; x < end; x++)
		{
			Z_BUFFER_DEPTH z = zValues[x - start];

			if (zBuffer[y * RENDER_W + x] < z)
			{
				zBuffer[y * RENDER_W + x] = z;
				drawPoint(x, y, color);

				if (Platform::input->keyPressed(90))
				{
					int zColor = z * 600;
					drawPoint(x, y, zColor);
				}
			}

		}
		//std::memset(frameBuffer + (y * RENDER_W + start), color, sizeof(frameBuffer[0]) * abs(end - start));
	}
}

void Render::verticesWorldToView(Vec3f *inVertices, uint32_t &verticesCount, Transform &transform, Vec4f *outVertices)
{
	Mat4x4 translate = Mat4x4::translation(transform.position);
	Mat4x4 matRotX = Mat4x4::rotationMatrix_X(transform.rotation.x * 3.14159f / 180);
	Mat4x4 matRotY = Mat4x4::rotationMatrix_Y(transform.rotation.y * 3.14159f / 180);
	Mat4x4 matRotZ = Mat4x4::rotationMatrix_Z(transform.rotation.z * 3.14159f / 180);
	Mat4x4 scale = Mat4x4::scale(transform.scale);

	Mat4x4 transformed = scale * matRotX * matRotY * matRotZ * translate * Camera::viewMatrix;

	for (int i = 1; i < verticesCount; i++)
	{
		Vec4f worldPos = inVertices[i];
		outVertices[i] = Mat4x4::multiplyVector(transformed, worldPos);
	}
}

void Render::verticesViewToScreen(Vec4f *inVertices, uint32_t &verticesCount, ScreenTriangle *outVertices)
{
	for (int i = 1; i < verticesCount; i++)
	{
		Vec4f projected = Mat4x4::multiplyVector(Camera::projectionMatrix, inVertices[i]);

		projected.x /= projected.w;
		projected.y /= projected.w;

		outVertices[i].x = (projected.x + 1.0f) * (RENDER_W / 2);
		outVertices[i].y = (projected.y + 1.0f) * (RENDER_H / 2);
		outVertices[i].z = inVertices[i].z;
	}
}

void Render::cullBackFaces(Vec4f *inVertices, Triangle *inTriangles, uint32_t &trianglesCount, Triangle *outTriangles, uint32_t &outCount)
{
	outCount = 0;

	for (int i = 0; i < trianglesCount; i++)
	{
		Vec3f v1 = inVertices[inTriangles[i].face[1]] - inVertices[inTriangles[i].face[0]];
		Vec3f v2 = inVertices[inTriangles[i].face[2]] - inVertices[inTriangles[i].face[0]];
		Vec3f n = Vec3f::crossProduct(v1, v2);

		Vec3f cameraDir = Vec3f(0, 0, 0) - inVertices[inTriangles[i].face[0]];
		float dot = Vec3f::dotProduct(cameraDir, n);

		if (dot > 0)
		{
			outTriangles[outCount] = inTriangles[i];
			outCount++;
		}
	}
}

void Render::clipFaces(Vec4f *inVertices, Triangle *inTriangles, uint32_t &verticesCount, uint32_t &trianglesCount, Triangle *outTriangles, uint32_t &outTrianglesCount)
{
	outTrianglesCount = 0;
	uint8_t clipCase = 0;

	for (int i = 0; i < trianglesCount; i++)
	{
		Triangle needCheckFaces[100];
		uint8_t needCheckFacesCount = 0;

		Triangle newFaces[100];
		newFaces[0] = inTriangles[i];
		uint8_t newFacesCount = 1;

		for (int j = 0; j < 6; j++)
		{
			std::copy(newFaces, &newFaces[newFacesCount], needCheckFaces);
			needCheckFacesCount = newFacesCount;
			newFacesCount = 0;

			for (int k = 0; k < needCheckFacesCount; k++)
			{
				uint8_t d0 = !std::signbit(Vec3f::dotProduct(Camera::clipPlaneNormals[j], inVertices[needCheckFaces[k].face[0]]) - Camera::clipPlaneD[j]);
				uint8_t d1 = !std::signbit(Vec3f::dotProduct(Camera::clipPlaneNormals[j], inVertices[needCheckFaces[k].face[1]]) - Camera::clipPlaneD[j]);
				uint8_t d2 = !std::signbit(Vec3f::dotProduct(Camera::clipPlaneNormals[j], inVertices[needCheckFaces[k].face[2]]) - Camera::clipPlaneD[j]);

				clipCase = d0;
				clipCase = clipCase << 1;
				clipCase = clipCase | d1;
				clipCase = clipCase << 1;
				clipCase = clipCase | d2;

				switch (clipCase)
				{
					case 0:
						continue;

					case 7:
						newFaces[newFacesCount] = needCheckFaces[k];
						newFacesCount++;
						break;

					case 1:
					{
						Vec3f a = inVertices[needCheckFaces[k].face[2]];
						Vec3f b = inVertices[needCheckFaces[k].face[0]];
						Vec3f c = inVertices[needCheckFaces[k].face[1]];

						Vec3f intersectAB, intersectAC;
						intersectionWithPlane(a, b, Camera::clipPlaneNormals[j], Camera::clipPlaneD[j], intersectAB);
						intersectionWithPlane(a, c, Camera::clipPlaneNormals[j], Camera::clipPlaneD[j], intersectAC);

						inVertices[verticesCount] = intersectAB;
						newFaces[newFacesCount].face[0] = verticesCount;
						verticesCount++;

						inVertices[verticesCount] = intersectAC;
						newFaces[newFacesCount].face[1] = verticesCount;
						verticesCount++;

						newFaces[newFacesCount].face[2] = needCheckFaces[k].face[2];
						newFacesCount++;
						break;
					}

					case 2:
					{
						Vec3f a = inVertices[needCheckFaces[k].face[1]];
						Vec3f b = inVertices[needCheckFaces[k].face[0]];
						Vec3f c = inVertices[needCheckFaces[k].face[2]];

						Vec3f intersectAB, intersectAC;
						intersectionWithPlane(a, b, Camera::clipPlaneNormals[j], Camera::clipPlaneD[j], intersectAB);
						intersectionWithPlane(a, c, Camera::clipPlaneNormals[j], Camera::clipPlaneD[j], intersectAC);

						inVertices[verticesCount] = intersectAB;
						newFaces[newFacesCount].face[0] = verticesCount;
						verticesCount++;

						inVertices[verticesCount] = intersectAC;
						newFaces[newFacesCount].face[2] = verticesCount;
						verticesCount++;

						newFaces[newFacesCount].face[1] = needCheckFaces[k].face[1];
						newFacesCount++;
						break;
					}
					case 4:
					{
						Vec3f a = inVertices[needCheckFaces[k].face[0]];
						Vec3f b = inVertices[needCheckFaces[k].face[1]];
						Vec3f c = inVertices[needCheckFaces[k].face[2]];

						Vec3f intersectAB, intersectAC;
						intersectionWithPlane(a, b, Camera::clipPlaneNormals[j], Camera::clipPlaneD[j], intersectAB);
						intersectionWithPlane(a, c, Camera::clipPlaneNormals[j], Camera::clipPlaneD[j], intersectAC);

						inVertices[verticesCount] = intersectAB;
						newFaces[newFacesCount].face[1] = verticesCount;
						verticesCount++;

						inVertices[verticesCount] = intersectAC;
						newFaces[newFacesCount].face[2] = verticesCount;
						verticesCount++;

						newFaces[newFacesCount].face[0] = needCheckFaces[k].face[0];
						newFacesCount++;
						break;
					}
					case 3:
					{
						Vec3f a = inVertices[needCheckFaces[k].face[1]];
						Vec3f b = inVertices[needCheckFaces[k].face[2]];
						Vec3f c = inVertices[needCheckFaces[k].face[0]];

						Vec3f intersectAC, intersectBC;
						intersectionWithPlane(a, c, Camera::clipPlaneNormals[j], Camera::clipPlaneD[j], intersectAC);
						intersectionWithPlane(b, c, Camera::clipPlaneNormals[j], Camera::clipPlaneD[j], intersectBC);

						newFaces[newFacesCount].face[1] = needCheckFaces[k].face[1];
						newFaces[newFacesCount].face[2] = needCheckFaces[k].face[2];
						inVertices[verticesCount] = intersectAC;
						newFaces[newFacesCount].face[0] = verticesCount;

						newFacesCount++;

						newFaces[newFacesCount].face[1] = verticesCount;
						verticesCount++;

						inVertices[verticesCount] = intersectBC;
						newFaces[newFacesCount].face[0] = verticesCount;
						verticesCount++;

						newFaces[newFacesCount].face[2] = needCheckFaces[k].face[2];
						newFacesCount++;
						break;
					}
					case 5:
					{
						Vec3f a = inVertices[needCheckFaces[k].face[0]];
						Vec3f b = inVertices[needCheckFaces[k].face[2]];
						Vec3f c = inVertices[needCheckFaces[k].face[1]];

						Vec3f intersectAC, intersectBC;
						intersectionWithPlane(a, c, Camera::clipPlaneNormals[j], Camera::clipPlaneD[j], intersectAC);
						intersectionWithPlane(b, c, Camera::clipPlaneNormals[j], Camera::clipPlaneD[j], intersectBC);

						newFaces[newFacesCount].face[0] = needCheckFaces[k].face[0];
						newFaces[newFacesCount].face[2] = needCheckFaces[k].face[2];
						inVertices[verticesCount] = intersectAC;
						newFaces[newFacesCount].face[1] = verticesCount;

						newFacesCount++;

						newFaces[newFacesCount].face[0] = verticesCount;
						verticesCount++;

						inVertices[verticesCount] = intersectBC;
						newFaces[newFacesCount].face[1] = verticesCount;
						verticesCount++;

						newFaces[newFacesCount].face[2] = needCheckFaces[k].face[2];
						newFacesCount++;
						break;
					}

					case 6:
					{
						Vec3f a = inVertices[needCheckFaces[k].face[0]];
						Vec3f b = inVertices[needCheckFaces[k].face[1]];
						Vec3f c = inVertices[needCheckFaces[k].face[2]];

						Vec3f intersectAC, intersectBC;
						intersectionWithPlane(a, c, Camera::clipPlaneNormals[j], Camera::clipPlaneD[j], intersectAC);
						intersectionWithPlane(b, c, Camera::clipPlaneNormals[j], Camera::clipPlaneD[j], intersectBC);

						newFaces[newFacesCount].face[0] = needCheckFaces[k].face[0];
						newFaces[newFacesCount].face[1] = needCheckFaces[k].face[1];
						inVertices[verticesCount] = intersectAC;
						newFaces[newFacesCount].face[2] = verticesCount;

						newFacesCount++;

						newFaces[newFacesCount].face[0] = verticesCount;
						verticesCount++;

						inVertices[verticesCount] = intersectBC;
						newFaces[newFacesCount].face[2] = verticesCount;
						verticesCount++;

						newFaces[newFacesCount].face[1] = needCheckFaces[k].face[1];
						newFacesCount++;
						break;
					}

				}
			}
		}

		if (newFacesCount > 0)
		{
			std::copy(&newFaces[0], &newFaces[0] + newFacesCount, &outTriangles[outTrianglesCount]);
			outTrianglesCount += newFacesCount;
		}
	}
}


void Render::clipFaceZ(Vec4f *inVertices, uint32_t &verticesCount, Triangle &inTriangle, Triangle *outTriangle, uint8_t &outTrianglesCount)
{
	outTrianglesCount = 0;
	Vec3f zPlane = Vec3f(0, 0, 1);

	uint8_t d0 = !std::signbit(Vec3f::dotProduct(zPlane, inVertices[inTriangle.face[0]]) - Camera::near);
	uint8_t d1 = !std::signbit(Vec3f::dotProduct(zPlane, inVertices[inTriangle.face[1]]) - Camera::near);
	uint8_t d2 = !std::signbit(Vec3f::dotProduct(zPlane, inVertices[inTriangle.face[2]]) - Camera::near);

	uint8_t clipCase = d0;
	clipCase = clipCase << 1;
	clipCase = clipCase | d1;
	clipCase = clipCase << 1;
	clipCase = clipCase | d2;

	switch (clipCase)
	{
		case 0:
			return;

		case 7:
			outTriangle[outTrianglesCount] = inTriangle;
			outTrianglesCount++;
			return;

		case 1:
		{
			Vec4f a = inVertices[inTriangle.face[2]];
			Vec4f b = inVertices[inTriangle.face[0]];
			Vec4f c = inVertices[inTriangle.face[1]];

			Vec4f intersectAB, intersectAC;
			intersectionWithPlane(a, b, zPlane, Camera::near, intersectAB);
			intersectionWithPlane(a, c, zPlane, Camera::near, intersectAC);

			inVertices[verticesCount] = intersectAB;
			outTriangle[outTrianglesCount].face[0] = verticesCount;
			verticesCount++;

			inVertices[verticesCount] = intersectAC;
			outTriangle[outTrianglesCount].face[1] = verticesCount;
			verticesCount++;

			outTriangle[outTrianglesCount].face[2] = inTriangle.face[2];
			outTrianglesCount++;
			return;
		}

		case 2:
		{
			Vec4f a = inVertices[inTriangle.face[1]];
			Vec4f b = inVertices[inTriangle.face[0]];
			Vec4f c = inVertices[inTriangle.face[2]];

			Vec4f intersectAB, intersectAC;
			intersectionWithPlane(a, b, zPlane, Camera::near, intersectAB);
			intersectionWithPlane(a, c, zPlane, Camera::near, intersectAC);

			inVertices[verticesCount] = intersectAB;
			outTriangle[outTrianglesCount].face[0] = verticesCount;
			verticesCount++;

			inVertices[verticesCount] = intersectAC;
			outTriangle[outTrianglesCount].face[2] = verticesCount;
			verticesCount++;

			outTriangle[outTrianglesCount].face[1] = inTriangle.face[1];
			outTrianglesCount++;
			return;
		}
		case 4:
		{
			Vec4f a = inVertices[inTriangle.face[0]];
			Vec4f b = inVertices[inTriangle.face[1]];
			Vec4f c = inVertices[inTriangle.face[2]];

			Vec4f intersectAB, intersectAC;
			intersectionWithPlane(a, b, zPlane, Camera::near, intersectAB);
			intersectionWithPlane(a, c, zPlane, Camera::near, intersectAC);

			inVertices[verticesCount] = intersectAB;
			outTriangle[outTrianglesCount].face[1] = verticesCount;
			verticesCount++;

			inVertices[verticesCount] = intersectAC;
			outTriangle[outTrianglesCount].face[2] = verticesCount;
			verticesCount++;

			outTriangle[outTrianglesCount].face[0] = inTriangle.face[0];
			outTrianglesCount++;
			return;
		}
		case 3:
		{
			Vec4f a = inVertices[inTriangle.face[1]];
			Vec4f b = inVertices[inTriangle.face[2]];
			Vec4f c = inVertices[inTriangle.face[0]];

			Vec4f intersectAC, intersectBC;
			intersectionWithPlane(a, c, zPlane, Camera::near, intersectAC);
			intersectionWithPlane(b, c, zPlane, Camera::near, intersectBC);

			outTriangle[outTrianglesCount].face[1] = inTriangle.face[1];
			outTriangle[outTrianglesCount].face[2] = inTriangle.face[2];
			inVertices[verticesCount] = intersectAC;
			outTriangle[outTrianglesCount].face[0] = verticesCount;

			outTrianglesCount++;

			outTriangle[outTrianglesCount].face[1] = verticesCount;
			verticesCount++;

			inVertices[verticesCount] = intersectBC;
			outTriangle[outTrianglesCount].face[0] = verticesCount;
			verticesCount++;

			outTriangle[outTrianglesCount].face[2] = inTriangle.face[2];
			outTrianglesCount++;
			return;
		}
		case 5:
		{
			Vec4f a = inVertices[inTriangle.face[0]];
			Vec4f b = inVertices[inTriangle.face[2]];
			Vec4f c = inVertices[inTriangle.face[1]];

			Vec4f intersectAC, intersectBC;
			intersectionWithPlane(a, c, zPlane, Camera::near, intersectAC);
			intersectionWithPlane(b, c, zPlane, Camera::near, intersectBC);

			outTriangle[outTrianglesCount].face[0] = inTriangle.face[0];
			outTriangle[outTrianglesCount].face[2] = inTriangle.face[2];
			inVertices[verticesCount] = intersectAC;
			outTriangle[outTrianglesCount].face[1] = verticesCount;

			outTrianglesCount++;

			outTriangle[outTrianglesCount].face[0] = verticesCount;
			verticesCount++;

			inVertices[verticesCount] = intersectBC;
			outTriangle[outTrianglesCount].face[1] = verticesCount;
			verticesCount++;

			outTriangle[outTrianglesCount].face[2] = inTriangle.face[2];
			outTrianglesCount++;
			return;
		}

		case 6:
		{
			Vec4f a = inVertices[inTriangle.face[0]];
			Vec4f b = inVertices[inTriangle.face[1]];
			Vec4f c = inVertices[inTriangle.face[2]];

			Vec4f intersectAC, intersectBC;
			intersectionWithPlane(a, c, zPlane, Camera::near, intersectAC);
			intersectionWithPlane(b, c, zPlane, Camera::near, intersectBC);

			outTriangle[outTrianglesCount].face[0] = inTriangle.face[0];
			outTriangle[outTrianglesCount].face[1] = inTriangle.face[1];
			inVertices[verticesCount] = intersectAC;
			outTriangle[outTrianglesCount].face[2] = verticesCount;

			outTrianglesCount++;

			outTriangle[outTrianglesCount].face[0] = verticesCount;
			verticesCount++;

			inVertices[verticesCount] = intersectBC;
			outTriangle[outTrianglesCount].face[2] = verticesCount;
			verticesCount++;

			outTriangle[outTrianglesCount].face[1] = inTriangle.face[1];
			outTrianglesCount++;
			return;
		}
	}
}

void Render::drawMesh(Mesh &mesh, Transform &transform)
{
	uint32_t verticesCount = mesh.verticesCount;
	uint32_t facesCount = mesh.trianglesCount;
	uint32_t newTrianglesCount = 0;
	uint32_t cullFacesCount = 0;

	verticesWorldToView(&mesh.vertices[0], verticesCount, transform, viewVertices);
	cullBackFaces(viewVertices, mesh.triangles, facesCount, cullFaces, cullFacesCount);
	clipFaces(viewVertices, cullFaces, verticesCount, cullFacesCount, newTriangles, newTrianglesCount);
	verticesViewToScreen(viewVertices, verticesCount, screenPositions);

	for(int i = 0; i < newTrianglesCount; i++)
	{
		Vec3f triangleView[3];
		ScreenTriangle triangle[3];

		triangleView[0] = viewVertices[newTriangles[i].face[0]];
		triangleView[1] = viewVertices[newTriangles[i].face[1]];
		triangleView[2] = viewVertices[newTriangles[i].face[2]];

		triangle[0] = screenPositions[newTriangles[i].face[0]];
		triangle[1] = screenPositions[newTriangles[i].face[1]];
		triangle[2] = screenPositions[newTriangles[i].face[2]];

		Vec3f v1 = (triangleView[1] - triangleView[0]);
		Vec3f v2 = (triangleView[2] - triangleView[0]);
		Vec3f n = Vec3f::crossProduct(v1, v2);
		n = Vec3f::normalize(n);

		Vec3f lightDir = Camera::transform.position - Vec3f(0, 0, 1);
		Mat4x4 l = Camera::viewMatrix * Camera::projectionMatrix;
		lightDir = Mat4x4::multiplyVector(l, lightDir);
		lightDir = Vec3f::normalize(lightDir);

		float lightDot = Vec3f::dotProduct(lightDir, n);

		uint8_t color = 0b00000100;
		if (lightDot > 0.143f)
		{
			color = 7 * lightDot;
			color = color << 2;
		}

		drawTriangleFill(triangle[0], triangle[1], triangle[2], color);

		if (Platform::input->keyPressed(88))
		{
			drawTriangleWire(triangle[0], triangle[1], triangle[2], 0b11100000);
		}
	}
}

void Render::drawMesh2(Mesh &mesh, Transform &transform)
{
	Mat4x4 translate = Mat4x4::translation(transform.position);
	Mat4x4 matRotX = Mat4x4::rotationMatrix_X(transform.rotation.x * 3.14159f / 180);
	Mat4x4 matRotY = Mat4x4::rotationMatrix_Y(transform.rotation.y * 3.14159f / 180);
	Mat4x4 matRotZ = Mat4x4::rotationMatrix_Z(transform.rotation.z * 3.14159f / 180);
	Mat4x4 scale = Mat4x4::scale(transform.scale);

	Mat4x4 transformed = scale * matRotX * matRotY * matRotZ * translate * Camera::viewMatrix * Camera::projectionMatrix;

	for (int v = 1; v < mesh.verticesCount; v++)
	{
		projectedVertices[v] = Mat4x4::multiplyVector(transformed, mesh.vertices[v]);
	}

	for (int f = 0; f < mesh.trianglesCount; f++)
	{
		if (projectedVertices[mesh.triangles[f].face[0]].x < -projectedVertices[mesh.triangles[f].face[0]].w &&
			projectedVertices[mesh.triangles[f].face[1]].x < -projectedVertices[mesh.triangles[f].face[1]].w &&
			projectedVertices[mesh.triangles[f].face[2]].x < -projectedVertices[mesh.triangles[f].face[2]].w)
			continue;

		if (projectedVertices[mesh.triangles[f].face[0]].x > projectedVertices[mesh.triangles[f].face[0]].w &&
			projectedVertices[mesh.triangles[f].face[1]].x > projectedVertices[mesh.triangles[f].face[1]].w &&
			projectedVertices[mesh.triangles[f].face[2]].x > projectedVertices[mesh.triangles[f].face[2]].w)
			continue;

		if (projectedVertices[mesh.triangles[f].face[0]].y < -projectedVertices[mesh.triangles[f].face[0]].w &&
			projectedVertices[mesh.triangles[f].face[1]].y < -projectedVertices[mesh.triangles[f].face[1]].w &&
			projectedVertices[mesh.triangles[f].face[2]].y < -projectedVertices[mesh.triangles[f].face[2]].w)
			continue;

		if (projectedVertices[mesh.triangles[f].face[0]].y > projectedVertices[mesh.triangles[f].face[0]].w &&
			projectedVertices[mesh.triangles[f].face[1]].y > projectedVertices[mesh.triangles[f].face[1]].w &&
			projectedVertices[mesh.triangles[f].face[2]].y > projectedVertices[mesh.triangles[f].face[2]].w)
			continue;

		Triangle clipZTris[2];
		uint8_t clipZTrisCount;
		uint32_t verticesCount = mesh.verticesCount;

		clipFaceZ(projectedVertices, verticesCount, mesh.triangles[f], clipZTris, clipZTrisCount);

		for (int i = 0; i < clipZTrisCount; i++)
		{
			Triangle t = clipZTris[i];

			Vec3f v1 = projectedVertices[t.face[1]] - projectedVertices[t.face[0]];
			Vec3f v2 = projectedVertices[t.face[2]] - projectedVertices[t.face[0]];
			Vec3f n = Vec3f::crossProduct(v1, v2);

			Vec3f cameraDir = Vec3f() - projectedVertices[t.face[0]];
			float dot = Vec3f::dotProduct(cameraDir, n);

			if (dot <= 0) continue;

			n = Vec3f::normalize(n);
			Vec3f lightDir = Camera::transform.position - Vec3f(0, -0.5f, 1);
			Mat4x4 l = Camera::viewMatrix * Camera::projectionMatrix;
			lightDir = Mat4x4::multiplyVector(l, lightDir);
			lightDir = Vec3f::normalize(lightDir);
			float lightDot = Vec3f::dotProduct(lightDir, n);

			uint8_t color = 0b00000100;
			if (lightDot > 0.143f)
			{
				color = 7 * lightDot;
				color = color << 2;
			}


			ScreenTriangle screenPos[3];

			for (int j = 0; j < 3; j++)
			{
				Vec4f p = projectedVertices[t.face[j]];

				p.x /= p.w;
				p.y /= p.w;

				screenPos[j].x = (p.x + 1.0f) * (RENDER_W / 2);
				screenPos[j].y = (p.y + 1.0f) * (RENDER_H / 2);
				screenPos[j].z = projectedVertices[t.face[j]].z;
			}

			drawTriangleFill(screenPos[0], screenPos[1], screenPos[2], color);
		}
	}

}

void Render::render()
{
	Platform::screen->sendToScreen(&frameBuffer[0]);
	clearScreen();
}

void Render::clearScreen()
{
	std::memset(frameBuffer, 0b00010111, sizeof(frameBuffer[0]) * PIXELS_COUNT);
	std::memset(zBuffer, 0, sizeof(zBuffer[0]) * PIXELS_COUNT);
}

void Render::interpolate(int i0, float d0, int i1, float d1, float *values, int& count)
{
	if (i0 == i1)
	{
		values[0] = d0;
		count = 1;
		return;
	}

	float a = (d1 - d0) / static_cast<float>(i1 - i0);
	float d = d0;

	int index = 0;

	for (int i = i0; i <= i1; i++)
	{
		values[index] = d;
		index++;
		d = d + a;
	}

	count = index;
}

void Render::interpolateInScreenBounds(int i0, float d0, int i1, float d1, float *values, int &count, int offset)
{
	if (i0 == i1)
	{
		values[0] = d0;
		count = 1;
		return;
	}

	int minDif = 0;
	int maxDif = 0;
	int index = 0;

	float a = (d1 - d0) / static_cast<float>(i1 - i0);
	float d = d0;

	if (i1 > RENDER_H) maxDif = i1 - RENDER_H;

	if (i0 < 0)
	{
		minDif = 0 - i0;
		minDif -= offset;

		d += a * minDif;
	}

	for (int i = i0 + minDif; i <= i1 - maxDif; i++)
	{
		values[index] = d;
		index++;
		d += a;
	}

	count = index;
}

void Render::intersectionWithPlane(Vec3f &startPositive, Vec3f &endNegative, Vec3f &planeNormal, float &planeD, Vec3f &intersectPoint)
{
	Vec3f line = endNegative - startPositive;
	float t = (planeD - Vec3f::dotProduct(planeNormal, startPositive)) / (Vec3f::dotProduct(planeNormal, line));
	intersectPoint = startPositive + (line * t);
}

void Render::intersectionWithPlane(Vec4f &startPositive, Vec4f &endNegative, Vec3f &planeNormal, float &planeD, Vec4f &intersectPoint)
{
	Vec3f line = endNegative - startPositive;
	float t = (planeD - Vec3f::dotProduct(planeNormal, startPositive)) / (Vec3f::dotProduct(planeNormal, line));

	float w = startPositive.w + t * (endNegative.w - startPositive.w);
	intersectPoint = startPositive + (line * t);
	intersectPoint.w = w;
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