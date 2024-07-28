#include <src\raylib-cpp.hpp>
#include <src\rlgl.h>
#include <string>
#include "Components.h"
#include "Stack.h"
#include "globals.h"
#include "FunctionalBox.h"
#include "utils.h"

void StackFunctions::PlayGame() {
    G::window.ClearBackground(raylib::Color::DarkGray());
    rlPushMatrix();
        rlTranslatef(0, 25 * 50, 0);
        rlRotatef(90, 1, 0, 0);
    rlPopMatrix();

    //GLOBALS::gBloom.BeginMode();
    if (G::debugMode) {
        spatial_hash::gGird.drawGrid();
        spatial_hash::gGird.drawPopulatedTiles();
    }
   
    G::gUI.executeById(G::gUI.getKey());

    if (G::GameIsPlaying) {
        std::any_cast<std::shared_ptr<RenderSystem>>(G::renderSystem)->updateSprites();
        std::any_cast<std::shared_ptr<PhysicsSystem>>(G::physicsSystem)->update(GetFrameTime());
        std::any_cast<std::shared_ptr<InputSystem>>(G::inputSystem)->update();
        std::any_cast<std::shared_ptr<EnemySpawningSystem>>(G::enemySpawningSystem)->update();
        std::any_cast<std::shared_ptr<EnemyAIMovmentSystem>>(G::enemyAIMovmentSystem)->update();
        std::any_cast<std::shared_ptr<CollisionSystem>>(G::collisionSystem)->update();
        std::any_cast<std::shared_ptr<BulletManipulationSystem>>(G::bulletManipulationSystem)->update();
        std::any_cast<std::shared_ptr<HealthSystem>>(G::healthSystem)->update();
        std::any_cast<std::shared_ptr<EntityRemovalSystem>>(G::entityRemovalSystem)->update();
    }
    //GLOBALS::gBloom.EndMode();

    if (IsKeyPressed(KeyboardKey::KEY_BACKSPACE)) {
        G::camera.target = { 0.0f, 0.0f };
        G::camera.zoom = 1.0f;
        G::camera.offset = { 0.0f, 0.0f };
        G::gStack.pushToTop(ID_MAIN_MENU);
    }
}

void StackFunctions::MainMenu() {
    G::window.ClearBackground(raylib::Color::DarkGray());
    FunctionalBox fPlayGame(
        {100.0f, 100.0f},
        RED,
        raylib::Text("Play game", 20, YELLOW, GetFontDefault(), 1.0f)
        );
    FunctionalBox fSettings(
        { 100.0f, 140.0f },
        RED,
        raylib::Text("Settings", 20, GREEN, GetFontDefault(), 1.0f)
        );
    FunctionalBox fExit(
        { 100.0f, 180.0f },
        RED,
        raylib::Text("Exit", 20, BEIGE, GetFontDefault(), 1.0f)
        );

    if (fPlayGame.update() && IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT)) {
        G::gStack.pushToTop(FUNC_ID::ID_PLAY_GAME);
    }
    if (fSettings.update() && IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT)) {
        G::gStack.pushToTop(FUNC_ID::ID_SETTINGS);
    }
    if (fExit.update() && IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT)) {
        G::gStack.pushToTop(FUNC_ID::ID_EXIT);
    }
}

void StackFunctions::Settings() {
    G::window.ClearBackground(raylib::Color::DarkGray());
    FunctionalBox fKB(
        { 100.0f, 100.0f },
        RED,
        raylib::Text("KEYBOARD", 20, YELLOW, GetFontDefault(), 1.0f)
        );
    FunctionalBox fSD(
        { 100.0f, 140.0f },
        RED,
        raylib::Text("SOUND", 20, YELLOW, GetFontDefault(), 1.0f)
        );

    if (fKB.update() && IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT)) {
        G::gStack.pushToTop(FUNC_ID::ID_SETTINGS_KEYBOARD);
    }


    if (fSD.update() && IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT)) {
        G::gStack.pushToTop(FUNC_ID::ID_SETTINGS_SOUND);
    }
  

    if (IsKeyPressed(KeyboardKey::KEY_BACKSPACE)) {
        G::gStack.pushToTop(FUNC_ID::ID_MAIN_MENU);
    }
}

void StackFunctions::Exit() {
    G::window.ClearBackground(raylib::Color::DarkGray());
    FunctionalBox f1(
        { 100.0f, 100.0f },
        RED,
        raylib::Text("Click me to exit", 20, YELLOW, GetFontDefault(), 1.0f));
    FunctionalBox f2(
        { 100.0f, 140.0f },
        RED,
        raylib::Text("Press backspace to comeback", 20, YELLOW, GetFontDefault(), 1.0f));

    if (f1.update() && IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT)) {
        G::exitWindow = true;
    }
    f2.update();

    if (IsKeyPressed(KeyboardKey::KEY_BACKSPACE)) {
        G::gStack.pushToTop(FUNC_ID::ID_MAIN_MENU);
    }
}

void StackFunctions::SettingsKeyBoard() {
    G::window.ClearBackground(raylib::Color::DarkGray());
    FunctionalBox fShoot(
        {20.0f, 10.0f},
        RED,
        raylib::Text("Shoot: " + raylib::KeyInputToChar(KeyInputs::SHOOT), 20, BEIGE, GetFontDefault(), 1.0f)
    );
    FunctionalBox fFly(
        { 20.0f, 30.0f },
        RED,
        raylib::Text("Fly: " + raylib::KeyInputToChar(KeyInputs::FLY), 20, BEIGE, GetFontDefault(), 1.0f)
    );  
 
    if (fShoot.update()) {
        fShoot.changeTextOnInput(KeyInputs::SHOOT);
    }
    if (fFly.update()) {
        fFly.changeTextOnInput(KeyInputs::FLY);
    }

    if (IsKeyPressed(KeyboardKey::KEY_BACKSPACE)) {
        G::gStack.pushToTop(FUNC_ID::ID_SETTINGS);
    }
}

void StackFunctions::SettingsSound() {
    G::window.ClearBackground(raylib::Color::DarkGray());

    FunctionalBox fSG(
        { 20.0f, 10.0f },
        RED,
        raylib::Text("GENERAL SOUND: ", 20, YELLOW, GetFontDefault(), 1.0f)
    );
    FunctionalBox fSM(
        { 20.0f, 30.0f },
        RED,
        raylib::Text("MUSIC SOUND: ", 20, YELLOW, GetFontDefault(), 1.0f)
    );
    FunctionalBox fSP(
        { 20.0f, 50.0f },
        RED,
        raylib::Text("PLAYER SOUND: ", 20, YELLOW, GetFontDefault(), 1.0f)
    );

    fSG.update();
    fSM.update();
    fSP.update();


    if (IsKeyPressed(KeyboardKey::KEY_BACKSPACE)) {
        G::gStack.pushToTop(FUNC_ID::ID_SETTINGS);
    }
}