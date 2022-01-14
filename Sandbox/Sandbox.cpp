#define REDSHIFT_IMPLEMENTATION
#include <Redshift.h>

#include <Actors/Enemy.h>
#include <Actors/Gate.h>
#include <Actors/Player.h>

class Sandbox : public Module
{
public:

  Sandbox(World& world)
    : Module(world)
  {

  }

public:

  virtual void Tick(R32 deltaTime)
  {
    Module::Tick(deltaTime);

    static R32 roll = 0.0f;
    roll += 10.0f * deltaTime;
    if (roll >= 360.0f) roll = 0.0f;

    //for (auto& actor : mActors)
    //{
    //  World::DestroyActor(mWorld, actor);
    //}
    //mActors.clear();

    //mActors.emplace_back(World::CreateActor<Player>(mWorld, "Player"));
    //mActors.emplace_back(World::CreateActor<Enemy>(mWorld, "Enemy"));
    //mActors.emplace_back(World::CreateActor<Gate>(mWorld, "Gate"));

    World::DispatchFor<
      Transform,
      Camera>(mWorld, [=](Transform* transform, Camera* camera)
        {
          //transform->SetWorldRotation(R32V3{ 0.0f, 0.0f, roll });
        });

    World::DispatchFor<
      Transform,
      Renderable>(mWorld, [=](Transform* transform, Renderable* renderable)
        {
          transform->SetWorldRotation(R32V3{ 90.0f, 0.0f, 0.0f });
        });

    ImGui::Begin("Debug");
    if (ImGui::Button("Create Player"))
    {
      mActors.emplace_back(World::CreateActor<Player>(mWorld, "Player"));
    }
    if (ImGui::Button("Create Enemy"))
    {
      mActors.emplace_back(World::CreateActor<Enemy>(mWorld, "Enemy"));
    }
    if (ImGui::Button("Create Gate"))
    {
      mActors.emplace_back(World::CreateActor<Gate>(mWorld, "Gate"));
    }
    if (ImGui::Button("Create 1000 Enemies"))
    {
      for (U32 i = 0; i < 10; ++i)
      {
        for (U32 j = 0; j < 10; ++j)
        {
          for (U32 k = 0; k < 10; ++k)
          {
            Enemy* enemy = mWorld.CreateActor<Enemy>(mWorld, "Enemy");
            enemy->GetTransform()->SetWorldPosition((R32V3{ i, j, k } * 3.0f) - 13.5f);
            mActors.emplace_back(enemy);
          }
        }
      }
    }
    if (ImGui::Button("Destroy Actors"))
    {
      for (auto& actor : mActors)
      {
        World::DestroyActor(mWorld, actor);
      }
      mActors.clear();
    }
    ImGui::End();
  }

private:

  std::vector<Actor*> mActors = {};
};

DECLARE_MODULE_IMPL(Sandbox)