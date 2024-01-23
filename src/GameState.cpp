#include "GameState.hpp"

void GameStateManager::update(float deltatime)
{
    int result = currentState->update(deltatime);
    if (result != 0)
    {
        currentState->onExit();
        if (currentState->getNextState() != nullptr)
        {
            currentState = currentState->getNextState();
            currentState->onEnter();
        }
        else
        {
            currentState = nullptr;
        }
    }
}

void GameStateManager::setState(GameState *state)
{
    if (currentState != nullptr)
    {
        currentState->onExit();
    }
    currentState = state;
    currentState->onEnter();
}

bool StartState::update(float deltaTime)
{
    return false;
}

void StartState::onEnter()
{
}

void StartState::onExit()
{
}

bool FoxState::update(float deltaTime)
{
    return false;
}

void FoxState::onEnter()
{
}

void FoxState::onExit()
{
}

bool PlayState::update(float deltaTime)
{
    return false;
}

void PlayState::onEnter()
{
}

void PlayState::onExit()
{
}

bool EndState::update(float deltaTime)
{
    return false;
}

void EndState::onEnter()
{
}

void EndState::onExit()
{
}