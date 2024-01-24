#pragma once

#include <iostream>
#include <vector>
#include <string>

#include <GameGlobals.hpp>

using namespace glm;

class GameState
{
protected:
    std::string name;
    GameState *nextState = nullptr;
    GameState(std::string name) : name(name) {}
    GameState(std::string name, GameState *nextState) : name(name), nextState(nextState) {}
    virtual ~GameState() {}

public:
    // returns true if we should go to the next state or false if we should stay in this state
    virtual bool update(float deltaTime) = 0;
    virtual void onEnter() = 0;
    virtual void onExit() = 0;

    std::string getName() { return name; }
    GameState *getNextState() { return nextState; }

    void setNextState(GameState *nextState) { this->nextState = nextState; }
};

class StartState final : public GameState
{
public:
    StartState() : GameState("Start") {}
    StartState(GameState *nextState) : GameState("Start", nextState) {}
    ~StartState() {}

    bool update(float deltaTime) override;
    void onEnter() override;
    void onExit() override;
};

class FoxState final : public GameState
{
public:
    FoxState() : GameState("Fox") {}
    FoxState(GameState *nextState) : GameState("Fox", nextState) {}
    ~FoxState() {}

    // ScenePointLight l;
    BenchTimer timer;

    bool update(float deltaTime) override;
    void onEnter() override;
    void onExit() override;
};

class PlayState final : public GameState
{
public:
    PlayState() : GameState("Play") {}
    PlayState(GameState *nextState) : GameState("Play", nextState) {}
    ~PlayState() {}

    bool update(float deltaTime) override;
    void onEnter() override;
    void onExit() override;
};

class EndState final : public GameState
{
public:
    EndState() : GameState("End") {}
    EndState(GameState *nextState) : GameState("End", nextState) {}
    ~EndState() {}

    bool update(float deltaTime) override;
    void onEnter() override;
    void onExit() override;
};

class EndFoxState final : public GameState
{
public:
    EndFoxState() : GameState("End") {}
    EndFoxState(GameState *nextState) : GameState("End", nextState) {}
    ~EndFoxState() {}

    bool update(float deltaTime) override;
    void onEnter() override;
    void onExit() override;
};


class GameStateManager
{
private:
    GameState *currentState = nullptr;

public:
    GameStateManager() {}
    ~GameStateManager() {}

    void update(float deltaTime);
    void setState(GameState *state);
    GameState *getState() { return currentState; }
};