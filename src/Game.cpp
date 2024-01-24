#include <Game.hpp>
#include <../Engine/include/Globals.hpp>
#include <GameObject.hpp>
#include <CompilingOptions.hpp>
#include <MathsUtils.hpp>
#include <Audio.hpp>
#include <TreeMapGenerator.hpp>
#include <GameState.hpp>
#include <FenceGenerator.hpp>

#include <thread>

Game::Game(GLFWwindow *window) : App(window) {}

void Game::init(int paramSample)
{
    setIcon("ressources/icon.png");

    ambientLight = vec3(0.1);

    finalProcessingStage = ShaderProgram(
        "shader/post-process/final composing.frag",
        "shader/post-process/basic.vert",
        "",
        globals.standartShaderUniform2D());

    finalProcessingStage.addUniform(ShaderUniform(Bloom.getIsEnableAddr(), 10));

    camera.init(radians(70.0f), globals.windowWidth(), globals.windowHeight(), 0.1f, 1E4f);
    camera.setMouseFollow(false);
    camera.setPosition(vec3(0, 1, 0));
    camera.setDirection(vec3(1, 0, 0));

    /* Loading 3D Materials */
    depthOnlyMaterial = MeshMaterial(
        new ShaderProgram(
            "shader/depthOnly.frag",
            "shader/foward/basic.vert",
            ""));

    depthOnlyStencilMaterial = MeshMaterial(
        new ShaderProgram(
            "shader/depthOnlyStencil.frag",
            "shader/foward/basic.vert",
            ""));

    depthOnlyInstancedMaterial = MeshMaterial(
        new ShaderProgram(
            "shader/depthOnlyStencil.frag",
            "shader/foward/basicInstance.vert",
            ""));

    std::vector<ShaderUniform> uniforms3D = globals.standartShaderUniform3D();
    uniforms3D.push_back(ShaderUniform(&effects.sunColor, 20));
    uniforms3D.push_back(ShaderUniform(&effects.z1lerp, 21));
    uniforms3D.push_back(ShaderUniform(&effects.o1lerp, 22));

    GameGlobals::PBR = MeshMaterial(
        new ShaderProgram(
            "shader/foward/PBR.frag",
            "shader/foward/basic.vert",
            "",
            uniforms3D));

    GameGlobals::PBRground = MeshMaterial(
        new ShaderProgram(
            "shader/foward/PBRground.frag",
            "shader/foward/basic.vert",
            "",
            uniforms3D));

    GameGlobals::PBRstencil = MeshMaterial(
        new ShaderProgram(
            "shader/foward/PBR.frag",
            "shader/foward/basic.vert",
            "",
            uniforms3D));

    GameGlobals::PBRinstanced = MeshMaterial(
        new ShaderProgram(
            "shader/foward/PBR.frag",
            "shader/foward/basicInstance.vert",
            "",
            globals.standartShaderUniform3D()));

    skyboxMaterial = MeshMaterial(
        new ShaderProgram(
            "shader/foward/Skybox.frag",
            "shader/foward/basic.vert",
            "",
            uniforms3D));

    GameGlobals::PBRstencil.depthOnly = depthOnlyStencilMaterial;
    GameGlobals::PBRinstanced.depthOnly = depthOnlyInstancedMaterial;
    scene.depthOnlyMaterial = depthOnlyMaterial;

    // GameGlobals::PBR->addUniform(ShaderUniform(&effects.sunColor, 20));
    // GameGlobals::PBR->addUniform(ShaderUniform(&effects.z1lerp, 21));

    // GameGlobals::PBRstencil->addUniform(ShaderUniform(&effects.sunColor, 20));
    // GameGlobals::PBRstencil->addUniform(ShaderUniform(&effects.z1lerp, 21));

    /* UI */
    FUIfont = FontRef(new FontUFT8);
    FUIfont->readCSV("ressources/fonts/Roboto/out.csv");
    FUIfont->setAtlas(Texture2D().loadFromFileKTX("ressources/fonts/Roboto/out.ktx"));
    defaultFontMaterial = MeshMaterial(
        new ShaderProgram(
            "shader/2D/sprite.frag",
            "shader/2D/sprite.vert",
            "",
            globals.standartShaderUniform2D()));

    defaultSUIMaterial = MeshMaterial(
        new ShaderProgram(
            "shader/2D/fastui.frag",
            "shader/2D/fastui.vert",
            "",
            globals.standartShaderUniform2D()));

    fuiBatch = SimpleUiTileBatchRef(new SimpleUiTileBatch);
    fuiBatch->setMaterial(defaultSUIMaterial);
    fuiBatch->state.position.z = 0.0;
    fuiBatch->state.forceUpdate();

    /* VSYNC and fps limit */
    globals.fpsLimiter.activate();
    globals.fpsLimiter.freq = 144.f;
    glfwSwapInterval(0);

    handItems = std::make_shared<HandItemHandler>();
    effects.setDefaultMist();
    effects.setDefaultPixel();
}

