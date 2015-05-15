#ifndef _PADDLE_H_
#define _PADDLE_H

#include <Ogre.h>
#include <btBulletDynamicsCommon.h>

class Paddle
{
    public:
        Paddle(Ogre::Real width, Ogre::Real height, Ogre::Real length, std::string name);
        ~Paddle();

        Ogre::Real width;
        Ogre::Real length;
        Ogre::Real height;
        std::string name;

        Ogre::SceneNode* paddleNode;
        Ogre::Real paddleSpeed;
        int paddleDirection[2];
        btRigidBody* boxRigidBody;

        void renderObject(Ogre::SceneManager* mSceneMgr);
        void createMeshes();
        void setPosition(Ogre::Real x, Ogre::Real y, Ogre::Real z);
        void modifyDirection(int x, int y);
        void move(const Ogre::FrameEvent& evt);

        void initializeBullet(btDiscreteDynamicsWorld*);

        Ogre::Vector3 getPosition();
        Ogre::Real getWidth();
        Ogre::Real getLength();
        Ogre::Real getHeight();
};

#endif
