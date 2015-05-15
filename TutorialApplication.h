/*
-----------------------------------------------------------------------------
Filename:    TutorialApplication.h
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

#ifndef __TutorialApplication_h_
#define __TutorialApplication_h_

#include "BaseApplication.h"
#include "Room.h"
#include "Ball.h"

#include <string>
#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_net.h>
#include "NetManager.h"
#include <vector>
#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/Ogre/Renderer.h>


//---------------------------------------------------------------------------

class TutorialApplication : public BaseApplication
{
public:
    TutorialApplication(void);
    virtual ~TutorialApplication(void);

    // Wrap music initiation and closing
    int  loadAudio();
    void startBGM();
    void closeAudio();

    //Bullet globals
    btBroadphaseInterface* broadphase;
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld* dynamicsWorld;

    //CEGUI globals
    CEGUI::Window *sheet;
    CEGUI::Window *menu;
    CEGUI::Window *settingsMenu;
    CEGUI::Window *sound;
    CEGUI::Window *pause;
    CEGUI::Window *pauseMenu;
    CEGUI::Window *soundPause;
    CEGUI::Window *scoreBox;
    CEGUI::Window *overMenu;
    CEGUI::Window *multiMenu;
    CEGUI::Window *joinMenu;
    CEGUI::Window *editBox;
    CEGUI::Window *howToPlayMenu;

    //Game globals
    Ball *ball;
    Room *room;
    Paddle *paddle;
    Paddle *paddle2;
    Ogre::Vector3 ballDirection;

    void sendState();
protected:
    CEGUI::OgreRenderer* mRenderer;

    virtual void createScene(void);
    virtual void destroyScene(void);
    virtual void createCamera(void);
    virtual void createFrameListener(void);
 
    // Run one game step
    virtual void gameStep(const Ogre::FrameEvent& evt);

    // Ogre::FrameListener
    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
 
    // OIS::KeyListener
    virtual bool keyPressed( const OIS::KeyEvent &arg );
    virtual bool keyReleased( const OIS::KeyEvent &arg );
    // OIS::MouseListener
    virtual bool mouseMoved( const OIS::MouseEvent &arg );
    virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
    virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

    // Helper methods for networking
    bool beginServer();
    void endNetwork();
    bool beginClient(const char* hostIP);
    void checkState();

    //methods for button events
    bool quit(const CEGUI::EventArgs &e);
    bool startSinglePlayer(const CEGUI::EventArgs &e);
    bool openSettingsMenu(const CEGUI::EventArgs &e);
    bool goBack(const CEGUI::EventArgs &e);
    bool changeSound(const CEGUI::EventArgs &e);
    bool pauseGame(const CEGUI::EventArgs &e);
    bool resumeGame(const CEGUI::EventArgs &e);
    bool openMainMenu(const CEGUI::EventArgs &e);
    bool playAgain(const CEGUI::EventArgs &e);
    bool reopenMainMenu(const CEGUI::EventArgs &e);
    bool startMultiPlayer(const CEGUI::EventArgs &e);
    bool goBackMulti(const CEGUI::EventArgs &e);
    bool joinGame(const CEGUI::EventArgs &e);
    bool goBackJoin(const CEGUI::EventArgs &e);
    bool gameSearch(const CEGUI::EventArgs &e);
    bool startMultiGame(const CEGUI::EventArgs &e);
    bool openHowToPlayMenu(const CEGUI::EventArgs &e);
    bool goBackHTP(const CEGUI::EventArgs &e);

private:
    // Type enumeration of possible game states
    enum GameState {PLAY,PAUSE,MENU,OVER};
    GameState mMode; 
    enum PlayerState {SINGLE,SERVER,CLIENT};
    PlayerState mPlayers;

    // Audio data
    enum AudioState {MUSPLAY, MUSPAUSE};
    AudioState mMusicState;
    Mix_Music* mMusicData;
    Mix_Chunk* mWallImpact;
    Mix_Chunk* mPaddleImpact;

    // Network Manager
    NetManager* mNetMgr;
};

//---------------------------------------------------------------------------

#endif // #ifndef __TutorialApplication_h_

//---------------------------------------------------------------------------
