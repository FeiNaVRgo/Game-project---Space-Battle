#include <src\raylib-cpp.hpp>
#include <string>
#include "globals.h"
#include "ECS.h"
#include "Components.h"
#include "utils.hpp"
#include "UI.h"
#include "EventManager.h"

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION            100
#endif

int main() {
    //TODO - more diffrent enemies + bosses - add coroutines for complex behaviour - 6
    //TODO - UPGRADES - A LOT OF THEM - it will be like augments for ship and also little upgrades - 1
    //TODO - gui(partially done) + for enemies and damage indicators - 1.5
    //TODO - particle system- when I need this i will do this shit
    //TODO - coroutines with lua - 3
    //TODO - funny bullet behaviours - mayby try coroutines you lazy fuck - 4
    //TODO - TURRESTS FOR PLAYER TO TRY THESE BULLETS BEHAVIOURS - PART OF UPGRADES SYSTEM - 5
    //TODO - make script for generating code for upgrades -2
    //maybe skill tree or some shit and multiple characters
    //TODO - make application class so all globals are stored in there + make abstractions so main.cpp file is very small
    
    // Initialization
    //--------------------------------------------------------------------------------------
    #pragma region GLOBAL_COORDINATOR
    G::gCoordinator.Init();
    
    G::gCoordinator.RegisterComponent<Transforms>();
    G::gCoordinator.RegisterComponent<RigidBody>();
    G::gCoordinator.RegisterComponent<PlayerSpecific>();
    G::gCoordinator.RegisterComponent<Sprite>();
    G::gCoordinator.RegisterComponent<Health>();
    G::gCoordinator.RegisterComponent<Enemy>();
    G::gCoordinator.RegisterComponent<Bullet>();
    G::gCoordinator.RegisterComponent<MovmentAI>();
    G::gCoordinator.RegisterComponent<EntitySpecific>();
    G::gCoordinator.RegisterComponent<Damage>();
    G::gCoordinator.RegisterComponent<TimerComponent>();
    G::gCoordinator.RegisterComponent<JustBorn>();
    G::gCoordinator.RegisterComponent<events::IListener>();
    G::gCoordinator.RegisterComponent<Inventory>();
    G::gCoordinator.RegisterComponent<WeaponMini>();
    G::gCoordinator.RegisterComponent<WeaponNormal>();
    G::gCoordinator.RegisterComponent<WeaponType>();

    G::physicsSystem = G::gCoordinator.RegisterSystem<PhysicsSystem>();

    ECS::Signature signature1;
    signature1.set(G::gCoordinator.GetComponentType<RigidBody>());
    signature1.set(G::gCoordinator.GetComponentType<Transforms>());
    G::gCoordinator.SetSystemSignature<PhysicsSystem>(signature1);

    G::renderSystem = G::gCoordinator.RegisterSystem<RenderSystem>();

    ECS::Signature signature2;
    signature2.set(G::gCoordinator.GetComponentType<Transforms>());
    signature2.set(G::gCoordinator.GetComponentType<Sprite>());
    G::gCoordinator.SetSystemSignature<RenderSystem>(signature2);

    G::inputSystem = G::gCoordinator.RegisterSystem<InputSystem>();

    ECS::Signature signature3;
    signature3.set(G::gCoordinator.GetComponentType<RigidBody>());
    signature3.set(G::gCoordinator.GetComponentType<PlayerSpecific>());
    G::gCoordinator.SetSystemSignature<InputSystem>(signature3);

    G::healthSystem = G::gCoordinator.RegisterSystem<HealthSystem>();

    ECS::Signature signature4;
    signature4.set(G::gCoordinator.GetComponentType<Health>());
    signature4.set(G::gCoordinator.GetComponentType<Sprite>());
    signature4.set(G::gCoordinator.GetComponentType<RigidBody>());
    signature4.set(G::gCoordinator.GetComponentType<TimerComponent>());
    signature4.set(G::gCoordinator.GetComponentType<EntitySpecific>());
    G::gCoordinator.SetSystemSignature<HealthSystem>(signature4);

    G::enemySpawningSystem = G::gCoordinator.RegisterSystem<EnemySpawningSystem>();

    ECS::Signature signature5;
    signature5.set(G::gCoordinator.GetComponentType<Transforms>());
    signature5.set(G::gCoordinator.GetComponentType<PlayerSpecific>());
    G::gCoordinator.SetSystemSignature<EnemySpawningSystem>(signature5);

    G::enemyAIMovmentSystem = G::gCoordinator.RegisterSystem<EnemyAIMovmentSystem>();

    ECS::Signature signature6;
    signature6.set(G::gCoordinator.GetComponentType<Transforms>());
    signature6.set(G::gCoordinator.GetComponentType<Sprite>());
    signature6.set(G::gCoordinator.GetComponentType<RigidBody>());
    signature6.set(G::gCoordinator.GetComponentType<MovmentAI>());
    signature6.set(G::gCoordinator.GetComponentType<Enemy>());
    G::gCoordinator.SetSystemSignature<EnemyAIMovmentSystem>(signature6);

    G::bulletManipulationSystem = G::gCoordinator.RegisterSystem<BulletManipulationSystem>();

    ECS::Signature signature7;
    signature7.set(G::gCoordinator.GetComponentType<RigidBody>());
    signature7.set(G::gCoordinator.GetComponentType<Health>());
    signature7.set(G::gCoordinator.GetComponentType<Bullet>());
    signature7.set(G::gCoordinator.GetComponentType<MovmentAI>());
    signature7.set(G::gCoordinator.GetComponentType<Sprite>());
    signature7.set(G::gCoordinator.GetComponentType<JustBorn>());
    G::gCoordinator.SetSystemSignature<BulletManipulationSystem>(signature7);
    
    G::collisionSystem = G::gCoordinator.RegisterSystem<CollisionSystem>();

    ECS::Signature signature8;
    signature8.set(G::gCoordinator.GetComponentType<Transforms>());
    signature8.set(G::gCoordinator.GetComponentType<RigidBody>());
    signature8.set(G::gCoordinator.GetComponentType<Health>());
    signature8.set(G::gCoordinator.GetComponentType<EntitySpecific>());
    signature8.set(G::gCoordinator.GetComponentType<Damage>());
    G::gCoordinator.SetSystemSignature<CollisionSystem>(signature8);

    G::entityRemovalSystem = G::gCoordinator.RegisterSystem<EntityRemovalSystem>();

    ECS::Signature signature9;
    signature9.set(G::gCoordinator.GetComponentType<RigidBody>());
    signature9.set(G::gCoordinator.GetComponentType<Sprite>());
    G::gCoordinator.SetSystemSignature<EntityRemovalSystem>(signature9);

    G::weaponSystem = G::gCoordinator.RegisterSystem<WeaponSystem>();

    ECS::Signature signature10;
    signature10.set(G::gCoordinator.GetComponentType<WeaponType>());
    signature10.set(G::gCoordinator.GetComponentType<Transforms>());
    signature10.set(G::gCoordinator.GetComponentType<Sprite>());
    G::gCoordinator.SetSystemSignature<WeaponSystem>(signature10);

    #pragma endregion initialization of components and systems
    
    G::player = G::gCoordinator.CreateEntity();

    G::gCoordinator.AddComponent<Transforms>(G::player, Transforms{
        .position = Vector2{G::gridRect.width / 2.0f, G::gridRect.height / 2.0f},
        .rotation = Vector2{0.0f, 0.0f},
        .scale = Vector2{1.0f, 1.0f}
        });
    G::gCoordinator.AddComponent<RigidBody>(G::player, RigidBody{
        .velocity = {0.0f, 0.0f},
        .acceleration = {0.0f, 0.0f},
        .hitbox = {{0.0f, 0.0f, 18.0f, 18.0f}, raylib::Color::Yellow()},
        .isColliding = false,
        .onWhatSideIsColliding = {false ,false ,false ,false }
        });
    G::gCoordinator.AddComponent<Sprite>(G::player, Sprite{
        .sprite = G::playerTexture,
        .angle = 0.0f,
        .tint = {255, 255, 255, 255},
        .origin = raylib::Vector2(G::playerTexture.width * 0.5f, G::playerTexture.height * 0.5f)
        });
    G::gCoordinator.AddComponent<PlayerSpecific>(G::player, PlayerSpecific{
        .dash = 1.0f
        });
    G::gCoordinator.AddComponent<Health>(G::player, Health{
        .maxHealth = 30.0f,
        .health = 30.0f,
        .isDamaged = false,
        .frameImmunityTime = 500,
        .healthToSubstract = 0.0f,
        .toBeDamaged = false
        });
    G::gCoordinator.AddComponent<EntitySpecific>(G::player, EntitySpecific{
        .id = ENTITY_ID::PLAYER_ID
        });
    G::gCoordinator.AddComponent<TimerComponent>(G::player, TimerComponent{});
    ComponentCommons::addComponent<Damage>(G::player, 5, 5);
    G::gCoordinator.AddComponent<Inventory>(G::player, Inventory{});
    
    auto const& t = G::gCoordinator.GetComponent<RigidBody>(G::player);
    spatial_hash::gGird.insert(G::player, t.hitbox.hitboxRect);

    G::window.SetExitKey(KEY_NULL);
    G::camera.zoom = 1.0f;
    // Main game loop
    while (!G::exitWindow) {    // Detect window close button or ESC key
        // Update
        //----------------------------------------------------------------------------------
        // 
        // Update your variables here
        //---------------------------------------------------------------------------------- 
        // 
        raylib::PanAndZoom();
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        {
            BeginMode2D(G::camera);
            G::gStack.execute();
            EndMode2D();
        }
        //raylib::DrawText(std::to_string(spatial_hash::gGird.getEntityCount()), 50, 50, 15, raylib::Color::Beige());
        EndDrawing();
        //----------------------------------------------------------------------------------
        
    }

    G::playerTexture.Unload();
    G::enemyTexture.Unload();
    G::playerBulletTexture1.Unload();

    return 0;
}