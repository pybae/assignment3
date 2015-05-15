/*
-----------------------------------------------------------------------------
Filename:    TutorialApplication.cpp
-----------------------------------------------------------------------------

This source file is part of the
   ___                 __    __ _ _    _
  /___\__ _ _ __ ___  / / /\ \ (_) | _(_)
 //  // _` | '__/ _ \ \ \/  \/ / | |/ / |
/ \_// (_| | | |  __/  \  /\  /| |   <| |
\___/ \__, |_|  \___|   \/  \/ |_|_|\_\_|
      |___/
Tutorial Framework (for Ogre 1.9)
http://www.ogre3d.org/wiki/
-----------------------------------------------------------------------------
*/

#include "TutorialApplication.h"

#define PATHTOAUDIO ""

CEGUI::MouseButton convertButton(OIS::MouseButtonID buttonID);

typedef bool (*ContactProcessedCallback)(
  btManifoldPoint& cp,
  void* body0,void* body1);

//---------------------------------------------------------------------------
TutorialApplication::TutorialApplication(void) :
  ball(NULL), 
  room(NULL), 
  paddle(NULL), 
  mMode(MENU),
  mPlayers(SINGLE),
  mMusicState(MUSPLAY),
  mNetMgr(NULL)
{
}
//---------------------------------------------------------------------------
TutorialApplication::~TutorialApplication(void) {
  endNetwork();
  closeAudio();
}
//---------------------------------------------------------------------------
void TutorialApplication::createScene(void) {
  // initialize bullet components
  broadphase = new btDbvtBroadphase();
  collisionConfiguration = new btDefaultCollisionConfiguration();
  dispatcher = new btCollisionDispatcher(collisionConfiguration);
  solver = new btSequentialImpulseConstraintSolver;
  dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

  dynamicsWorld->setGravity(btVector3(-5.0f, 0, 0));

  ballDirection = Ogre::Vector3(Ogre::Math::RangeRandom(0, 10),
                                Ogre::Math::RangeRandom(0, 10),
                                Ogre::Math::RangeRandom(0, 10));
  Ogre::Real ballRadius = 100.0f;

  ball = new Ball(ballDirection, ballRadius);
  paddle = new Paddle(10.0f, 200.0f, 200.0f, "Paddle");
  paddle2 = new Paddle(10.0f, 200.0f, 200.0f, "Paddle2");
  room = new Room(paddle, paddle2, ball);

  room->createScene(mSceneMgr);
  ball->renderObject(mSceneMgr);
  paddle->renderObject(mSceneMgr);
  paddle2->renderObject(mSceneMgr);

  ball->initializeBullet(dynamicsWorld);
  room->initializeBullet(dynamicsWorld);
  paddle->initializeBullet(dynamicsWorld);
  paddle2->initializeBullet(dynamicsWorld);
  paddle->setPosition(-(room->getWidth()/2)+50, 0, -200);
  paddle2->setPosition(-(room->getWidth()/2)+50, 0, 200);

  // initalize CEGUI components
  {
    mRenderer = &CEGUI::OgreRenderer::bootstrapSystem();

    CEGUI::ImageManager::setImagesetDefaultResourceGroup("Imagesets");
    CEGUI::Font::setDefaultResourceGroup("Fonts");
    CEGUI::Scheme::setDefaultResourceGroup("Schemes");
    CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
    CEGUI::WindowManager::setDefaultResourceGroup("Layouts");

    CEGUI::SchemeManager::getSingleton().createFromFile("TaharezLook.scheme"); 

    CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("TaharezLook/MouseArrow");

    CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
    sheet = wmgr.createWindow("DefaultWindow", "Raquetball/Sheet");

    //the start menu
    menu = wmgr.createWindow("TaharezLook/FrameWindow", "Raquetball/Menu");
    menu->setSize(CEGUI::USize(CEGUI::UDim(0.4, 0), CEGUI::UDim(0.5, 0)));
    menu->setPosition(CEGUI::Vector2<CEGUI::UDim>(CEGUI::UDim(0.3,0), CEGUI::UDim(0.25,0)));

    //the one-player game button
    CEGUI::Window *onePlayer = wmgr.createWindow("TaharezLook/Button", "Raquetball/Menu/OnePlayer");
    onePlayer->setText("Single Player Game");
    onePlayer->setSize(CEGUI::USize(CEGUI::UDim(0.75, 0), CEGUI::UDim(0.16, 0)));
    onePlayer->setPosition(CEGUI::Vector2<CEGUI::UDim>(CEGUI::UDim(0.125,0), CEGUI::UDim(0.02,0)));
    menu->addChild(onePlayer);

    //the two-player game button
    CEGUI::Window *twoPlayer = wmgr.createWindow("TaharezLook/Button", "Raquetball/Menu/TwoPlayer");
    twoPlayer->setText("Multiplayer Player Game");
    twoPlayer->setSize(CEGUI::USize(CEGUI::UDim(0.75, 0), CEGUI::UDim(0.16, 0)));
    twoPlayer->setPosition(CEGUI::Vector2<CEGUI::UDim>(CEGUI::UDim(0.125,0), CEGUI::UDim(0.22,0)));
    menu->addChild(twoPlayer);

    //the settings button
    CEGUI::Window *settings = wmgr.createWindow("TaharezLook/Button", "Raquetball/Menu/Settings");
    settings->setText("Settings");
    settings->setSize(CEGUI::USize(CEGUI::UDim(0.75, 0), CEGUI::UDim(0.16, 0)));
    settings->setPosition(CEGUI::Vector2<CEGUI::UDim>(CEGUI::UDim(0.125,0), CEGUI::UDim(0.42,0)));
    menu->addChild(settings);

    //the how to play button
    CEGUI::Window *howToPlay = wmgr.createWindow("TaharezLook/Button", "Raquetball/Menu/HowToPlay");
    howToPlay->setText("How To Play");
    howToPlay->setSize(CEGUI::USize(CEGUI::UDim(0.75, 0), CEGUI::UDim(0.16, 0)));
    howToPlay->setPosition(CEGUI::Vector2<CEGUI::UDim>(CEGUI::UDim(0.125,0), CEGUI::UDim(0.62,0)));
    menu->addChild(howToPlay);

    //the quit button
    CEGUI::Window *quit = wmgr.createWindow("TaharezLook/Button", "Raquetball/Menu/Quit");
    quit->setText("Quit");
    quit->setSize(CEGUI::USize(CEGUI::UDim(0.75, 0), CEGUI::UDim(0.16, 0)));
    quit->setPosition(CEGUI::Vector2<CEGUI::UDim>(CEGUI::UDim(0.125,0), CEGUI::UDim(0.82,0)));
    menu->addChild(quit);

    //the settings menu
    settingsMenu = wmgr.createWindow("TaharezLook/FrameWindow", "Raquetball/Settings/SettingsMenu");
    settingsMenu->setSize(CEGUI::USize(CEGUI::UDim(0.4, 0), CEGUI::UDim(0.5, 0)));
    settingsMenu->setPosition(CEGUI::Vector2<CEGUI::UDim>(CEGUI::UDim(0.3,0), CEGUI::UDim(0.25,0)));

    //the sound button (in the settings menu)
    sound = wmgr.createWindow("TaharezLook/Button", "Raquetball/Menu/SettingsMenu/Sound");
    sound->setText("Disable Sound");
    sound->setSize(CEGUI::USize(CEGUI::UDim(0.75, 0), CEGUI::UDim(0.2, 0)));
    sound->setPosition(CEGUI::Vector2<CEGUI::UDim>(CEGUI::UDim(0.125,0), CEGUI::UDim(0.28,0)));
    settingsMenu->addChild(sound);

    //the back button (in the settings menu)
    CEGUI::Window *back = wmgr.createWindow("TaharezLook/Button", "Raquetball/Menu/SettingsMenu/Back");
    back->setText("Back");
    back->setSize(CEGUI::USize(CEGUI::UDim(0.75, 0), CEGUI::UDim(0.2, 0)));
    back->setPosition(CEGUI::Vector2<CEGUI::UDim>(CEGUI::UDim(0.125,0), CEGUI::UDim(0.52,0)));
    settingsMenu->addChild(back);

    //the pause button
    pause = wmgr.createWindow("TaharezLook/Button", "Raquetball/Pause");
    pause->setText("Pause");
    pause->setSize(CEGUI::USize(CEGUI::UDim(0.1, 0), CEGUI::UDim(0.05, 0)));

    //the pause menu
    pauseMenu = wmgr.createWindow("TaharezLook/FrameWindow", "Raquetball/PauseMenu");
    pauseMenu->setSize(CEGUI::USize(CEGUI::UDim(0.4, 0), CEGUI::UDim(0.5, 0)));
    pauseMenu->setPosition(CEGUI::Vector2<CEGUI::UDim>(CEGUI::UDim(0.3,0), CEGUI::UDim(0.25,0)));

    //the resume button
    CEGUI::Window *resume = wmgr.createWindow("TaharezLook/Button", "Raquetball/Pause/PauseMenu/Resume");
    resume->setText("Resume Game");
    resume->setSize(CEGUI::USize(CEGUI::UDim(0.75, 0), CEGUI::UDim(0.2, 0)));
    resume->setPosition(CEGUI::Vector2<CEGUI::UDim>(CEGUI::UDim(0.125,0), CEGUI::UDim(0.16,0)));
    pauseMenu->addChild(resume);

    //the main menu button (in the pause menu)
    CEGUI::Window *mainMenu = wmgr.createWindow("TaharezLook/Button", "Raquetball/Pause/PauseMenu/MainMenu");
    mainMenu->setText("Main Menu");
    mainMenu->setSize(CEGUI::USize(CEGUI::UDim(0.75, 0), CEGUI::UDim(0.2, 0)));
    mainMenu->setPosition(CEGUI::Vector2<CEGUI::UDim>(CEGUI::UDim(0.125,0), CEGUI::UDim(0.4,0)));
    pauseMenu->addChild(mainMenu);

    //the sound button (in the pause menu)
    soundPause = wmgr.createWindow("TaharezLook/Button", "Raquetball/Pause/PauseMenu/Sound");
    soundPause->setText("Disable Sound");
    soundPause->setSize(CEGUI::USize(CEGUI::UDim(0.75, 0), CEGUI::UDim(0.2, 0)));
    soundPause->setPosition(CEGUI::Vector2<CEGUI::UDim>(CEGUI::UDim(0.125,0), CEGUI::UDim(0.64,0)));
    pauseMenu->addChild(soundPause);

    //the score box
    scoreBox = wmgr.createWindow("TaharezLook/StaticText", "Raquetball/Pause/PauseMenu/Sound");
    scoreBox->setText("Score: 0");
    scoreBox->setSize(CEGUI::USize(CEGUI::UDim(0.1, 0), CEGUI::UDim(0.05, 0)));
    scoreBox->setPosition(CEGUI::Vector2<CEGUI::UDim>(CEGUI::UDim(0.9,0), CEGUI::UDim(0,0)));

    //the game over menu
    overMenu = wmgr.createWindow("TaharezLook/FrameWindow", "Raquetball/GameOverMenu");
    overMenu->setSize(CEGUI::USize(CEGUI::UDim(0.4, 0), CEGUI::UDim(0.5, 0)));
    overMenu->setPosition(CEGUI::Vector2<CEGUI::UDim>(CEGUI::UDim(0.3,0), CEGUI::UDim(0.25,0)));

    //the play again button
    CEGUI::Window *again = wmgr.createWindow("TaharezLook/Button", "Raquetball/GameOverMenu/PlayAgain");
    again->setText("Play Again");
    again->setSize(CEGUI::USize(CEGUI::UDim(0.75, 0), CEGUI::UDim(0.2, 0)));
    again->setPosition(CEGUI::Vector2<CEGUI::UDim>(CEGUI::UDim(0.125,0), CEGUI::UDim(0.28,0)));
    overMenu->addChild(again);

    //the main menu button (in the game over menu)
    CEGUI::Window *mainMenuOver = wmgr.createWindow("TaharezLook/Button", "Raquetball/GameOverMenu/MainMenu");
    mainMenuOver->setText("Main Menu");
    mainMenuOver->setSize(CEGUI::USize(CEGUI::UDim(0.75, 0), CEGUI::UDim(0.2, 0)));
    mainMenuOver->setPosition(CEGUI::Vector2<CEGUI::UDim>(CEGUI::UDim(0.125,0), CEGUI::UDim(0.52,0)));
    overMenu->addChild(mainMenuOver);

    //the multiplayer menu
    multiMenu = wmgr.createWindow("TaharezLook/FrameWindow", "Raquetball/Menu/MultiplayerMenu");
    multiMenu->setSize(CEGUI::USize(CEGUI::UDim(0.4, 0), CEGUI::UDim(0.5, 0)));
    multiMenu->setPosition(CEGUI::Vector2<CEGUI::UDim>(CEGUI::UDim(0.3,0), CEGUI::UDim(0.25,0)));

    //the back button (in the multiplayer menu)
    CEGUI::Window *backMulti = wmgr.createWindow("TaharezLook/Button", "Raquetball/Menu/MultiplayerMenu/Back");
    backMulti->setText("Back");
    backMulti->setSize(CEGUI::USize(CEGUI::UDim(0.75, 0), CEGUI::UDim(0.2, 0)));
    backMulti->setPosition(CEGUI::Vector2<CEGUI::UDim>(CEGUI::UDim(0.125,0), CEGUI::UDim(0.64,0)));
    multiMenu->addChild(backMulti);

    //the start game button
    CEGUI::Window *startGame = wmgr.createWindow("TaharezLook/Button", "Raquetball/Menu/MultiplayerMenu/Start");
    startGame->setText("Start Game");
    startGame->setSize(CEGUI::USize(CEGUI::UDim(0.75, 0), CEGUI::UDim(0.2, 0)));
    startGame->setPosition(CEGUI::Vector2<CEGUI::UDim>(CEGUI::UDim(0.125,0), CEGUI::UDim(0.16,0)));
    multiMenu->addChild(startGame);

    //the join game button
    CEGUI::Window *joinGame = wmgr.createWindow("TaharezLook/Button", "Raquetball/Menu/MultiplayerMenu/Join");
    joinGame->setText("Join Game");
    joinGame->setSize(CEGUI::USize(CEGUI::UDim(0.75, 0), CEGUI::UDim(0.2, 0)));
    joinGame->setPosition(CEGUI::Vector2<CEGUI::UDim>(CEGUI::UDim(0.125,0), CEGUI::UDim(0.4,0)));
    multiMenu->addChild(joinGame);

    //the join game menu
    joinMenu = wmgr.createWindow("TaharezLook/FrameWindow", "Raquetball/Menu/MultiplayerMenu/JoinMenu");
    joinMenu->setSize(CEGUI::USize(CEGUI::UDim(0.4, 0), CEGUI::UDim(0.5, 0)));
    joinMenu->setPosition(CEGUI::Vector2<CEGUI::UDim>(CEGUI::UDim(0.3,0), CEGUI::UDim(0.25,0)));

    //the edit box in the join game menu
    editBox = wmgr.createWindow("TaharezLook/Editbox", "Raquetball/Menu/MultiplayerMenu/JoinMenu/Editbox");
    editBox->setSize(CEGUI::USize(CEGUI::UDim(0.75, 0), CEGUI::UDim(0.2, 0)));
    editBox->setPosition(CEGUI::Vector2<CEGUI::UDim>(CEGUI::UDim(0.125,0), CEGUI::UDim(0.16,0)));
    joinMenu->addChild(editBox);

    //the search button (in the join game menu)
    CEGUI::Window *search = wmgr.createWindow("TaharezLook/Button", "Raquetball/Menu/MultiplayerMenu/JoinMenu/Search");
    search->setText("Search!");
    search->setSize(CEGUI::USize(CEGUI::UDim(0.75, 0), CEGUI::UDim(0.2, 0)));
    search->setPosition(CEGUI::Vector2<CEGUI::UDim>(CEGUI::UDim(0.125,0), CEGUI::UDim(0.4,0)));
    joinMenu->addChild(search);

    //the back button (in the join game menu)
    CEGUI::Window *backJoin = wmgr.createWindow("TaharezLook/Button", "Raquetball/Menu/MultiplayerMenu/JoinMenu/Back");
    backJoin->setText("Back");
    backJoin->setSize(CEGUI::USize(CEGUI::UDim(0.75, 0), CEGUI::UDim(0.2, 0)));
    backJoin->setPosition(CEGUI::Vector2<CEGUI::UDim>(CEGUI::UDim(0.125,0), CEGUI::UDim(0.64,0)));
    joinMenu->addChild(backJoin);

    //the how to play menu
    howToPlayMenu = wmgr.createWindow("TaharezLook/FrameWindow", "Raquetball/Menu/howToPlayMenu");
    howToPlayMenu->setSize(CEGUI::USize(CEGUI::UDim(0.4, 0), CEGUI::UDim(0.5, 0)));
    howToPlayMenu->setPosition(CEGUI::Vector2<CEGUI::UDim>(CEGUI::UDim(0.3,0), CEGUI::UDim(0.25,0)));

    //the back button (in the how to play menu)
    CEGUI::Window *backHTP = wmgr.createWindow("TaharezLook/Button", "Raquetball/Menu/howToPlayMenu/Back");
    backHTP->setText("Back");
    backHTP->setSize(CEGUI::USize(CEGUI::UDim(0.75, 0), CEGUI::UDim(0.16, 0)));
    backHTP->setPosition(CEGUI::Vector2<CEGUI::UDim>(CEGUI::UDim(0.125,0), CEGUI::UDim(0.82,0)));
    howToPlayMenu->addChild(backHTP);

    //how to play information
    CEGUI::Window *info = wmgr.createWindow("TaharezLook/StaticText", "Raquetball/Menu/howToPlayMenu/Info");
    info->setText("Single Player Instructions:\nEvery time you hit the ball, you get a point.\nWhen you miss the ball, the game ends.\nWhat's the highest score you can get?\n\nMulti Player Instructions:\nTake turns hitting the ball.\nIf your opponent misses, you win!\n\nControls:\nTo move your paddle, use the WASD keys.\nYou can pause/resume the game at any time\nby using the 'Q' key");
    info->setSize(CEGUI::USize(CEGUI::UDim(0.96, 0), CEGUI::UDim(0.78, 0)));
    info->setPosition(CEGUI::Vector2<CEGUI::UDim>(CEGUI::UDim(0.02,0), CEGUI::UDim(0.02,0)));
    howToPlayMenu->addChild(info);

    //the game title
    // CEGUI::Window *title = wmgr.createWindow("TaharezLook/StaticText","Raquetball/Title");
    // title->setText("RAQUETBALL");
    // title->getText().textFormatting("HorzCentred");
    // title->setSize(CEGUI::USize(CEGUI::UDim(0.4, 0), CEGUI::UDim(0.1, 0)));
    // title->setPosition(CEGUI::Vector2<CEGUI::UDim>(CEGUI::UDim(0.3,0), CEGUI::UDim(0.075,0)));

    // sheet->addChild(title);
    sheet->addChild(menu);

    CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(sheet);
    quit->        subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TutorialApplication::quit,              this));  
    onePlayer->   subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TutorialApplication::startSinglePlayer, this));
    settings->    subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TutorialApplication::openSettingsMenu,  this));
    back->        subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TutorialApplication::goBack,            this));
    sound->       subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TutorialApplication::changeSound,       this));
    pause->       subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TutorialApplication::pauseGame,         this));
    resume->      subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TutorialApplication::resumeGame,        this));
    mainMenu->    subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TutorialApplication::openMainMenu,      this));
    soundPause->  subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TutorialApplication::changeSound,       this));  
    again->       subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TutorialApplication::playAgain,         this)); 
    mainMenuOver->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TutorialApplication::reopenMainMenu,    this));
    twoPlayer->   subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TutorialApplication::startMultiPlayer,  this));
    backMulti->   subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TutorialApplication::goBackMulti,       this));  
    joinGame->    subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TutorialApplication::joinGame,          this)); 
    backJoin->    subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TutorialApplication::goBackJoin,        this)); 
    search->      subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TutorialApplication::gameSearch,        this)); 
    startGame->   subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TutorialApplication::startMultiGame,    this)); 
    howToPlay->   subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TutorialApplication::openHowToPlayMenu, this)); 
    backHTP->     subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TutorialApplication::goBackHTP,         this));   
  }
}
//---------------------------------------------------------------------------
void TutorialApplication::createCamera(void)
{
  // create the camera
  mCamera = mSceneMgr->createCamera("PlayerCam");
  // set its position, direction
  mCamera->setPosition(Ogre::Vector3(-2250,0,0));
  mCamera->lookAt(Ogre::Vector3(0,0,0));
  // set the near clip distance
  mCamera->setNearClipDistance(5);

  mCameraMan = new OgreBites::SdkCameraMan(mCamera);   // create a default camera controller
}
//---------------------------------------------------------------------------
void TutorialApplication::destroyScene(void)
{
  delete collisionConfiguration;
  delete dispatcher;
  delete solver;

  delete ball;
  delete paddle;
  delete room;
}
//---------------------------------------------------------------------------
void TutorialApplication::createFrameListener(void)
{  
  Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
  OIS::ParamList pl;
  size_t windowHnd = 0;
  std::ostringstream windowHndStr;

  mWindow->getCustomAttribute("WINDOW", &windowHnd);
  windowHndStr << windowHnd;
  pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

  mInputManager = OIS::InputManager::createInputSystem( pl );

  mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, true ));
  mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, true ));

  mMouse->setEventCallback(this);
  mKeyboard->setEventCallback(this);

  //Set initial mouse clipping size
  windowResized(mWindow);

  //Register as a Window listener
  Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);

  mRoot->addFrameListener(this);
}
//-------------------------------------------------------------------------------------
void TutorialApplication::endNetwork() {
  if ( mNetMgr ) {
    mNetMgr->close();
    delete mNetMgr;
    mNetMgr = NULL;
  }
}
//-------------------------------------------------------------------------------------
bool TutorialApplication::beginServer() {
  if ( mNetMgr ) {
    endNetwork();
  }
  mNetMgr = new NetManager();
  mNetMgr->initNetManager();
  mNetMgr->addNetworkInfo();
  if ( mNetMgr->startServer() ) {
    mNetMgr->acceptConnections();
    return true;
  }
  return false;
}
//-------------------------------------------------------------------------------------
void TutorialApplication::sendState() {
//server checks client player position
  if ( mNetMgr->pollForActivity(10) ) {
    ClientData& msg = *(mNetMgr->tcpClientData[0]);
    //if ( msg.updated ) {//update client paddle direction
      room->paddle2->paddleDirection[0] = msg.output[0];
      room->paddle2->paddleDirection[1] = msg.output[1]; 
    //}
  }
//server sends correct server position and turn
  char buf[] = { (char)(room->paddle->paddleDirection[0]),
                 (char)(room->paddle->paddleDirection[1])};
  mNetMgr->messageClients(PROTOCOL_TCP,buf,2);

}
//-------------------------------------------------------------------------------------
bool TutorialApplication::beginClient(const char* hostIP) {
  if ( mNetMgr ) {
    endNetwork();
  }
  mNetMgr = new NetManager();
  mNetMgr->initNetManager();
  mNetMgr->addNetworkInfo(PROTOCOL_ALL,hostIP);
  if ( mNetMgr->startClient() ) {
    return true;
  }
  return false;
}
//-------------------------------------------------------------------------------------
void TutorialApplication::checkState() {
//sends client player position
  char buf[] = { (char)(room->paddle2->paddleDirection[0]),
                 (char)(room->paddle2->paddleDirection[1])};
  mNetMgr->messageServer(PROTOCOL_TCP,buf,2);
//verifies server player position and turn
  if ( mNetMgr->pollForActivity(10) ) {
    ClientData& msg = mNetMgr->tcpServerData;
    //if ( msg.updated ) {//update server paddle direction
      room->paddle->paddleDirection[0] = msg.output[0];
      room->paddle->paddleDirection[1] = msg.output[1];
    //}
  }
}
//-------------------------------------------------------------------------------------
void TutorialApplication::gameStep(const Ogre::FrameEvent& evt) {

  int soundStatus = room->collision(evt);

  if(room->gameOver)
  {
    mMode = OVER;

    // zero paddle velocity on game over
    room->paddle->paddleDirection[0] = 0;
    room->paddle->paddleDirection[1] = 0;
    room->paddle2->paddleDirection[0] = 0;
    room->paddle2->paddleDirection[1] = 0;

    sheet->removeChild(pause);
    sheet->addChild(overMenu);
  }

  scoreBox->setText("Score: " + std::to_string(room->score));

  dynamicsWorld->stepSimulation(1/60.0, 100);

  int numManifolds = dispatcher->getNumManifolds();
  // for (int i=0;i<numManifolds;i++) {
  //   btPersistentManifold* contactManifold =  dispatcher->getManifoldByIndexInternal(i);
  //   const btCollisionObject* obA = static_cast<const btCollisionObject*>(contactManifold->getBody0());
  //   const btCollisionObject* obB = static_cast<const btCollisionObject*>(contactManifold->getBody1());

  //   int numContacts = contactManifold->getNumContacts();
  //   if ( numContacts < 1 ) 
  //     continue;

  //   if ( (btCollisionObject*)(ball->ballRigidBody) == obA ) {
  //     if ( (btCollisionObject*)(paddle->boxRigidBody) == obB ) {
  //       paddleHit = true;
  //       room->score += 1;
  //     } 
  //     else {
  //       wallHit = true;
  //     }
  //   } 
  //   else if ( (btCollisionObject*)(ball->ballRigidBody) == obB ) {
  //     if ( (btCollisionObject*)(paddle->boxRigidBody) == obA ) {
  //         paddleHit = true;
  //         room->score += 1;
  //     } 
  //     else { 
  //       wallHit = true;
  //     }
  //   }
  //   else {
  //       //gotta be wall and ball ... or wall and paddle?
  //   }
  // }

  // if () Mix_PlayChannel( -1, mWallImpact, 0 );
  if (soundStatus == 1) 
    Mix_PlayChannel( -1, mPaddleImpact, 0 );
}
//-------------------------------------------------------------------------------------
bool TutorialApplication::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
  if(mWindow->isClosed())
    return false;

  if(mShutDown)
    return false;

  //Need to capture/update each device
  mKeyboard->capture();
  mMouse->capture();

  //Need to inject timestamps to CEGUI System.
  CEGUI::System::getSingleton().injectTimePulse(evt.timeSinceLastFrame);

  if (mMode == PLAY) 
  {
    if (mPlayers == SERVER)
    {
      sendState();
    }
    else if (mPlayers == CLIENT)
    {
      checkState();
    }
    gameStep(evt);
  } 

  return true;
}
//-------------------------------------------------------------------------------------
bool TutorialApplication::keyPressed( const OIS::KeyEvent &arg )
{
  CEGUI::GUIContext &context = CEGUI::System::getSingleton().getDefaultGUIContext();
  context.injectKeyDown((CEGUI::Key::Scan)arg.key);
  context.injectChar((CEGUI::Key::Scan)arg.text);

  if (arg.key == OIS::KC_ESCAPE) 
  {
      //close the program
      mShutDown = true;
  }
  else if (arg.key == OIS::KC_Q)
  {
    if (mMode == PLAY) 
    {
      sheet->removeChild(pause);
      sheet->removeChild(scoreBox);
      sheet->addChild(pauseMenu);
      mMode = PAUSE;
    }
    else if (mMode == PAUSE) 
    {
      sheet->removeChild(pauseMenu);
      sheet->addChild(pause);
      sheet->addChild(scoreBox);
      mMode = PLAY;
    }

  }
  else 
  {
    if ( room && mMode==PLAY ) 
    {
        int type = 0; //default, for single player or server mode
        if ( mPlayers == CLIENT )
        {
            type = 132;
        }
        room->keyPressed(arg,type);
    }
  }

  return true;
}
//-------------------------------------------------------------------------------------
bool TutorialApplication::keyReleased( const OIS::KeyEvent &arg )
{
  CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp((CEGUI::Key::Scan)arg.key);

  if ( room && mMode==PLAY ) 
      room->keyReleased(arg);

  return true;
}
//-------------------------------------------------------------------------------------
bool TutorialApplication::mouseMoved( const OIS::MouseEvent &arg )
{
  CEGUI::GUIContext &context = CEGUI::System::getSingleton().getDefaultGUIContext();
  context.injectMouseMove(arg.state.X.rel, arg.state.Y.rel);
  
  // Scroll wheel.
  if (arg.state.Z.rel)
    context.injectMouseWheelChange(arg.state.Z.rel / 120.0f);
  return true;
}
//-------------------------------------------------------------------------------------
bool TutorialApplication::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(convertButton(id));
  return true;  
}
//-------------------------------------------------------------------------------------
bool TutorialApplication::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(convertButton(id));
  return true;
}
//-------------------------------------------------------------------------------------
CEGUI::MouseButton convertButton(OIS::MouseButtonID buttonID)
{
    switch (buttonID)
    {
    case OIS::MB_Left:
        return CEGUI::LeftButton;
 
    case OIS::MB_Right:
        return CEGUI::RightButton;
 
    case OIS::MB_Middle:
        return CEGUI::MiddleButton;
 
    default:
        return CEGUI::LeftButton;
    }
}
//-------------------------------------------------------------------------------------
int TutorialApplication::loadAudio()
{
    // Init SDL_mixer, used for sound management
    if ( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 ) 
    {
        std::fprintf(stderr, "Couldn't initialize SDL_mixer. Err: %s\n", Mix_GetError() );
        return -2;
    }
    // Attempt to load background music
    if ( !(mMusicData = Mix_LoadMUS( "bg.wav")) ) 
    {
        std::fprintf(stderr, "Err: %s\n", Mix_GetError() );
        return -4; 
    }
    // Attempt to load sound effects
    if ( !(mWallImpact = Mix_LoadWAV( "wallImpact.wav" )) ) 
    { 
        std::fprintf(stderr, "Err: %s\n", Mix_GetError() ) ;
        return -5; 
    }
    if ( !(mPaddleImpact = Mix_LoadWAV( "paddleImpact.wav" )) ) 
    { 
        std::fprintf(stderr, "Err: %s\n", Mix_GetError() ) ;
        return -5; 
    }

    return 0;
}
//-------------------------------------------------------------------------------------
void TutorialApplication::startBGM() {
    mMusicState = MUSPLAY;
    // Loops background music
    if ( Mix_PlayMusic( mMusicData, -1 ) == -1 ) {
        std::fprintf(stderr, "Err: %s\n", Mix_GetError() );
    }
}
//-------------------------------------------------------------------------------------
void TutorialApplication::closeAudio() 
{
    Mix_FreeChunk( mWallImpact );
    Mix_FreeChunk( mPaddleImpact);
    Mix_FreeMusic( mMusicData );
    Mix_CloseAudio();
    SDL_Quit();
}
//-------------------------------------------------------------------------------------
bool TutorialApplication::quit(const CEGUI::EventArgs &e)
{
  mShutDown = true;
  return true;
}
//-------------------------------------------------------------------------------------
bool TutorialApplication::startSinglePlayer(const CEGUI::EventArgs &e)
{
  sheet->addChild(pause);
  sheet->addChild(scoreBox);
  sheet->removeChild(menu);
  paddle2->setPosition(0,0,room->getLength()+1000);
  paddle->setPosition(-(room->getWidth()/2)+50, 0, 0);
  mMode = PLAY;
  mPlayers = SINGLE;
  return true;
}
//-------------------------------------------------------------------------------------
bool TutorialApplication::openSettingsMenu(const CEGUI::EventArgs &e)
{
  sheet->removeChild(menu);
  sheet->addChild(settingsMenu);
  return true;
}
//-------------------------------------------------------------------------------------
bool TutorialApplication::goBack(const CEGUI::EventArgs &e)
{
  sheet->removeChild(settingsMenu);
  sheet->addChild(menu);
  return true;
}
//-------------------------------------------------------------------------------------
bool TutorialApplication::changeSound(const CEGUI::EventArgs &e)
{
  if (mMusicState == MUSPLAY) 
  {
    mMusicState = MUSPAUSE;
    Mix_PauseMusic();
    sound->setText("Enable Sound");
    soundPause->setText("Enable Sound");
  } 
  else if (mMusicState == MUSPAUSE) 
  {
    mMusicState = MUSPLAY;
    Mix_ResumeMusic();
    sound->setText("Disable Sound");
    soundPause->setText("Disable Sound");
  }
  return true;
}
//-------------------------------------------------------------------------------------
bool TutorialApplication::pauseGame(const CEGUI::EventArgs &e)
{
  sheet->removeChild(pause);
  sheet->removeChild(scoreBox);
  sheet->addChild(pauseMenu);
  mMode = PAUSE;
  return true;
}
//-------------------------------------------------------------------------------------
bool TutorialApplication::resumeGame(const CEGUI::EventArgs &e)
{
  sheet->removeChild(pauseMenu);
  sheet->addChild(pause);
  sheet->addChild(scoreBox);
  mMode = PLAY;
  return true;
}
//-------------------------------------------------------------------------------------
bool TutorialApplication::openMainMenu(const CEGUI::EventArgs &e)
{
  sheet->removeChild(pauseMenu);
  sheet->addChild(menu);
  mMode = MENU;

  //restart the game
  paddle->setPosition(-(room->getWidth()/2)+50, 0, 0);
  ball->reset(dynamicsWorld);
  // ball->ballNode->setPosition(0,0,0);
  room->score = 0;
  room->gameOver = false;

  return true;
}
//-------------------------------------------------------------------------------------
bool TutorialApplication::playAgain(const CEGUI::EventArgs &e)
{
  sheet->removeChild(overMenu);
  sheet->addChild(pause);
  mMode = PLAY;

  //restart the game
  paddle->setPosition(-(room->getWidth()/2)+50, 0, 0);
  ball->reset(dynamicsWorld);
  // ball->ballNode->setPosition(0,0,0);
  room->score = 0;
  room->gameOver = false;

  return true;
}
//-------------------------------------------------------------------------------------
bool TutorialApplication::reopenMainMenu(const CEGUI::EventArgs &e)
{
  sheet->removeChild(overMenu);
  sheet->addChild(menu);
  mMode = MENU;

  //restart the game
  paddle->setPosition(-(room->getWidth()/2)+50, 0, 0);
  ball->reset(dynamicsWorld);
  // ball->ballNode->setPosition(0,0,0);
  room->score = 0;
  room->gameOver = false;

  return true;
}
//-------------------------------------------------------------------------------------
bool TutorialApplication::startMultiPlayer(const CEGUI::EventArgs &e)
{
  sheet->removeChild(menu);
  sheet->addChild(multiMenu);

  return true;
}
//-------------------------------------------------------------------------------------
bool TutorialApplication::goBackMulti(const CEGUI::EventArgs &e)
{
  sheet->removeChild(multiMenu);
  sheet->addChild(menu);

  return true;
}
//-------------------------------------------------------------------------------------
bool TutorialApplication::joinGame(const CEGUI::EventArgs &e)
{
  sheet->removeChild(multiMenu);
  sheet->addChild(joinMenu);

  return true;
}
//-------------------------------------------------------------------------------------
bool TutorialApplication::goBackJoin(const CEGUI::EventArgs &e)
{
  sheet->removeChild(joinMenu);
  sheet->addChild(multiMenu);

  return true;
}
//-------------------------------------------------------------------------------------
bool TutorialApplication::gameSearch(const CEGUI::EventArgs &e)
{
  CEGUI::String address = editBox->getText();

  //verify valid IP here

  if ( beginClient( address.c_str() ) )
  {
    //don't allow connection on same IP - crash
    if ( !address.compare( mNetMgr->getIPstring() ) ) return true;
    //start game, exit menu
    sheet->removeChild(joinMenu);

    mMode = PLAY;
    mPlayers = CLIENT;
  }

  return true;
}
//-------------------------------------------------------------------------------------
bool TutorialApplication::startMultiGame(const CEGUI::EventArgs &e)
{
  if ( beginServer() ) {
    if ( mNetMgr->pollForActivity(5000) ) {
  //don't accept any more connections...any past the first are ignored on this end
  //    mNetMgr->denyConnections();
  //start game, exit menu
      if ( mNetMgr->getClients() ) {
        sheet->removeChild(multiMenu);
        mMode = PLAY;
        mPlayers = SERVER;
      }
    }
  }
  return true;
}
//-------------------------------------------------------------------------------------
bool TutorialApplication::openHowToPlayMenu(const CEGUI::EventArgs &e)
{
  sheet->removeChild(menu);
  sheet->addChild(howToPlayMenu);

  return true;
}
//-------------------------------------------------------------------------------------
bool TutorialApplication::goBackHTP(const CEGUI::EventArgs &e)
{
  sheet->removeChild(howToPlayMenu);
  sheet->addChild(menu);

  return true;
}
//-------------------------------------------------------------------------------------

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
    int main(int argc, char *argv[])
#endif
    {
        
        // Init SDL
        if ( SDL_Init( SDL_INIT_AUDIO ) < 0 ) 
        {
            std::fprintf(stderr, "Couldn't initialize SDL. Err: %s\n", SDL_GetError() );
            return -1;
        }

        // Create application object
        TutorialApplication app;

        try {
            app.loadAudio();
            app.startBGM();
            app.go();
        } catch(Ogre::Exception& e)  {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBox(NULL, e.getFullDescription().c_str(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occurred: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }

        return 0;
    }

#ifdef __cplusplus
}
#endif

//---------------------------------------------------------------------------
