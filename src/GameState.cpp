#include "GameState.hpp"
#include <Player.hpp>
#include <FastUI.hpp>

ScenePointLight lightFoxAlive;
ScenePointLight lightFoxDead;

SingleStringBatchRef EpourInteragir;
SingleStringBatchRef textRenard;
SingleStringBatchRef textPeluche;
SingleStringBatchRef textLivre;
SingleStringBatchRef textFin;

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
        GameGlobals::foxAlive->state.position);

    EpourInteragir->state.hide = ModelStateHideStatus::HIDE;
    if (d < 12.0)
    {
        EpourInteragir->state.hide = ModelStateHideStatus::SHOW;
        if (GameGlobals::E)
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
    GameGlobals::carWithBlood->state.hide = ModelStateHideStatus::HIDE;
    GameGlobals::foxDead->state.hide = ModelStateHideStatus::HIDE;
    GameGlobals::foxTeddy->state.hide = ModelStateHideStatus::HIDE;
    GameGlobals::bookFox->state.hide = ModelStateHideStatus::HIDE;

    GameGlobals::Zone1Center = vec3(1E8);
    GameGlobals::Zone2Center = vec3(1E8);

    GameGlobals::zone2radius = 200 / 1.5;
    GameGlobals::zone1radius = 240 / 1.5;

    Player::reviveAnimationLength = 5.0;
    globals.currentCamera->setDirection(vec3(0, 0, 1));

    Player::respawnPoint = vec3(11.5, 1, -188);
    GameGlobals::player->respawn();
    Player::lifeLookDir = vec3(1, 0, 0);
    // GameGlobals::player->teleport(vec3(126, 1, 136));

    // GameGlobals::foxAlive->state.setPosition(vec3(110, 0, 126));
    // GameGlobals::scene->add(GameGlobals::foxAlive);

    lightFoxAlive = newPointLight();
    lightFoxAlive->setPosition(
                     GameGlobals::foxAlive->state.position + vec3(0, 2, 0))
        .setIntensity(15.0)
        .setRadius(6.0)
        .setColor(vec3(0.0, 0.1, 1.0));

    EpourInteragir = std::make_shared<SingleStringBatch>();
    EpourInteragir->setMaterial(GameGlobals::defaultFontMaterial);
    EpourInteragir->setFont(GameGlobals::FUIfont);
    EpourInteragir->text = U"Appuyer sur E pour intéragir.";
    EpourInteragir->batchText();

    EpourInteragir->state
        .scaleScalar(2.0)
        .setPosition(vec3(-0.85, -0.35, 0));

    EpourInteragir->state.hide = ModelStateHideStatus::HIDE;

    textRenard = std::make_shared<SingleStringBatch>();
    textRenard->setMaterial(GameGlobals::defaultFontMaterial);
    textRenard->setFont(GameGlobals::FUIfont);
    textRenard->text = U"Il est magnifique ! Je devrais aller chercher mon appareil photo dans à la maison!";
    textRenard->batchText();
    textRenard->state
        .scaleScalar(1.75)
        .setPosition(vec3(-0.85, -0.25, 0));

    textRenard->state.hide = ModelStateHideStatus::HIDE;

    textPeluche = std::make_shared<SingleStringBatch>();
    textPeluche->setMaterial(GameGlobals::defaultFontMaterial);
    textPeluche->setFont(GameGlobals::FUIfont);
    textPeluche->text = U"Quelqu'un a dû perdre ça...";
    textPeluche->batchText();

    textPeluche->state
        .scaleScalar(1.75)
        .setPosition(vec3(-0.85, -0.25, 0));

    textPeluche->state.hide = ModelStateHideStatus::HIDE;



    textLivre = std::make_shared<SingleStringBatch>();
    textLivre->setMaterial(GameGlobals::defaultFontMaterial);
    textLivre->setFont(GameGlobals::FUIfont);
    textLivre->text = U"Contes et Légendes du Renard, celui-là c'est toute mon enfance.";
    textLivre->batchText();

    textLivre->state
        .scaleScalar(1.75)
        .setPosition(vec3(-0.85, -0.25, 0))
        ;

    textLivre->state.hide = ModelStateHideStatus::HIDE;

    textFin = std::make_shared<SingleStringBatch>();
    textFin->setMaterial(GameGlobals::defaultFontMaterial);
    textFin->setFont(GameGlobals::FUIfont);
    textFin->text = U"Je ne sens plus aucunes présences dans ces bois...";
    textFin->batchText();

    textFin->state
        .scaleScalar(1.75)
        .setPosition(vec3(-0.85, -0.15, 0))
        ;

    textFin->state.hide = ModelStateHideStatus::HIDE;


    GameGlobals::scene2D->add(EpourInteragir);
    GameGlobals::scene2D->add(textRenard);
    GameGlobals::scene2D->add(textPeluche);
    GameGlobals::scene2D->add(textLivre);
    GameGlobals::scene2D->add(textFin);
}