bool Game::userInput(GLFWKeyInfo input)
{
    if (baseInput(input))
        return true;

    player->doInputs(input);

    handItems->inputs(input);

    GameGlobals::E = false;

    if (input.action == GLFW_PRESS)
    {
        switch (input.key)
        {
        case GLFW_KEY_ESCAPE:
            state = quit;
            break;

        case GLFW_KEY_E:
            GameGlobals::E = true;
            break;

        case GLFW_KEY_F2:
            globals.currentCamera->toggleMouseFollow();
            break;

        case GLFW_KEY_1:
            Bloom.toggle();
            break;
        case GLFW_KEY_2:
            SSAO.toggle();
            break;

        case GLFW_KEY_F5:
#ifdef _WIN32
            system("cls");
#else
            system("clear");
#endif
            finalProcessingStage.reset();
            Bloom.getShader().reset();
            SSAO.getShader().reset();
            depthOnlyMaterial->reset();
            GameGlobals::PBR->reset();
            GameGlobals::PBRground->reset();
            GameGlobals::PBRstencil->reset();
            skyboxMaterial->reset();
            break;

        default:
            break;
        }
    }

    return true;
};

void Game::physicsLoop()
{
    physicsTicks.freq = 45.f;
    physicsTicks.activate();

    while (state != quit)
    {
        physicsTicks.start();

        physicsMutex.lock();
        physicsEngine.update(1.f / physicsTicks.freq);
        physicsMutex.unlock();

        physicsTicks.waitForEnd();
    }
}

