#include "RocketEnd.h"
#include "../../MusicManager.h"
#include <Game.h>
#include "../../WorldManager.h"
#include "../../LightingManager.h"
#include "../../Scenes/Gameplay.h"
#include "../../Scenes/MainMenu.h"

RocketEnd::RocketEnd(glm::vec3 _pos) : GameObject(_pos)
{
	m = Model("Assets/Models/rocket_frim.obj");
	m.LoadUVMap("rocket_frim");
	m.scale = glm::vec3(0.6f, 0.8f, 0.6f);
	position += glm::vec3(0.5f, 0.0f, 0.5f);

	Gameplay* gp = (Gameplay*)Game::instance->currentScene;

	WorldManager::instance->SetPlayerPosition(gp->player->position);
	WorldManager::instance->SaveWorldNow();

	Camera* cam = Game::instance->GetCamera();

	smoothPos = cam->position;
	fakePos = position;

	fire = new Sprite3D("Assets/Textures/rocket_burn.png", position);
	fire->width = 1.25;
	fire->height = 2;

	fire->UpdateSprite();

	creditCam = gp->credits;

	cr.ReadCredits("Assets/credits.txt");

	for(Credit& c : cr.credits)
	{
		if (c.credit[0] == '[')
		{
			// image

			std::string path = c.credit.substr(1, c.credit.size() - 2);

			Sprite2D* s = new Sprite2D("Assets/" + path, glm::vec3(0, -600, 0));
			s->position.x = 350 - (s->width / 2);

			creditCam->AddObject(s);
			continue;
		}
		Text2D* t = new Text2D(c.credit, "ArialFrim", glm::vec3(0, -600, 0), glm::vec4(1, 1, 1, 1), 42);
		t->position.x = 350;
		t->center = true;
		creditCam->AddObject(t);
	}

	black = new Sprite2D("Assets/Textures/MainMenu/background.png", glm::vec3(0, -1920, 0));
	black->color = glm::vec4(0, 0, 0, 1);
	black->width = creditCam->_w;
	black->height = creditCam->_h;
	black->tag_id = "b";
	creditCam->AddObject(black);

	logo = new Sprite2D("Assets/Textures/logo.png", glm::vec3(0, -600, 0));
	logo->position.x = (creditCam->_w / 2) - (logo->width / 2);
	logo->tag_id = "b";
	creditCam->AddObject(logo);
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
	else if (!creditsSong && playing && MusicManager::GetInstance()->currentSong == "Assets/Music/cutscene/theend.mp3")
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
	else if (creditsSong)
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

		// scroll credits

		for(int i = 0; i < creditCam->objects.size(); i++)
		{
			GameObject2D* t = (GameObject2D*)creditCam->objects[i];

			if (t->tag_id == "b")
				continue;

			Credit& c = cr.credits[i];

			float diff = c.time - (time - 6);

			float y = (diff * (475 / 60)) * 12;

			t->position.y = -y;
		}

		if (time > 120.5f)
		{
			black->position.y = 0;
			logo->position.y = logo->height / 2;
		}
	}

	if (MusicManager::GetInstance()->currentSong == "Assets/Music/cutscene/thesongthatlastedalifetime.mp3" && time > 124)
	{
		if (!takeScreen)
		{
			takeScreen = true;
			Game::instance->TakeScreenshot(WorldManager::instance->_path + "/screenshot.png");
		}
		if (Game::instance->DidTakeScreenshot())
		{
			MainMenu* mm = new MainMenu();

			Game::instance->SwitchScene(mm);
		}
	}


	fire->position = position - glm::vec3(0.0f, 1.9f, 0.0f);

	if (playedLift)
	{
		fire->Draw();

		float dist = glm::distance(position, lightPos);

		if (lightPos != glm::vec3((int)position.x, (int)position.y, (int)position.z) && position.y < 180 && dist > 4.0f)
		{
			LightingManager::GetInstance()->RemoveLight(lightPos);
			lightPos = glm::vec3((int)position.x, (int)position.y, (int)position.z);
			LightingManager::GetInstance()->AddLight(lightPos, 10.0f);
		}
		else if (position.y > 180 && lightPos.y > -1)
		{
			lightPos.y = -1;
			LightingManager::GetInstance()->RemoveLight(lightPos);
		}
	}

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
