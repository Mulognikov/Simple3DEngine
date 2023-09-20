#include "Platform/Platform.h"
#include "Core/Render.h"
#include "Core/ResourceManager.h"
#include "Core/Camera.h"
#include "Core/Time.h"

int main(void)
{
	Platform::initialize();

	Platform::screen->initialize();
	Platform::systemTime->initialize();
	Platform::input->initialize();

	Render::initialize();
	Camera::updateProjectionMatrix();
	Camera::transform.position = Vec3f(0.0, 0.0f, 0.0f);
	Camera::transform.rotation = Vec3f(0.0, 0, 0);
	Camera::calculateViewMatrix();

	Texture texture;
	ResourceManager::loadTexturePPM("res/WoodenCrateTexture.ppm", texture);

	Texture texture2;
	ResourceManager::loadTexturePPM("res/WoodCrateHighRes.ppm", texture2);

	Mesh tri;
	tri.vertices = new Vec3f[4];
	tri.vertices[1] = Vec3f(-1,1,0);
	tri.vertices[2] = Vec3f(1,1,0);
	tri.vertices[3] = Vec3f(1,-1,0);
	tri.textureVertices = new Vec3f[4];
	tri.textureVertices[1] = Vec3f(0,1,0);
	tri.textureVertices[2] = Vec3f(1,1,0);
	tri.textureVertices[3] = Vec3f(1,0,0);
	tri.triangles = new Triangle[1];
	tri.triangles[0].face[0] = 1;
	tri.triangles[0].face[1] = 2;
	tri.triangles[0].face[2] = 3;
	tri.triangles[0].uv[0] = 1;
	tri.triangles[0].uv[1] = 2;
	tri.triangles[0].uv[2] = 3;
	tri.verticesCount = 4;
	tri.textureVerticesCount = 4;
	tri.trianglesCount = 1;

	Transform triTransform;
	triTransform.position = Vec3f(0,-1.0,5);
	triTransform.rotation = Vec3f(0, -45, 0);

	Mesh cube;
	ResourceManager::loadMesh("res/cube2.obj", cube);

	Transform cubeTransform;
	cubeTransform.position = Vec3f(0.0f, 0.f, 3.5f);
	cubeTransform.rotation = Vec3f(20, 0, 0);
	cubeTransform.scale = Vec3f(1.0f, 1.0f, 1.0f);

	Transform cube2Transform;
	cube2Transform.position = Vec3f(-3.0f, 1.0f, 2.0f);

	Mesh torus;
	ResourceManager::loadMesh("res/torus.obj", torus);

	Transform torusTransform;
	torusTransform.position = Vec3f(0.4f, 0.85f, 2.5f);
	torusTransform.rotation = Vec3f(0, 75, 26);

	Mesh icosphere;
	ResourceManager::loadMesh("res/icosphere.obj", icosphere);

	Transform icosphereTransform;
	icosphereTransform.position = Vec3f(-0.6f, -0.3f, 3.0f);
	icosphereTransform.scale = Vec3f(1.0f, 1.0f, 1.0f);

	Mesh car;
	ResourceManager::loadMesh("res/car.obj", car);

	Transform carTransform;
	carTransform.position = Vec3f(-5.0f, -1.5f, 0.0f);
	carTransform.rotation = Vec3f(0, 135, 0);
	carTransform.scale = Vec3f(1.0f, 1.0f, 1.0f);

	Mesh head;
	ResourceManager::loadMesh("res/head.obj", head);

	Transform headTransform;
	headTransform.position = Vec3f(7.0f, 0.f, -3.0f);
	headTransform.rotation = Vec3f(0, -110, 0);
	headTransform.scale = Vec3f(1.0f, 1.0f, 1.0f);

	Mesh mountains;
	ResourceManager::loadMesh("res/mountains.obj", mountains);
	Transform mountainsTransform;

	int frameCount = 0;
	uint32_t fpsSum = 0;

	while (!Platform::application->shouldExit())
	{
		Time::startUpdateTimer();
		//Camera::transform.rotation.y += 72.f * Time::deltaTime;

		float cameraMoveSpeed = 5.f;
		float cameraRotationSpeed = 45.f;

		Mat4x4 rightRot = Mat4x4::rotationMatrix_X(Camera::transform.rotation.x * 3.14159f / 180);
		Mat4x4 upRot = Mat4x4::rotationMatrix_Y(Camera::transform.rotation.y * 3.14159f / 180);
		Mat4x4 forwardRot = Mat4x4::rotationMatrix_Z(Camera::transform.rotation.z * 3.14159f / 180);

		Mat4x4 allRot = rightRot * upRot * forwardRot;

		Vec4f f = Vec4f(0, 0, 1, 0);
		Vec3f forward = Mat4x4::multiplyVector(allRot, f);

		Vec4f r = Vec4f(1, 0, 0, 0);
		Vec3f right = Mat4x4::multiplyVector(allRot, r);

		if (Platform::input->keyPressed(342))
		{
			cameraRotationSpeed = 30.f;
			cameraMoveSpeed = 1.0f;
		}
		else
		{
			cameraRotationSpeed = 90.f;
			cameraMoveSpeed = 5.0f;
		}

		if (Platform::input->keyPressed(87))
			Camera::transform.position = Camera::transform.position + forward * cameraMoveSpeed * Time::deltaTime;

		if (Platform::input->keyPressed(83))
			Camera::transform.position = Camera::transform.position + forward * -cameraMoveSpeed * Time::deltaTime;

		if (Platform::input->keyPressed(65))
			Camera::transform.position = Camera::transform.position + right * -cameraMoveSpeed * Time::deltaTime;

		if (Platform::input->keyPressed(68))
			Camera::transform.position = Camera::transform.position + right * cameraMoveSpeed * Time::deltaTime;

		if (Platform::input->keyPressed(340))
			Camera::transform.position = Camera::transform.position + Vec3f(0, 1, 0) * cameraMoveSpeed * Time::deltaTime;

		if (Platform::input->keyPressed(341))
			Camera::transform.position = Camera::transform.position + Vec3f(0, 1, 0) * -cameraMoveSpeed * Time::deltaTime;


		if (Platform::input->keyPressed(265))
			Camera::transform.rotation = Camera::transform.rotation + Vec3f(1, 0, 0) * cameraRotationSpeed * Time::deltaTime;

		if (Platform::input->keyPressed(264))
			Camera::transform.rotation = Camera::transform.rotation + Vec3f(-1, 0, 0) * cameraRotationSpeed * Time::deltaTime;

		if (Platform::input->keyPressed(263))
			Camera::transform.rotation = Camera::transform.rotation + Vec3f(0, -1, 0) * cameraRotationSpeed * Time::deltaTime;

		if (Platform::input->keyPressed(262))
			Camera::transform.rotation = Camera::transform.rotation + Vec3f(0, 1, 0) * cameraRotationSpeed * Time::deltaTime;


		//torusTransform.rotation = torusTransform.rotation + Vec3f(20.0f, 20.0f, 20.0f) * Time::deltaTime;
		//icosphereTransform.rotation = icosphereTransform.rotation + Vec3f(0, 50.0f, 0) * Time::deltaTime;
		//cubeTransform.rotation = cubeTransform.rotation + Vec3f(10.0f, 90.0f, 20.0f) * Time::deltaTime;

		Camera::calculateViewMatrix();

		//Render::drawMesh2(tri, triTransform, texture);
		//Render::drawMesh(torus, torusTransform);
		//Render::drawMesh(icosphere, icosphereTransform);
		Render::drawMesh2(cube, cubeTransform, texture);
		Render::drawMesh2(cube, cube2Transform, texture2);
		//Render::drawMesh(car, carTransform);
		//Render::drawMesh(head, headTransform);
		//Render::drawMesh(mountains, mountainsTransform);

		Render::render();
		Time::stopUpdateTimer();

		frameCount++;
		int fps = static_cast<int>(round(1.0f / Time::deltaTime));
		fpsSum += fps;
	}

	std::cout << fpsSum / frameCount << std::endl;
	Platform::screen->finalize();
	return 0;
}

