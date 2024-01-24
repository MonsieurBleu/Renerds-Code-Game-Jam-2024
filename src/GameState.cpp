#include "GameState.hpp"
#include <Player.hpp>

ScenePointLight lightFoxAlive;

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

    float d = distance(
        GameGlobals::playerPosition,
        GameGlobals::foxAlive->state.position
    );

    if(d < 3.0)
    {
        if(GameGlobals::E)
        {
            // GameGlobals::foxAlive->state
            //     .scaleScalar(
            //         5.0*GameGlobals::foxAlive->state.scale.x);

            // ScenePointLight l = newPointLight();
            // l->
            //      setPosition(
            //         GameGlobals::foxAlive->state.position
            //         +vec3(0, 2, 0))
            //     .setIntensity(10.0)
            //     .setRadius(10.0)
            //     .setColor(vec3(0.2, 0.5, 1.0))
            //     ;
            
            // GameGlobals::scene->add(l);

            return true;
        }
    }


    return false;
}

void StartState::onEnter()
{
    //globals.currentCamera->setPosition(vec3(126, 1, 136));
    Player::respawnPoint = vec3(0, 1, 0);
    GameGlobals::player->respawn();
    //GameGlobals::player->teleport(vec3(126, 1, 136));


    GameGlobals::foxAlive->state.setPosition(vec3(110, 0, 126));
    GameGlobals::scene->add(GameGlobals::foxAlive);


    lightFoxAlive = newPointLight();
    lightFoxAlive->setPosition(
            GameGlobals::foxAlive->state.position
            +vec3(0, 2, 0))
        .setIntensity(15.0)
        .setRadius(3.0)
        .setColor(vec3(0.0, 0.1, 1.0))
        ;
    
    GameGlobals::scene->add(lightFoxAlive);
}

void StartState::onExit()
{
}

bool FoxState::update(float deltaTime)
{
    static vec3 lp = globals.currentCamera->getPosition() + globals.currentCamera->getDirection();

    timer.end();
    timer.start();

    float t = timer.getElapsedTime();

    float i = 0.5 + 0.5*cos(t*1.5 - M_PI);

    lightFoxAlive->setRadius(3.0 + i*5.f);

    // std::cout << i << "\n";

    globals.currentCamera->lookAt(
        mix(
            lp,
            GameGlobals::foxAlive->state.position,
            min(t, 1.f)
        )
    );

    return false;
}

void FoxState::onEnter()
{
    timer.start();

    globals.currentCamera->setForceLookAtPoint(true);
    // globals.currentCamera->lookAt(
    //     GameGlobals::foxAlive->state.position
    // );
    Player::locked = true;
}

void FoxState::onExit()
{
    globals.currentCamera->setForceLookAtPoint(false);
    Player::locked = false;
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