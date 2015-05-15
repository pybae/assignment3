#include "Ball.h"

#include <ctime>
#include <cstdlib>

/**
 * the main constructor to call
 */
Ball::Ball(Ogre::Vector3 ballDirection, Ogre::Real ballRadius) :
    ballDirection(ballDirection),
    ballRadius(ballRadius)
{
    ballNode = NULL;
}

/**
 * the destructor for Ball
 */
Ball::~Ball()
{
}

/**
 * renders the Ball
 * we've modularized this function apart from the Ball's constructor for more
 * flexibility in createScene
 */
void Ball::renderObject(Ogre::SceneManager* mSceneMgr)
{
    Ogre::Entity* ballEntity = mSceneMgr->createEntity("Ball", "sphere.mesh");
    ballEntity->setCastShadows(true);

    ballNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("sphereNode");
    ballNode->attachObject(ballEntity);
}

/**
 * links up the class with bullet
 * note that this cannot be called before renderObject
 */
void Ball::initializeBullet(btDiscreteDynamicsWorld* dynamicsWorld)
{
    if (ballNode == NULL) {
        return;
    }

    ballPosition = ballNode->getPosition();

    btVector3 position(ballPosition.x,
                       ballPosition.y,
                       ballPosition.z);

    ballShape = new btSphereShape(ballRadius);
    ballMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), position));

    btScalar mass = 100;
    //btVector3 ballInertia(0, 0, 0);
    //ballShape->calculateLocalInertia(mass, ballInertia);
    btRigidBody::btRigidBodyConstructionInfo ballRigidBodyCI(mass, ballMotionState, ballShape);
    ballRigidBody = new btRigidBody(ballRigidBodyCI);

    ballRigidBody->setRestitution(1.0f);

    srand(time(NULL));
    btVector3 direction(10.0f * (rand() * 1.0f / RAND_MAX),
                        10.0f * (rand() * 1.0f / RAND_MAX),
                        10.0f * (rand() * 1.0f / RAND_MAX));

    ballRigidBody->setLinearVelocity(direction);

    dynamicsWorld->addRigidBody(ballRigidBody);
}

/**
 * moves the Ball
 * this is called in frameRenderingQueued
 */
void Ball::move(const Ogre::FrameEvent& evt)
{
    // ball hasn't been rendered yet
    if (ballNode == NULL) {
        return;
    }

    btTransform trans;
    ballRigidBody->getMotionState()->getWorldTransform(trans);
    btVector3 pos = trans.getOrigin();

    ballNode->setPosition(pos.x(), pos.y(), pos.z());

    // printf("%f %f %f\n", pos.x(), pos.y(), pos.z());
    //move the ball
    //ballNode->translate(ballSpeed * ballDirection * evt.timeSinceLastFrame);
}

void Ball::reset(btDiscreteDynamicsWorld* dynamicsWorld)
{
    btVector3 position(0,0,0);

    ballShape = new btSphereShape(ballRadius);
    ballMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), position));

    btScalar mass = 100;
    //btVector3 ballInertia(0, 0, 0);
    //ballShape->calculateLocalInertia(mass, ballInertia);
    btRigidBody::btRigidBodyConstructionInfo ballRigidBodyCI(mass, ballMotionState, ballShape);
    ballRigidBody = new btRigidBody(ballRigidBodyCI);

    ballRigidBody->setRestitution(1.0f);

    srand(time(NULL));
    btVector3 direction(10.0f * (rand() * 1.0f / RAND_MAX),
                        10.0f * (rand() * 1.0f / RAND_MAX),
                        10.0f * (rand() * 1.0f / RAND_MAX));

    ballRigidBody->setLinearVelocity(direction);

    dynamicsWorld->addRigidBody(ballRigidBody);
}
