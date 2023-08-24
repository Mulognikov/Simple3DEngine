#pragma once
#include "Vec3f.h"
#include "Vec4f.h"

struct Mat4x4
{
	float m[4][4] = { 0 };

	Mat4x4 operator * (const Mat4x4 &m1) const
	{
		Mat4x4 matrix;
		for (int c = 0; c < 4; c++)
			for (int r = 0; r < 4; r++)
				matrix.m[r][c] = m[r][0] * m1.m[0][c] + m[r][1] * m1.m[1][c] + m[r][2] * m1.m[2][c] + m[r][3] * m1.m[3][c];
		return matrix;
	}

	static Mat4x4 identityMatrix()
	{
		Mat4x4 matrix;
		matrix.m[0][0] = 1.0f;
		matrix.m[1][1] = 1.0f;
		matrix.m[2][2] = 1.0f;
		matrix.m[3][3] = 1.0f;
		return matrix;
	}

	static Mat4x4 rotationMatrix_X(float radianAngle)
	{
		Mat4x4 matRotX;
		matRotX.m[0][0] = 1;
		matRotX.m[1][1] = cosf(radianAngle);
		matRotX.m[1][2] = -sinf(radianAngle);
		matRotX.m[2][1] = sinf(radianAngle);
		matRotX.m[2][2] = cosf(radianAngle);
		matRotX.m[3][3] = 1;
		return matRotX;
	}

	static Mat4x4 rotationMatrix_Y(float radianAngle)
	{
		Mat4x4 matRotY;
		matRotY.m[0][0] = cosf(radianAngle);
		matRotY.m[1][1] = 1;
		matRotY.m[0][2] = -sinf(radianAngle);
		matRotY.m[2][0] = sinf(radianAngle);
		matRotY.m[2][2] = cosf(radianAngle);
		matRotY.m[3][3] = 1;
		return  matRotY;
	}

	static Mat4x4 rotationMatrix_Z(float radianAngle)
	{
		Mat4x4 matRotZ;
		matRotZ.m[0][0] = cosf(radianAngle);
		matRotZ.m[1][1] = cosf(radianAngle);
		matRotZ.m[1][0] = -sinf(radianAngle);
		matRotZ.m[0][1] = sinf(radianAngle);
		matRotZ.m[2][2] = 1;
		matRotZ.m[3][3] = 1;
		return matRotZ;
	}

	static Mat4x4 translation(float x, float y, float z)
	{
		Mat4x4 matrix;
		matrix.m[0][0] = 1.0f;
		matrix.m[1][1] = 1.0f;
		matrix.m[2][2] = 1.0f;
		matrix.m[3][3] = 1.0f;
		matrix.m[3][0] = x;
		matrix.m[3][1] = y;
		matrix.m[3][2] = z;
		return matrix;
	}

	static Mat4x4 translation(Vec3f t)
	{
		Mat4x4 matrix;
		matrix.m[0][0] = 1.0f;
		matrix.m[1][1] = 1.0f;
		matrix.m[2][2] = 1.0f;
		matrix.m[3][3] = 1.0f;
		matrix.m[3][0] = t.x;
		matrix.m[3][1] = t.y;
		matrix.m[3][2] = t.z;
		return matrix;
	}

	static Mat4x4 scale(Vec3f t)
	{
		Mat4x4 matrix;
		matrix.m[0][0] = t.x;
		matrix.m[1][1] = t.y;
		matrix.m[2][2] = t.z;
		matrix.m[3][3] = 1.0f;
		return matrix;
	}

	static Mat4x4 projectionMatrix(float fov, float aspectRatio, float near, float far)
	{
		float fovRad = 1.0f / tanf(fov * 0.5f / 180.0f * 3.14159f);
		Mat4x4 projectionMatrix;
		projectionMatrix.m[0][0] = aspectRatio * fovRad;
		projectionMatrix.m[1][1] = fovRad;
		projectionMatrix.m[2][2] = far / (far - near);
		projectionMatrix.m[3][2] = (-far * near) / (far - near);
		projectionMatrix.m[2][3] = 1.0f;
		projectionMatrix.m[3][3] = 0.0f;
		return projectionMatrix;
	}

	static Mat4x4 multiplyMatrix(Mat4x4 &m1, Mat4x4 &m2)
	{
		Mat4x4 matrix;
		for (int c = 0; c < 4; c++)
			for (int r = 0; r < 4; r++)
				matrix.m[r][c] = m1.m[r][0] * m2.m[0][c] + m1.m[r][1] * m2.m[1][c] + m1.m[r][2] * m2.m[2][c] + m1.m[r][3] * m2.m[3][c];
		return matrix;
	}

//	static void multiplyMatrixVector(Mat4x4 &mat, Vec3f &inVec, Vec3f &outVec)
//	{
//		outVec.x = inVec.x * mat.m[0][0] + inVec.y * mat.m[1][0] + inVec.z * mat.m[2][0] + mat.m[3][0];
//		outVec.y = inVec.x * mat.m[0][1] + inVec.y * mat.m[1][1] + inVec.z * mat.m[2][1] + mat.m[3][1];
//		outVec.z = inVec.x * mat.m[0][2] + inVec.y * mat.m[1][2] + inVec.z * mat.m[2][2] + mat.m[3][2];
//		float w = inVec.x * mat.m[0][3] + inVec.y * mat.m[1][3] + inVec.z * mat.m[2][3] + mat.m[3][3];
//
//		if (w != 0.0f)
//		{
//			outVec.x /= w;
//			outVec.y /= w;
//			outVec.z /= w;
//		}
//	}

	static Vec4f multiplyVector(Mat4x4 &m, Vec4f &i)
	{
		Vec4f v;
		v.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + i.w * m.m[3][0];
		v.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + i.w * m.m[3][1];
		v.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + i.w * m.m[3][2];
		v.w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + i.w * m.m[3][3];
		return v;
	}
};
