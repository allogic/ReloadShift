#define REDSHIFT_IMPLEMENTATION
#include <Redshift.h>

#include <Actors/Enemy.h>
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
    roll += 0.3f * deltaTime;
    if (roll >= 360.0f) roll = 0.0f;
    World::DispatchFor<
      Transform,
      Camera>(mWorld, [=](Transform* transform, Camera* camera)
        {
          //transform->SetWorldPosition(R32V3{ 0.0f, 0.0f, -50.0f });
        });
    World::DispatchFor<
      Transform,
      Renderable,
      Brain>(mWorld, [=](Transform* transform, Renderable* renderable, Brain* brain)
        {
          void* ptr = renderable->mTextureAlbedo.Get() ? (void*)(U64)renderable->mTextureAlbedo.Get()->GetID() : nullptr;
          ImGui::Image(ptr, ImVec2{ 512, 512 });
          transform->SetWorldRotation(R32V3{ 0.0f, 90.0f, roll });
        });

    ImGui::Begin("Debug");
    if (ImGui::Button("Create Player"))
    {
      mPlayers.emplace_back(World::CreateActor<Player>(mWorld, "Player"));
    }
    if (ImGui::Button("Create Enemy"))
    {
      mEnemies.emplace_back(World::CreateActor<Enemy>(mWorld, "Enemy"));
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
  }
  virtual void Render() override
  {
    
  }

private:

  std::vector<Player*> mPlayers = {};
  std::vector<Enemy*> mEnemies = {};
};

DECLARE_MODULE_IMPL(Sandbox)