void StartState::onExit()
{
    Player::reviveAnimationLength = 1.5;
}

bool FoxState::update(float deltaTime)
{
    static vec3 lp = globals.currentCamera->getPosition() + globals.currentCamera->getDirection();

    timer.end();
    timer.start();

    float t = timer.getElapsedTime();

    float i = 0.5 + 0.5 * cos(t * 1.5 - M_PI);

    lightFoxAlive->setRadius(3.0 + i * 5.f);

    // std::cout << i << "\n";

    globals.currentCamera->lookAt(
        mix(
            lp,
            GameGlobals::foxAlive->state.position,
            min(t, 1.f)));

    return t > 5.0;
}

void FoxState::onEnter()
{
    timer.start();

    globals.currentCamera->setForceLookAtPoint(true);
    // globals.currentCamera->lookAt(
    //     GameGlobals::foxAlive->state.position
    // );

    GameGlobals::scene->add(lightFoxAlive);

    Player::locked = true;
    textRenard->state.hide = ModelStateHideStatus::SHOW;
    EpourInteragir->state.hide = ModelStateHideStatus::HIDE;
}

void FoxState::onExit()
{
    globals.currentCamera->setForceLookAtPoint(false);
    Player::locked = false;
    GameGlobals::scene->remove(lightFoxAlive);
    textRenard->state.hide = ModelStateHideStatus::HIDE;
}

bool playerHasBook = false;
bool monsterIsGone = false;
BenchTimer pelucheTimer;
BenchTimer livreTimer;

bool PlayState::update(float deltaTime)
{
    if(
        GameGlobals::foxAlive->state.hide != ModelStateHideStatus::HIDE
    && !GameGlobals::foxAlive->cull())
    {
        GameGlobals::foxAlive->state.hide = ModelStateHideStatus::HIDE;
    }

    static bool shockEffect = false;

    if (!shockEffect)
    {
        GameGlobals::house->state.hide = ModelStateHideStatus::SHOW;
        if (GameGlobals::house->cull())
        {
            GameGlobals::gameBirds->pause();
            GameGlobals::gameMusic->pause();

            shockEffect = true;
        }
        GameGlobals::house->state.hide = ModelStateHideStatus::HIDE;
    }

    EpourInteragir->state.hide = ModelStateHideStatus::HIDE;

    if(monsterIsGone && playerHasBook)
    {
        return true;
    }

    if(!monsterIsGone && Player::hasTeddyBear)
    {
        float d = distance(
            GameGlobals::Zone2Objectif,
            GameGlobals::playerPosition);

        if (d < 3.0)
        {
            GameGlobals::Zone2Center = vec3(1E8);
            monsterIsGone = true;
            GameGlobals::monster->disable();
        }
    }

    if (!Player::hasTeddyBear)
    {
        float dTeddy = distance(
            GameGlobals::foxTeddy->state.position,
            GameGlobals::playerPosition);

        if (dTeddy < 6.0)
        {
            EpourInteragir->state.hide = ModelStateHideStatus::SHOW;
            if (GameGlobals::E)
            {
                pelucheTimer.start();
                Player::hasTeddyBear = true;
                GameGlobals::foxTeddy->state.hide = ModelStateHideStatus::HIDE;
                textPeluche->state.hide = ModelStateHideStatus::SHOW;
            }
        }
    }

    if (!playerHasBook)
    {
        float dBook = distance(
            GameGlobals::bookFox->state.position,
            GameGlobals::playerPosition);

        if (dBook < 6.0)
        {
            EpourInteragir->state.hide = ModelStateHideStatus::SHOW;
            if (GameGlobals::E)
            {
                livreTimer.start();
                textLivre->state.hide = ModelStateHideStatus::SHOW;
                playerHasBook = true;
                GameGlobals::Zone1Center = vec3(1E8);
                GameGlobals::Zone1Objectif = vec3(1E8);
                GameGlobals::bookFox->state.hide = ModelStateHideStatus::HIDE;
            }
        }
    }

    if (Player::hasTeddyBear)
    {
        pelucheTimer.end();
        pelucheTimer.start();

        if (pelucheTimer.getElapsedTime() > 5.0)
        {
            textPeluche->state.hide = ModelStateHideStatus::HIDE;
        }
    }

    if(playerHasBook)
    {
        livreTimer.end();
        livreTimer.start();

        if(livreTimer.getElapsedTime() > 5.0)
        {
            textLivre->state.hide = ModelStateHideStatus::HIDE;
        }
    }

    return false;
}

