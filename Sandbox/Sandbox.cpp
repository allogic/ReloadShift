#define REDSHIFT_IMPLEMENTATION
#include <Redshift.h>

#include <Actors/Enemy.h>
#include <Actors/Player.h>

#include <Renderer/DeferredRenderer.h>

class Sandbox : public Module
{
public:

  Sandbox(World* world)
    : Module(world)
    , mDeferredRenderer{ world }
  {

  }

public:

  virtual void Tick(R32 deltaTime)
  {
    Module::Tick(deltaTime);

    // Rotating enemies hooray
    static R32 roll = 0.0f;
    roll += 0.03f * deltaTime;
    if (roll >= 360.0f) roll = 0.0f;
    mWorld->DispatchFor<
      Transform,
      Brain>([=](Transform* transform, Brain* brain)
        {
          transform->SetRotation(R32V3{ -90.0f, 0.0f, roll });
        });

    ImGui::Begin("Debug");
    if (ImGui::Button("Create Player"))
    {
      mPlayers.emplace_back(mWorld->CreateActor<Player>("Player"));
    }
    if (ImGui::Button("Create Enemy"))
    {
      mEnemies.emplace_back(mWorld->CreateActor<Enemy>("Enemy"));
    }
    if (ImGui::Button("Create 1000 Enemies"))
    {
      for (U32 i = 0; i < 10; ++i)
      {
        for (U32 j = 0; j < 10; ++j)
        {
          for (U32 k = 0; k < 10; ++k)
          {
            Enemy* enemy = mWorld->CreateActor<Enemy>("Enemy");
            enemy->GetTransform()->SetPosition((R32V3{ i, j, k } * 3.0f) - 15.0f);
            mEnemies.emplace_back(enemy);
          }
        }
      }
    }
    if (ImGui::Button("Destroy Players"))
    {
      //mWorld->DestroyActor<Player>(mPlayers);
      //mPlayers.clear();
    }
    if (ImGui::Button("Destroy Enemies"))
    {
      //mWorld->DestroyActor<Enemy>(mEnemies);
      //mEnemies.clear();
    }
    ImGui::End();

    mDeferredRenderer.Debug();
  }
  virtual void Render() override
  {
    mDeferredRenderer.Render();
  }

private:

  DeferredRenderer mDeferredRenderer;

private:

  std::vector<Player*> mPlayers = {};
  std::vector<Enemy*> mEnemies = {};
};

DECLARE_MODULE_IMPL(Sandbox)