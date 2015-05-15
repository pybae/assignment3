#ifndef _BALL_H_
#define _BALL_H_

#include <Ogre.h>
#include <btBulletDynamicsCommon.h>
#include <math.h>

class Ball
{
    public:
        Ball(Ogre::Vector3, Ogre::Real);
        ~Ball();

        void renderObject(Ogre::SceneManager* mSceneMgr);
        void initializeBullet(btDiscreteDynamicsWorld* dynamicsWorld);
        void move(const Ogre::FrameEvent& evt);
        void reset(btDiscreteDynamicsWorld* dynamicsWorld);

        Ogre::SceneNode*      ballNode;
        btRigidBody*          ballRigidBody;
        Ogre::Vector3         ballDirection;
        Ogre::Real            ballRadius;
        Ogre::Vector3         ballPosition;
        btCollisionShape*     ballShape;
        btDefaultMotionState* ballMotionState;
};

#endif
