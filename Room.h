#ifndef _ROOM_H_
#define _ROOM_H_

#include <Ogre.h>
#include <OISMouse.h>
#include <OISKeyboard.h>
#include "Ball.h"
#include "Paddle.h"

class Room
{
    private:
        Ogre::Real width;
        Ogre::Real length;
        Ogre::Real height;

        void createMeshes();
        void createLights(Ogre::SceneManager* mSceneMgr);
        void createWall(btDiscreteDynamicsWorld* dynamicsWorld, Ogre::Real length, Ogre::Real width, btVector3 location);
        void wallCollision(Ogre::Vector3, Ogre::Vector3, Ogre::Real);
        bool paddleCollision(Ogre::Vector3, Ogre::Vector3, Ogre::Real);
        Ogre::Vector3 paddleWallCollision(Ogre::Vector3, Ogre::Real, Ogre::Real);

    public:
        Room(Paddle*, Paddle*, Ball*, Ogre::Real, Ogre::Real, Ogre::Real);
        Room(Paddle*, Paddle*, Ball*);
        Room(Paddle*, Ball*);
        ~Room();
        int collision(const Ogre::FrameEvent& evt);
        void createScene(Ogre::SceneManager* mSceneMgr);
        void initializeBullet(btDiscreteDynamicsWorld* dynamicsWorld);
        bool keyPressed(const OIS::KeyEvent &arg, int type);
        bool keyReleased(const OIS::KeyEvent &arg);

        Ball* ball;
        Paddle* paddle;
        Paddle* paddle2;

        Ogre::Real getWidth();
        Ogre::Real getLength();
        Ogre::Real getHeight();
        int getScore();
        int score;
        bool gameOver;
};

#endif