void PlayState::onEnter()
{
    GameGlobals::car->state.hide = ModelStateHideStatus::HIDE;

    // GameGlobals::Zone1Center = vec3(-208, 0, 40);
    // GameGlobals::Zone2Center = vec3(210, 0, 78);

    GameGlobals::Zone2Center = vec3(-208, 0, 40);
    GameGlobals::Zone1Center = vec3(210, 0, 78);

    GameGlobals::Zone1Objectif = vec3(185, 0, 53);
    GameGlobals::PeluchePosition = vec3(-165, 0, 96);

    GameGlobals::foxTeddy->state.hide = ModelStateHideStatus::SHOW;
    GameGlobals::bookFox->state.hide = ModelStateHideStatus::SHOW;
}

BenchTimer textFinTimer;

void PlayState::onExit()
{
    textFinTimer.start();
    textFin->state.hide = ModelStateHideStatus::SHOW;
}

bool EndState::update(float deltaTime)
{
    pelucheTimer.end();
    pelucheTimer.start();

    if(pelucheTimer.getElapsedTime() > 5.0)
    {
        textPeluche->state.hide = ModelStateHideStatus::HIDE;
    }

    livreTimer.end();
    livreTimer.start();

    if(livreTimer.getElapsedTime() > 5.0)
    {
        textLivre->state.hide = ModelStateHideStatus::HIDE;
    }

    textFinTimer.end();
    textFinTimer.start();

    if(textFinTimer.getElapsedTime() > 10.0)
    {
        textFin->state.hide = ModelStateHideStatus::HIDE;
    }
    
    float d = distance(
            GameGlobals::foxDead->state.position,
            GameGlobals::playerPosition
        );
    
    EpourInteragir->state.hide = ModelStateHideStatus::HIDE;

    if(d < 6.0)
    {
        EpourInteragir->state.hide = ModelStateHideStatus::SHOW;
        if(GameGlobals::E)
        {
            EpourInteragir->state.hide = ModelStateHideStatus::HIDE;
            return true;
        }
    }


    return false;
}

void EndState::onEnter()
{
    GameGlobals::foxDead->state.hide = ModelStateHideStatus::SHOW;
    GameGlobals::carWithBlood->state.hide = ModelStateHideStatus::SHOW;
    GameGlobals::house->state.hide = ModelStateHideStatus::SHOW;

    lightFoxDead = newPointLight();
    lightFoxDead->setPosition(
            GameGlobals::foxDead->state.position
            +vec3(0, 2, 0))
        .setIntensity(15.0)
        .setRadius(6.0)
        .setColor(vec3(1.0, 0.1, 0.0))
        ;

    GameGlobals::scene->add(lightFoxDead);
}

BenchTimer textFinFoxTimer;

void EndState::onExit()
{
    textFinFoxTimer.start();
}

bool EndFoxState::update(float deltaTime)
{
    textFinFoxTimer.end();
    textFinFoxTimer.start();

    static vec3 lp = globals.currentCamera->getPosition() + globals.currentCamera->getDirection();

    float t = textFinFoxTimer.getElapsedTime();

    float i = 0.5 + 0.5*cos(t*1.5 - M_PI);

    globals.currentCamera->lookAt(
        mix(
            lp,
            GameGlobals::foxDead->state.position,
            min(t, 1.f)
        )
    );

    lightFoxDead->setRadius(3.0 + i*5.f);

    // static bool tmp = false;
    // std::cout << t << "\n";
    if(t > 5.0)
    {
        // if(!tmp)
        // {
        //     Player::deathAnimationProgress = 0.f;
        //     tmp = true;
        // }

        Player::deathAnimationProgress += deltaTime/Player::respawnDelay;

        // GameGlobals::player->die();
        // std::cout << "DOIE \n";
        // tmp = true;
    }

    if(Player::deathAnimationProgress >= 0.999)
    {
        // return true;
        Player::deathAnimationProgress = 1.0;
    }

    return false;
}

void EndFoxState::onEnter()
{
    globals.currentCamera->setForceLookAtPoint(true);
    Player::locked = true;
}

void EndFoxState::onExit()
{
    Player::locked = false;
    Player::deathAnimationProgress = 1;
}
