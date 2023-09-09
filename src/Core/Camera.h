#pragma once
#include "../Math/Mat4x4.h"
#include "Render.h"

class Camera
{
private:
	Camera() = default;
	static inline Vec4f right4 = Vec4f(1, 0, 0, 0);
	static inline Vec4f up4 = Vec4f(0, 1, 0, 0);
	static inline Vec4f forward4 = Vec4f(0, 0, 1, 0);

public:
	static inline Transform transform;
	static inline float fov = 75.0f;
	static inline float near = 0.1f;
	static inline float far = 1000.0f;

	static inline Vec3f clipPlaneNormals[6];
	static inline float clipPlaneD[6] { 0 };
	static inline Mat4x4 projectionMatrix = Mat4x4::projectionMatrix(fov, ASPECT_RATIO, near, far);
	static inline Mat4x4 viewMatrix = Mat4x4::identityMatrix();

	static inline void updateProjectionMatrix()
	{
		projectionMatrix = Mat4x4::projectionMatrix(fov, ASPECT_RATIO, near, far);

		float cos = std::cosf(fov * 0.5f * 3.14159f / 180);
		float sin = std::sinf(fov * 0.5f * 3.14159f / 180);

		clipPlaneNormals[0] = Vec3f(0, 0, 1);
		clipPlaneNormals[1] = Vec3f(cos * ASPECT_RATIO, 0, sin);
		clipPlaneNormals[2] = Vec3f(-cos * ASPECT_RATIO, 0, sin);
		clipPlaneNormals[3] = Vec3f(0, cos, sin);
		clipPlaneNormals[4] = Vec3f(0, -cos, sin);
		clipPlaneNormals[5] = Vec3f(0, 0, -1);

		clipPlaneD[0] = near;
		clipPlaneD[5] = -far;
	}

	static inline void calculateViewMatrix()
	{
		Mat4x4 rightRot = Mat4x4::rotationMatrix_X(transform.rotation.x * 3.14159f / 180);
		Mat4x4 upRot = Mat4x4::rotationMatrix_Y(transform.rotation.y * 3.14159f / 180);
		Mat4x4 forwardRot = Mat4x4::rotationMatrix_Z(transform.rotation.z * 3.14159f / 180);

		Mat4x4 allRot = rightRot * upRot * forwardRot;

		Vec4f right = Mat4x4::multiplyVector(allRot, right4);
		Vec4f up = Mat4x4::multiplyVector(allRot, up4);
		Vec4f forward = Mat4x4::multiplyVector(allRot, forward4);

		Mat4x4 translate = Mat4x4::translation(transform.position * -1);
		Mat4x4 rotate;
		rotate.m[0][0] = right.x;
		rotate.m[1][0] = right.y;
		rotate.m[2][0] = right.z;
		rotate.m[0][1] = up.x;
		rotate.m[1][1] = up.y;
		rotate.m[2][1] = up.z;
		rotate.m[0][2] = forward.x;
		rotate.m[1][2] = forward.y;
		rotate.m[2][2] = forward.z;
		rotate.m[3][3] = 1;

		viewMatrix = translate * rotate;
	}
};
