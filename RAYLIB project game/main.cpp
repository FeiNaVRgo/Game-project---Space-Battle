#include <src\raylib-cpp.hpp>
#include <string>
#include "globals.h"
#include "ECS.h"
#include "Components.h"
#include "utils.h"

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION            100
#endif

#define MAX_POSTPRO_SHADERS         12

int main() {
    //TODO - movment restriction when colliding with another object
    //TODO - more diffrent enemies - add coroutines for complex behaviour
    //TODO - UPGRADES - A LOT OF THEM - it will be like augments for ship and also little upgrades
    //TODO - gui + health bars for enemies and damage indicators
    //TODO - particle system

    // Initialization
    //--------------------------------------------------------------------------------------
    GLOBALS::gStack.init();

    GLOBALS::gCoordinator.Init();
    
    GLOBALS::gCoordinator.RegisterComponent<Transforms>();
    GLOBALS::gCoordinator.RegisterComponent<RigidBody>();
    GLOBALS::gCoordinator.RegisterComponent<PlayerSpecific>();
    GLOBALS::gCoordinator.RegisterComponent<Sprite>();
    GLOBALS::gCoordinator.RegisterComponent<Health>();
    GLOBALS::gCoordinator.RegisterComponent<Enemy>();
    GLOBALS::gCoordinator.RegisterComponent<Bullet>();
    GLOBALS::gCoordinator.RegisterComponent<MovmentAI>();
    GLOBALS::gCoordinator.RegisterComponent<EntitySpecific>();
    GLOBALS::gCoordinator.RegisterComponent<Damage>();
    GLOBALS::gCoordinator.RegisterComponent<TimerComponent>();

    GLOBALS::physicsSystem = GLOBALS::gCoordinator.RegisterSystem<PhysicsSystem>();

    ECS::Signature signature1;
    signature1.set(GLOBALS::gCoordinator.GetComponentType<RigidBody>());
    signature1.set(GLOBALS::gCoordinator.GetComponentType<Transforms>());
    GLOBALS::gCoordinator.SetSystemSignature<PhysicsSystem>(signature1);

    GLOBALS::renderSystem = GLOBALS::gCoordinator.RegisterSystem<RenderSystem>();

    ECS::Signature signature2;
    signature2.set(GLOBALS::gCoordinator.GetComponentType<Transforms>());
    signature2.set(GLOBALS::gCoordinator.GetComponentType<Sprite>());
    signature2.set(GLOBALS::gCoordinator.GetComponentType<RigidBody>());
    GLOBALS::gCoordinator.SetSystemSignature<RenderSystem>(signature2);

    GLOBALS::inputSystem = GLOBALS::gCoordinator.RegisterSystem<InputSystem>();

    ECS::Signature signature3;
    signature3.set(GLOBALS::gCoordinator.GetComponentType<RigidBody>());
    signature3.set(GLOBALS::gCoordinator.GetComponentType<PlayerSpecific>());
    GLOBALS::gCoordinator.SetSystemSignature<InputSystem>(signature3);

    GLOBALS::healthSystem = GLOBALS::gCoordinator.RegisterSystem<HealthSystem>();

    ECS::Signature signature4;
    signature4.set(GLOBALS::gCoordinator.GetComponentType<Health>());
    signature4.set(GLOBALS::gCoordinator.GetComponentType<Sprite>());
    signature4.set(GLOBALS::gCoordinator.GetComponentType<RigidBody>());
    signature4.set(GLOBALS::gCoordinator.GetComponentType<TimerComponent>());
    signature4.set(GLOBALS::gCoordinator.GetComponentType<EntitySpecific>());
    GLOBALS::gCoordinator.SetSystemSignature<HealthSystem>(signature4);

    GLOBALS::enemySpawningSystem = GLOBALS::gCoordinator.RegisterSystem<EnemySpawningSystem>();

    ECS::Signature signature5;
    signature5.set(GLOBALS::gCoordinator.GetComponentType<Transforms>());
    signature5.set(GLOBALS::gCoordinator.GetComponentType<PlayerSpecific>());
    GLOBALS::gCoordinator.SetSystemSignature<EnemySpawningSystem>(signature5);

    GLOBALS::enemyAIMovmentSystem = GLOBALS::gCoordinator.RegisterSystem<EnemyAIMovmentSystem>();

    ECS::Signature signature6;
    signature6.set(GLOBALS::gCoordinator.GetComponentType<Transforms>());
    signature6.set(GLOBALS::gCoordinator.GetComponentType<Sprite>());
    signature6.set(GLOBALS::gCoordinator.GetComponentType<RigidBody>());
    signature6.set(GLOBALS::gCoordinator.GetComponentType<MovmentAI>());
    signature6.set(GLOBALS::gCoordinator.GetComponentType<Enemy>());
    GLOBALS::gCoordinator.SetSystemSignature<EnemyAIMovmentSystem>(signature6);

    GLOBALS::bulletRemovalSystem = GLOBALS::gCoordinator.RegisterSystem<BulletManipulationSystem>();

    ECS::Signature signature7;
    signature7.set(GLOBALS::gCoordinator.GetComponentType<RigidBody>());
    signature7.set(GLOBALS::gCoordinator.GetComponentType<Health>());
    signature7.set(GLOBALS::gCoordinator.GetComponentType<Bullet>());
    signature7.set(GLOBALS::gCoordinator.GetComponentType< MovmentAI>());
    signature7.set(GLOBALS::gCoordinator.GetComponentType<Sprite>());
    GLOBALS::gCoordinator.SetSystemSignature<BulletManipulationSystem>(signature7);

    GLOBALS::collisionSystem = GLOBALS::gCoordinator.RegisterSystem<CollisionSystem>();

    ECS::Signature signature8;
    signature8.set(GLOBALS::gCoordinator.GetComponentType<RigidBody>());
    signature8.set(GLOBALS::gCoordinator.GetComponentType<Health>());
    signature8.set(GLOBALS::gCoordinator.GetComponentType<EntitySpecific>());
    signature8.set(GLOBALS::gCoordinator.GetComponentType<Damage>());
    GLOBALS::gCoordinator.SetSystemSignature<CollisionSystem>(signature8);

    GLOBALS::entityRemovalSystem = GLOBALS::gCoordinator.RegisterSystem<EntityRemovalSystem>();

    ECS::Signature signature9;
    signature9.set(GLOBALS::gCoordinator.GetComponentType<RigidBody>());
    signature9.set(GLOBALS::gCoordinator.GetComponentType<Sprite>());
    GLOBALS::gCoordinator.SetSystemSignature<EntityRemovalSystem>(signature9);

    ECS::Entity player = GLOBALS::gCoordinator.CreateEntity();
    GLOBALS::gCoordinator.AddComponent<Transforms>(player, Transforms{
        .position = Vector2(GLOBALS::gridRect.width / 2.0f, GLOBALS::gridRect.height / 2.0f),
        .rotation = Vector2(0.0f, 0.0f),
        .scale = Vector2(1.0f, 1.0f)
        });
    GLOBALS::gCoordinator.AddComponent<RigidBody>(player, RigidBody{
        .velocity = {0.0f, 0.0f},
        .acceleration = {0.0f, 0.0f},
        .hitbox = {{0.0f, 0.0f, 18.0f, 18.0f}, raylib::Color::Yellow()},
        .isColliding = false,
        .onWhatSideIsColliding = {false ,false ,false ,false }
        });
    GLOBALS::gCoordinator.AddComponent<Sprite>(player, Sprite{
        .sprite = raylib::Texture2DUnmanaged("resources/PlayerModel.png"),
        .angle = 0.0f,
        .tint = {255, 255, 255, 255},
        .origin = raylib::Vector2(raylib::Texture2D("resources/PlayerModel.png").width * 0.5f, raylib::Texture2D("resources/PlayerModel.png").height * 0.5f)
        });
    GLOBALS::gCoordinator.AddComponent<PlayerSpecific>(player, PlayerSpecific{
        .dash = 1.0f
        });
    GLOBALS::gCoordinator.AddComponent<Health>(player, Health{
        .maxHealth = 30.0f,
        .health = 30.0f,
        .isDamaged = false,
        .frameImmunityTime = 500,
        .healthToSubstract = 0.0f,
        .toBeDamaged = false
        });
    GLOBALS::gCoordinator.AddComponent<EntitySpecific>(player, EntitySpecific{
        .id = ENTITY_ID::PLAYER_ID
        });
    GLOBALS::gCoordinator.AddComponent<TimerComponent>(player, TimerComponent{});
    ComponentCommons::addComponent<Damage>(player, 5, 5);

    auto& t = GLOBALS::gCoordinator.GetComponent<RigidBody>(player);
    spatial_hash::gGird.insert(player, t.hitbox.hitboxRect);

    GLOBALS::window.SetExitKey(KEY_NULL);
    GLOBALS::camera.zoom = 1.0f;
    // Main game loop
    while (!GLOBALS::exitWindow) {    // Detect window close button or ESC key
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
            BeginMode2D(GLOBALS::camera);
            GLOBALS::gStack.execute();
            EndMode2D();
        }
        //raylib::DrawText(std::to_string(spatial_hash::gGird.getEntityCount()), 50, 50, 15, raylib::Color::Beige());
        EndDrawing();
        //----------------------------------------------------------------------------------
        
    }

    return 0;
}