void Game::mainloop()
{
    /* Loading Models and setting up the scene */
    ModelRef skybox = newModel(skyboxMaterial);
    skybox->loadFromFolder("ressources/models/skybox/", true, false);

    Texture2D skyboxNightTexture = Texture2D().loadFromFileKTX("ressources/models/skybox/8k_starsCE.ktx");

    Texture2D skyboxReflectTexture = Texture2D().
                                     // loadFromFile("ressources/models/skybox/reflect.png");
                                     loadFromFileKTX("ressources/models/skybox/reflect.ktx");

    // skybox->invertFaces = true;
    skybox->depthWrite = true;
    skybox->state.frustumCulled = false;
    skybox->state.scaleScalar(1E6);
    scene.add(skybox);

    ModelRef floor = newModel(GameGlobals::PBRground);
    floor->loadFromFolder("ressources/models/ground/");
    floor->setMap(Texture2D()
                      .loadFromFile("ressources/treeMap.png")
                      .setFormat(GL_RGBA)
                      .setInternalFormat(GL_RGBA)
                      .generate(),
                  7);
    floor->setMap(Texture2D().loadFromFileKTX("ressources/models/ground/CEpath.ktx"), 8);
    floor->setMap(Texture2D().loadFromFileKTX("ressources/models/ground/NRMpath.ktx"), 9);
    floor->setMap(Texture2D().loadFromFileKTX("ressources/models/ground/CEroad.ktx"), 10);
    floor->setMap(Texture2D().loadFromFileKTX("ressources/models/ground/NRMroad.ktx"), 11);
    // floor->setMap(Texture2D().loadFromFile("ressources/treeMap.png"), 8);

    int gridSize = 16;
    int gridScale = 10;
    for (int i = -gridSize; i < gridSize; i++)
        for (int j = -gridSize; j < gridSize; j++)
        {
            ModelRef f = floor->copyWithSharedMesh();
            f->state
                .scaleScalar(gridScale)
                .setPosition(vec3(i * gridScale * 1.80, 0, j * gridScale * 1.80));
            scene.add(f);
        }

    int forestSize = 8;
    float treeScale = 0.5;

    ModelRef leaves = newModel(GameGlobals::PBRstencil);
    leaves->loadFromFolder("ressources/models/fantasy tree/");
    leaves->noBackFaceCulling = true;

    ModelRef trunk = newModel(GameGlobals::PBR);
    trunk->loadFromFolder("ressources/models/fantasy tree/trunk/");

    // Models Oak
    Texture2D leafTexture = Texture2D().loadFromFileKTX("ressources/models/oak/textures/leaf/CE.ktx");
    Texture2D leafTextureMat = Texture2D().loadFromFileKTX("ressources/models/oak/textures/leaf/NRM.ktx");
    Texture2D trunkTexture = Texture2D().loadFromFileKTX("ressources/models/oak/textures/trunc/CE.ktx");
    Texture2D trunkTextureMat = Texture2D().loadFromFileKTX("ressources/models/oak/textures/trunc/NRM.ktx");

    ModelRef largeLeaf = newModel(GameGlobals::PBRstencil);
    ModelRef mediumLeaf = newModel(GameGlobals::PBRstencil);
    ModelRef smallLeaf = newModel(GameGlobals::PBRstencil);

    largeLeaf->loadFromFolder("ressources/models/oak/large/leaf/", false, false);
    mediumLeaf->loadFromFolder("ressources/models/oak/med/leaf/", false, false);
    smallLeaf->loadFromFolder("ressources/models/oak/small/leaf/", false, false);

    largeLeaf->setMap(leafTexture, 0).setMap(leafTextureMat, 1);
    mediumLeaf->setMap(leafTexture, 0).setMap(leafTextureMat, 1);
    smallLeaf->setMap(leafTexture, 0).setMap(leafTextureMat, 1);

    ModelRef largeTrunk = newModel(GameGlobals::PBR);
    ModelRef mediumTrunk = newModel(GameGlobals::PBR);
    ModelRef smallTrunk = newModel(GameGlobals::PBR);

    largeTrunk->loadFromFolder("ressources/models/oak/large/trunc/", false, false);
    mediumTrunk->loadFromFolder("ressources/models/oak/med/trunc/", false, false);
    smallTrunk->loadFromFolder("ressources/models/oak/small/trunc/", false, false);

    largeTrunk->setMap(trunkTexture, 0).setMap(trunkTextureMat, 1);
    mediumTrunk->setMap(trunkTexture, 0).setMap(trunkTextureMat, 1);
    smallTrunk->setMap(trunkTexture, 0).setMap(trunkTextureMat, 1);

    treeSizes trunkSizes;
    trunkSizes.t0 = smallTrunk;
    trunkSizes.t1 = mediumTrunk;
    trunkSizes.t2 = largeTrunk;

    treeSizes leavesSizes;
    leavesSizes.t0 = smallLeaf;
    leavesSizes.t1 = mediumLeaf;
    leavesSizes.t2 = largeLeaf;

    generateTreesFromHeatMap(scene, "ressources/treeMap.png", trunkSizes, leavesSizes, physicsEngine);

    /* old tree gen
    for (int i = -forestSize; i < forestSize; i++)
        for (int j = -forestSize; j < forestSize; j++)
        {
            ObjectGroupRef tree = newObjectGroup();
            tree->add(trunk->copyWithSharedMesh());
            ModelRef l = leaves->copyWithSharedMesh();
            l->noBackFaceCulling = true;
            tree->add(l);
            tree->state
                .scaleScalar(treeScale)
                .setPosition(vec3(i * treeScale * 40, 0, j * treeScale * 40));

            scene.add(tree);
        }

    */

    /* Instanced Mesh example */
    // InstancedModelRef trunk = newInstancedModel();
    // trunk->setMaterial(PBRinstanced);
    // trunk->loadFromFolder("ressources/models/fantasy tree/trunk/");
    // trunk->allocate(2E4);

    // for(int i = -forestSize; i < forestSize; i++)
    // for(int j = -forestSize; j < forestSize; j++)
    // {
    //     ModelInstance &inst = *trunk->createInstance();
    //     inst.scaleScalar(treeScale)
    //         .setPosition(vec3(i*treeScale*40, 0, j*treeScale*40));
    //     inst.update();
    // }
    // trunk->updateInstances();
    // scene.add(trunk);

    SceneDirectionalLight sun = newDirectionLight(
        DirectionLight()
            .setColor(vec3(143, 107, 71) / vec3(255))
            .setDirection(normalize(vec3(-1.0, -1.0, 0.0)))
            .setIntensity(5.0));

    sun->cameraResolution = vec2(2048);
    sun->shadowCameraSize = vec2(90, 90);
    sun->activateShadows();
    scene.add(sun);

    /* FPS demo initialization */
    RigidBody::gravity = vec3(0.0, -80, 0.0);

    AABBCollider aabbCollider = AABBCollider(vec3(-32 * 50, -.15, -32 * 50), vec3(32 * 50, .1, 32 * 50));

    RigidBodyRef FloorBody = newRigidBody(
        vec3(0.0, 0.0, 0.0),
        vec3(0.0, 0.0, 0.0),
        quat(0.0, 0.0, 0.0, 1.0),
        vec3(0.0, 0.0, 0.0),
        &aabbCollider,
        PhysicsMaterial(),
        0.0,
        false);

    physicsEngine.addObject(FloorBody);

    // GameObject FloorGameObject(newObjectGroup(), FloorBody);
    // FloorGameObject.getGroup()->add(floor);

    SphereCollider playerCollider = SphereCollider(2.0);
    RigidBodyRef playerBody = newRigidBody(
        vec3(0.0, 8.0, 0.0),
        vec3(0.0, 0.0, 0.0),
        quat(0.0, 0.0, 0.0, 1.0),
        vec3(0.0, 0.0, 0.0),
        &playerCollider,
        PhysicsMaterial(0.0f, 0.5f, 0.0f, 0.0f),
        1.0,
        true);

    physicsEngine.addObject(playerBody);

    player =
        std::make_shared<Player>(window, playerBody, &camera, &inputs);
    Player::thingsYouCanStandOn.push_back(FloorBody);

    GameGlobals::player = player.get();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glLineWidth(3.0);

    /* Setting up the UI */
    FastUI_context ui(fuiBatch, FUIfont, scene2D, defaultFontMaterial);
    FastUI_valueMenu menu(ui, {});

    menu->state.setPosition(vec3(-0.9, 0.5, 0)).scaleScalar(0.8);
    // globals.appTime.setMenuConst(menu);
    // globals.cpuTime.setMenu(menu);
    // globals.gpuTime.setMenu(menu);
    globals.fpsLimiter.setMenu(menu);
    // physicsTicks.setMenu(menu);

    sun->setMenu(menu, U"Sun");
    effects.setMenu(menu);
    GameGlobals::setMenu(menu);
    player->setMenu(menu);

    /* Music ! */
    // AudioFile music1;
    // music1.loadOGG("ressources/musics/Endless Space by GeorgeTantchev.ogg");

    // AudioSource musicSource;
    // musicSource
    //     .setBuffer(music1.getHandle())
    //     .setPosition(vec3(0, 0, 3))
    //     .play();

    // alSource3f(musicSource.getHandle(), AL_DIRECTION, 0.0, 0.0, 0.0);

    // Portail
    /*
        ModelRef portailOuvert = newModel(GameGlobals::PBR);
        portailOuvert->loadFromFolder("ressources/models/gate/gateOpen/");
        portailOuvert->state
            .scaleScalar(200)
            .setPosition(vec3(0, 0, 0));
        scene.add(portailOuvert);


        ModelRef portailFerme = newModel(GameGlobals::PBR);
        portailFerme->loadFromFolder("ressources/models/gate/gateClose/");
        portailFerme->state
            .scaleScalar(200)
            .setPosition(vec3(10, 0, 0));
        scene.add(portailFerme);
    */

    ModelRef maison = newModel(GameGlobals::PBR);
    maison->loadFromFolder("ressources/models/house/");
    maison->state
        .scaleScalar(1.5)
        .setPosition(vec3(135, 0, 135));
    scene.add(maison);

    ModelRef foxAlive = newModel(GameGlobals::PBR);
    foxAlive->loadFromFolder("ressources/models/fox/foxAlive/");
    foxAlive->state
        .scaleScalar(0.009)
        .setPosition(vec3(-10, 0, 0));
    // scene.add(foxAlive);

    ModelRef foxDead = newModel(GameGlobals::PBR);
    foxDead->loadFromFolder("ressources/models/fox/foxDead/");
    foxDead->state
        .scaleScalar(0.009)
        .setPosition(vec3(-20, 0, -20));
    // scene.add(foxDead);

    GameGlobals::scene = &scene;
    GameGlobals::foxAlive = foxAlive;
    GameGlobals::foxDead = foxDead;

    ModelRef fence = newModel(GameGlobals::PBRstencil);
    fence->loadFromFolder("ressources/models/fence/");
    fence->state
        .scaleScalar(0.8);
    generateFence(fence, scene, physicsEngine);

    // Peluche sur la souche
    ModelRef foxTeddy = newModel(GameGlobals::PBR);
    foxTeddy->loadFromFolder("ressources/models/foxTeddy/");
    foxTeddy->state
        .scaleScalar(7)
        .setPosition(vec3(-10, 0.69, 10));
    scene.add(foxTeddy);
    ModelRef stumpTeddy = newModel(GameGlobals::PBR);
    stumpTeddy->loadFromFolder("ressources/models/stump/");
    stumpTeddy->state
        .scaleScalar(0.009)
        .setPosition(vec3(-10, 0, 10));
    scene.add(stumpTeddy);

    // Livre sur la souche
    ModelRef bookFox = newModel(GameGlobals::PBR);
    bookFox->loadFromFolder("ressources/models/book/");
    bookFox->state
        .scaleScalar(0.025)
        .setPosition(vec3(-10, 0.69, 5));
    scene.add(bookFox);
    ModelRef stumpBook = newModel(GameGlobals::PBR);
    stumpBook->loadFromFolder("ressources/models/stump/");
    stumpBook->state
        .scaleScalar(0.009)
        .setPosition(vec3(-10, 0, 5));
    scene.add(stumpBook);

    ModelRef car = newModel(GameGlobals::PBR);
    car->loadFromFolder("ressources/models/car/");
    car->state
        .scaleScalar(0.012)
        .setPosition(vec3(104, 0, 111));
    scene.add(car);

    ModelRef carWithBlood = newModel(GameGlobals::PBR);
    carWithBlood->loadFromFolder("ressources/models/car/carWithBlood/");
    carWithBlood->state
        .scaleScalar(0.012)
        .setPosition(vec3(116, 0, 120));
    scene.add(carWithBlood);

    ModelRef signPost = newModel(GameGlobals::PBR);
    signPost->loadFromFolder("ressources/models/signpost/");
    signPost->state
        .scaleScalar(1)
        .setPosition(vec3(134, 0, 111));
    scene.add(signPost);

    /*
        ModelRef lanterne = newModel(GameGlobals::PBR);
        lanterne->loadFromFolder("ressources/models/lantern/");
        lanterne->state
            .scaleScalar(0.01)
            .setPosition(vec3(2, 2, 0));
        scene.add(lanterne);
        handItems->addItem(HandItemRef(new HandItem(HandItemType::lantern)));
        scene.add(handItems);
    */
    // lanterne->state.setPosition(GameGlobals::Zone2Objectif + vec3(0, 2, 0));

    ModelRef shadowMonster = newModel(GameGlobals::PBR);
    shadowMonster->loadFromFolder("ressources/models/shadow/");
    shadowMonster->state
        .scaleScalar(0.75)
        .setPosition(vec3(-30, 0, 0));
    scene.add(shadowMonster);

    GameGlobals::sun = sun;

    GameGlobals::Zone2Center = vec3(-80, 0, 5);
    GameGlobals::zone2radius = 0.0;
    GameGlobals::Zone2Objectif = vec3(80E8, 0, 5);

    GameGlobals::Zone1Center = vec3(100, 0, 0);
    GameGlobals::zone1radius = 0.0;
    GameGlobals::Zone1Objectif = vec3(100, 0, 0);

    GameGlobals::sun = sun;

    monster = Monster(shadowMonster);
    shadowMonster->state.hide = ModelStateHideStatus::HIDE;
    shadowMonster->state.setPosition(GameGlobals::Zone2Center);

    GameGlobals::monster = &monster;

    std::vector<GameState *> states;
    states.push_back(new StartState());
    states.push_back(new FoxState());
    states.push_back(new PlayState());
    states.push_back(new EndState());

    states[0]->setNextState(states[1]);
    states[1]->setNextState(states[2]);
    states[2]->setNextState(states[3]);

    GameState *currentState = states[0];

    GameStateManager stateManager;
    stateManager.setState(states[0]);

    monster.setMenu(menu);

    menu.batch();
    scene2D.updateAllObjects();
    fuiBatch->batch();

    state = AppState::run;
    std::thread physicsThreads(&Game::physicsLoop, this);

    AudioFile music;
    music.loadOGG("ressources/musics/calm_loop stereo.ogg");

    AudioSource musicSource;
    musicSource
        .setBuffer(music.getHandle())
        .loop(true)
        .play();

    /* Main Loop */
    while (state != AppState::quit)
    {
        mainloopStartRoutine();

        for (GLFWKeyInfo input; inputs.pull(input); userInput(input))
            ;

        cullTreeBodiesBasedOnDistance(GameGlobals::playerPosition);

        float delta = min(globals.simulationTime.getDelta(), 0.05f);
        if (globals.windowHasFocus() && delta > 0.00001f)
        {
            // physicsEngine.update(delta);
            player->update(delta);
            // FloorGameObject.update(delta);

            monster.update(delta);
        }

        stateManager.update(delta);

        // float c = 0.5 + 0.5*cos(globals.appTime.getElapsedTime());
        // musicSource.setPitch(0.1 + c*2);

        menu.trackCursor();
        menu.updateText();

        effects.update();

        mainloopPreRenderRoutine();

        /* UI & 2D Render */
        glEnable(GL_BLEND);
        glEnable(GL_FRAMEBUFFER_SRGB);

        scene2D.updateAllObjects();
        fuiBatch->batch();
        screenBuffer2D.activate();
        fuiBatch->draw();
        scene2D.cull();
        scene2D.draw();
        screenBuffer2D.deactivate();

        /* 3D Pre-Render */
        glDisable(GL_FRAMEBUFFER_SRGB);
        glDisable(GL_BLEND);
        glDepthFunc(GL_GREATER);
        glEnable(GL_DEPTH_TEST);

        scene.updateAllObjects();
        scene.generateShadowMaps();
        renderBuffer.activate();

        scene.cull();

        /* 3D Early Depth Testing */
        scene.depthOnlyDraw(*globals.currentCamera, true);
        glDepthFunc(GL_EQUAL);

        /* 3D Render */
        skybox->bindMap(0, 4);
        skyboxNightTexture.bind(5);
        skyboxReflectTexture.bind(6);
        scene.genLightBuffer();
        scene.draw();
        renderBuffer.deactivate();

        /* Post Processing */
        renderBuffer.bindTextures();
        SSAO.render(*globals.currentCamera);
        Bloom.render(*globals.currentCamera);

        /* Final Screen Composition */
        glViewport(0, 0, globals.windowWidth(), globals.windowHeight());
        finalProcessingStage.activate();

        mat4 cipm = inverse(camera.getProjectionMatrix());
        ShaderUniform(&cipm, 3).activate();

        mat4 cipv = inverse(camera.getViewMatrix());
        ShaderUniform(&cipv, 4).activate();

        effects.finalComposingUniforms.update();

        sun->shadowMap.bindTexture(0, 6);
        screenBuffer2D.bindTexture(0, 7);
        globals.drawFullscreenQuad();

        /* Main loop End */
        mainloopEndRoutine();
    }

    physicsThreads.join();
}
