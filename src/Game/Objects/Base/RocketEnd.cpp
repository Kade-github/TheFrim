#include "RocketEnd.h"
#include "../../MusicManager.h"
#include <Game.h>
#include "../../WorldManager.h"
#include "../../LightingManager.h"
RocketEnd::RocketEnd(glm::vec3 _pos) : GameObject(_pos)
{
	m = Model("Assets/Models/rocket_frim.obj");
	m.LoadUVMap("rocket_frim");
	m.scale = glm::vec3(0.6f, 0.8f, 0.6f);
	position += glm::vec3(0.5f, 0.0f, 0.5f);

	Camera* cam = Game::instance->GetCamera();

	smoothPos = cam->position;
	fakePos = position;

	fire = new Sprite3D("Assets/Textures/rocket_burn.png", position);
	fire->width = 1.25;
	fire->height = 2;

	fire->UpdateSprite();


}

void RocketEnd::Draw()
{
	float time = MusicManager::GetInstance()->GetTime();
	bool playing = MusicManager::GetInstance()->IsPlaying();

	Camera* cam = Game::instance->GetCamera();

	if (!playing && !creditsSong && MusicManager::GetInstance()->currentSong == "Assets/Music/cutscene/theend.mp3")
	{
		creditsSong = true;
		MusicManager::GetInstance()->PlayMusic("thesongthatlastedalifetime", 1.0f);
		LightingManager::GetInstance()->RemoveLight(lightPos);

		position += glm::vec3(0.0f, 10.0f * Game::instance->deltaTime, 0.0f);

		if (position.y > 400)
			position.y = 280;

		float randX = ((rand() % 4) - 5) / 100.0f;
		float randZ = ((rand() % 4) - 5) / 100.0f;

		fire->Draw();

		fakePos = position + glm::vec3(randX, 0.0f, randZ);

		glm::vec3 toPosition = position + glm::vec3(0, -2.0f, 4.0f);

		cam->position = toPosition;
		cam->LookAt(position);
		cam->SetDirection();
	}
	else if (!creditsSong && playing)
	{
		if (time < 4.72f)
		{
			glm::vec3 toPosition = position + glm::vec3(2.0f, 4.0f, 2.0f);

			float amount = time / 4.72f;

			cam->position = glm::mix(smoothPos, toPosition, amount);
			cam->LookAt(position);
			cam->SetDirectionSmooth(amount);
		}
		else if (time < 9.36f)
		{
			glm::vec3 toPosition = position + glm::vec3(-2.0f, 4.0f, -2.0f);

			// shake fakepos

			float randX = ((rand() % 5) - 5) / 100.0f;
			float randZ = ((rand() % 5) - 5) / 100.0f;

			fakePos = position + glm::vec3(randX, 0.0f, randZ);

			if (time > 6.4 && !playedLift)
			{
				playedLift = true;
				MusicManager::GetInstance()->PlaySFX("rocket_lift");
				LightingManager::GetInstance()->AddLight(position, 10.0f);
				lightPos = position;
			}

			cam->position = toPosition;
			cam->LookAt(position);
			cam->SetDirection();
		}
		else if (time < 14.12f)
		{
			position += glm::vec3(0.0f, 10.0f * Game::instance->deltaTime, 0.0f);

			float randX = ((rand() % 5) - 5) / 100.0f;
			float randZ = ((rand() % 5) - 5) / 100.0f;

			fakePos = position + glm::vec3(randX, 0.0f, randZ);

			glm::vec3 toPosition = position + glm::vec3(0, 4.0f, -2.0f);

			cam->position = toPosition;
			cam->LookAt(position);
			cam->SetDirection();
		}
		else if (time < 18.7f)
		{
			position += glm::vec3(0.0f, 10.0f * Game::instance->deltaTime, 0.0f);

			float randX = ((rand() % 10) - 5) / 100.0f;
			float randZ = ((rand() % 10) - 5) / 100.0f;

			fakePos = position + glm::vec3(randX, 0.0f, randZ);

			glm::vec3 toPosition = position + glm::vec3(2, 4.0f, -2.0f);

			cam->position = toPosition;
			cam->LookAt(position);
			cam->SetDirection();
		}
		else if (time < 25.0f)
		{
			position += glm::vec3(0.0f, 10.0f * Game::instance->deltaTime, 0.0f);

			float randX = ((rand() % 2) - 5) / 100.0f;
			float randZ = ((rand() % 2) - 5) / 100.0f;

			fakePos = position + glm::vec3(randX, 0.0f, randZ);

			glm::vec3 toPosition = position + glm::vec3(5.0f, -6.0f, 6.0f);

			cam->position = toPosition;
			cam->LookAt(position);
			cam->SetDirection();
		}
	}
	else
	{
		position += glm::vec3(0.0f, 10.0f * Game::instance->deltaTime, 0.0f);

		if (position.y > 400)
			position.y = 280;

		float randX = ((rand() % 4) - 5) / 100.0f;
		float randZ = ((rand() % 4) - 5) / 100.0f;

		fakePos = position + glm::vec3(randX, 0.0f, randZ);

		glm::vec3 toPosition = position + glm::vec3(5.0f, -6.0f, 6.0f);

		cam->position = toPosition;
		cam->LookAt(position);
		cam->SetDirection();
	}


	fire->position = position - glm::vec3(0.0f, 1.9f, 0.0f);

	if (playedLift)
		fire->Draw();

	int lightLevel = LightingManager::GetInstance()->GetLightLevel(position);

	Game::instance->shader->Bind();

	Game::instance->shader->SetUniform1f("lightLevel", lightLevel);

	Game::instance->shader->Unbind();


	m.position = fakePos;
	m.Draw();

	Game::instance->shader->Bind();

	Game::instance->shader->SetUniform1f("lightLevel", 10.0f);

	Game::instance->shader->Unbind();
}

void RocketEnd::Destroy()
{
	delete fire;
}
