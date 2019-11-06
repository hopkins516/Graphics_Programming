internal void
initGame(Game* pGame, Renderer* pRenderer, Time* pTime, CollisionManager* pCM = 0) {
    initMaterial(&pGame->musicVisualizerMaterial, "MusicVisualizer",
                 "..//resources//shaders//vMusicVisualizer.glsl", 
                 "..//resources//shaders//fMusicVisualizer.glsl");
    
    //readWAV(&pGame->sound,"../resources/test.wav");
    //readWAV(&pGame->sound,"../resources/moonglow.wav");
    readWAV(&pGame->sound,"../resources/Hold The Line.wav");
    //readWAV(&pGame->sound,"../resources/A Horse With No Name.wav");
    MusicData musicData = {};
    musicData.pFFTMod = (f32*)calloc(HALF_SAMPLE_RATE, sizeof(f32));
    musicData.cfg = kiss_fftr_alloc(SAMPLE_RATE, 0, NULL, NULL);
    musicData.pPlaying = (u8*)pGame->sound.pData;
    musicData.pTBuffer = (kiss_fft_scalar*)malloc(sizeof(kiss_fft_scalar)*SAMPLE_RATE);
    musicData.pFBuffer = (kiss_fft_cpx*)malloc(HALF_SAMPLE_RATE*sizeof(kiss_fft_cpx));
    musicData.audioBlockAlign = pGame->sound.blockAlign;
    musicData.musicBytesLeft = pGame->sound.dataSize;
    musicData.paused = true;
    pGame->musicData = musicData;
    
    pGame->musicVisualizerConfig = {};
    initMusicVisualizer(&pGame->musicVisualizerConfig,
                        &pGame->musicData,
                        &pGame->musicVisualizerMaterial);
    
    s32 err = Pa_Initialize();
    if (err != paNoError) {
        printf("PortAudio error: %s\n", Pa_GetErrorText(err));
    }
    
    PaStream *stream;
    /* Open an audio I/O stream. */
    err = Pa_OpenDefaultStream(&stream,
                               0,/* no input channels */
                               pGame->sound.channels,// stereo output 
                               paInt16,  /* 32 bit floating point output */
                               pGame->sound.sampleRate,
                               SAMPLE_RATE, /* frames per buffer, i.e. the number
                               of sample frames that PortAudio will
                               request from the callback. Many apps
                               may want to use
                               paFramesPerBufferUnspecified, which
                               tells PortAudio to pick the best,
                               possibly changing, buffer size.*/
                               fftCallback, /* this is your callback function */
                               &pGame->musicData); /*This is a pointer that will be passed to your callback*/
    
    err = Pa_StartStream(stream);
}

global b32 gPaused;
global b32 gPrevPaused;
global u32 gPauseCounter;
global u32 gBlock = true;

internal void
updateGame(Game* pGame, Renderer* pRenderer, Time* pTime, CollisionManager* pCM = 0) {
    drawMusicVisualizer(&pGame->musicVisualizerConfig, &pGame->musicData, pRenderer);
    
    gPrevPaused = gPaused;
    if (isKeyPressed(pRenderer, KEY_SPACE)) {
        if (!gBlock) {
            ++gPauseCounter;
            if (gPauseCounter == 4){
                gPaused = true;
                gBlock = true;
            }
            if (gPaused && !gPrevPaused) {
                pGame->musicData.paused = !pGame->musicData.paused;
            }
        }
    } else {
        gPauseCounter = 0;
        gPaused = false;
        gBlock = false;
    }
    if (isKeyPressed(pRenderer, KEY_D)) {
        moveCamera(&pGame->camera, pRenderer->pCamera->right, pTime->deltaTime);
    }
    if (isKeyPressed(pRenderer, KEY_A)) {
        moveCamera(&pGame->camera, -1.f * pRenderer->pCamera->right, pTime->deltaTime);
    }
    if (isKeyPressed(pRenderer, KEY_W)) {
        moveCamera(&pGame->camera, pRenderer->pCamera->front, pTime->deltaTime);
    }
    if (isKeyPressed(pRenderer, KEY_S)) {
        moveCamera(&pGame->camera, -1.f * pRenderer->pCamera->front, pTime->deltaTime);
    }
    if (isKeyPressed(pRenderer, KEY_E)) {
        moveCamera(&pGame->camera, pRenderer->pCamera->up, pTime->deltaTime);
    }
    if (isKeyPressed(pRenderer, KEY_Q)) {
        moveCamera(&pGame->camera, -1.f * pRenderer->pCamera->up, pTime->deltaTime);
    }
    if (isKeyPressed(pRenderer, KEY_Z)) {
        pRenderer->pCamera->roll -= pTime->deltaTime * 5.f;
        pRenderer->pCamera->roll -= pTime->deltaTime * 5.f;
        updateCameraVectors(pRenderer->pCamera);
    }
    if (isKeyPressed(pRenderer, KEY_C)) {
        pRenderer->pCamera->roll += pTime->deltaTime * 5.f;
        updateCameraVectors(pRenderer->pCamera);
    }
    
    transformUpdate(&pGame->empty, pTime->deltaTime);
    transformDraw(&pGame->empty, pRenderer);
    
    f64 x, y;
    getMousePos(pRenderer->pWindow, &x, &y);
    cameraMouseMovement(pRenderer->pCamera, x, y, true);
}

