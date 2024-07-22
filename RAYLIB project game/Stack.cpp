#include <src\raylib-cpp.hpp>
#include <src\rlgl.h>
#include <string>
#include "Components.h"
#include "Stack.h"
#include "globals.h"
#include "FunctionalBox.h"
#include "utils.h"

void Functions::PlayGame() {
    GLOBALS::window.ClearBackground(raylib::Color::DarkGray());
    rlPushMatrix();
        rlTranslatef(0, 25 * 50, 0);
        rlRotatef(90, 1, 0, 0);
    rlPopMatrix();

    if (GLOBALS::debugMode) {
        spatial_hash::gGird.drawGrid();
        spatial_hash::gGird.drawPopulatedTiles();
    }

    std::any_cast<std::shared_ptr<RenderSystem>>(GLOBALS::renderSystem)->updateSprites();
    std::any_cast<std::shared_ptr<PhysicsSystem>>(GLOBALS::physicsSystem)->update(GetFrameTime());
    std::any_cast<std::shared_ptr<InputSystem>>(GLOBALS::inputSystem)->update();
    std::any_cast<std::shared_ptr<EnemySpawningSystem>>(GLOBALS::enemySpawningSystem)->update();
    std::any_cast<std::shared_ptr<EnemyAIMovmentSystem>>(GLOBALS::enemyAIMovmentSystem)->update();
    std::any_cast<std::shared_ptr<CollisionSystem>>(GLOBALS::collisionSystem)->update();
    std::any_cast<std::shared_ptr<BulletRemovalSystem>>(GLOBALS::bulletRemovalSystem)->update();
    std::any_cast<std::shared_ptr<HealthSystem>>(GLOBALS::healthSystem)->update();
    std::any_cast<std::shared_ptr<EntityRemovalSystem>>(GLOBALS::entityRemovalSystem)->update();

    if (IsKeyPressed(KeyboardKey::KEY_BACKSPACE)) {
        GLOBALS::camera.target = { 0.0f, 0.0f };
        GLOBALS::camera.zoom = 1.0f;
        GLOBALS::camera.offset = { 0.0f, 0.0f };
        GLOBALS::gStack.pushToTop(ID_MAIN_MENU);
    }
}

void Functions::MainMenu() {
    GLOBALS::window.ClearBackground(raylib::Color::DarkGray());
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
        GLOBALS::gStack.pushToTop(FUNC_ID::ID_PLAY_GAME);
    }
    if (fSettings.update() && IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT)) {
        GLOBALS::gStack.pushToTop(FUNC_ID::ID_SETTINGS);
    }
    if (fExit.update() && IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT)) {
        GLOBALS::gStack.pushToTop(FUNC_ID::ID_EXIT);
    }
}

void Functions::Settings() {
    GLOBALS::window.ClearBackground(raylib::Color::DarkGray());
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
        GLOBALS::gStack.pushToTop(FUNC_ID::ID_SETTINGS_KEYBOARD);
    }


    if (fSD.update() && IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT)) {
        GLOBALS::gStack.pushToTop(FUNC_ID::ID_SETTINGS_SOUND);
    }
  

    if (IsKeyPressed(KeyboardKey::KEY_BACKSPACE)) {
        GLOBALS::gStack.pushToTop(FUNC_ID::ID_MAIN_MENU);
    }
}

void Functions::Exit() {
    GLOBALS::window.ClearBackground(raylib::Color::DarkGray());
    FunctionalBox f1(
        { 100.0f, 100.0f },
        RED,
        raylib::Text("Click me to exit", 20, YELLOW, GetFontDefault(), 1.0f));
    FunctionalBox f2(
        { 100.0f, 140.0f },
        RED,
        raylib::Text("Press backspace to comeback", 20, YELLOW, GetFontDefault(), 1.0f));

    if (f1.update() && IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT)) {
        GLOBALS::exitWindow = true;
    }
    f2.update();

    if (IsKeyPressed(KeyboardKey::KEY_BACKSPACE)) {
        GLOBALS::gStack.pushToTop(FUNC_ID::ID_MAIN_MENU);
    }
}

void Functions::SettingsKeyBoard() {
    GLOBALS::window.ClearBackground(raylib::Color::DarkGray());
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
        GLOBALS::gStack.pushToTop(FUNC_ID::ID_SETTINGS);
    }
}

void Functions::SettingsSound() {
    GLOBALS::window.ClearBackground(raylib::Color::DarkGray());

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
        GLOBALS::gStack.pushToTop(FUNC_ID::ID_SETTINGS);
    }
}