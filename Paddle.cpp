#include "Paddle.h"

/**
 * the main constructor for Paddle
 */
Paddle::Paddle(Ogre::Real width, Ogre::Real height, Ogre::Real length, std::string name) :
    width(width),
    height(height),
    length(length),
    name(name)
{
    paddleNode = NULL;
    paddleSpeed = 1.5f;
    paddleDirection[0] = 0;
    paddleDirection[1] = 0;
}

/**
 * the main destructor for Paddle
 */

Paddle::~Paddle()
{
}

/**
 * this function creates the Paddle in the object space
 * the Paddle is represented in a similar fashion as the Room
 */
void Paddle::renderObject(Ogre::SceneManager* mSceneMgr )
{
    // create the paddle node
    paddleNode = mSceneMgr->createSceneNode(name);

    // create the meshes
    createMeshes();

    // create the entities
    Ogre::Entity *paddleLeftEntity   = mSceneMgr->createEntity(name+"LeftEntity", name+"Left");
    Ogre::Entity *paddleRightEntity  = mSceneMgr->createEntity(name+"RightEntity", name+"Right");
    Ogre::Entity *paddleBottomEntity = mSceneMgr->createEntity(name+"BottomEntity", name+"Bottom");
    Ogre::Entity *paddleTopEntity    = mSceneMgr->createEntity(name+"TopEntity", name+"Top");
    Ogre::Entity *paddleFrontEntity  = mSceneMgr->createEntity(name+"FrontEntity", name+"Front");
    Ogre::Entity *paddleBackEntity   = mSceneMgr->createEntity(name+"BackEntity", name+"Back");

    // create the corresponding nodes
    Ogre::SceneNode *paddleLeftNode   = paddleNode->createChildSceneNode(name+"LeftNode");
    Ogre::SceneNode *paddleRightNode  = paddleNode->createChildSceneNode(name+"RightNode");
    Ogre::SceneNode *paddleBottomNode = paddleNode->createChildSceneNode(name+"BottomNode");
    Ogre::SceneNode *paddleTopNode    = paddleNode->createChildSceneNode(name+"TopNode");
    Ogre::SceneNode *paddleFrontNode  = paddleNode->createChildSceneNode(name+"FrontNode");
    Ogre::SceneNode *paddleBackNode   = paddleNode->createChildSceneNode(name+"BackNode");

    // set the material of each entity
    paddleLeftEntity->setMaterialName("Examples/Rockwall");
    paddleRightEntity->setMaterialName("Examples/Rockwall");
    paddleBottomEntity->setMaterialName("Examples/Rockwall");
    paddleTopEntity->setMaterialName("Examples/Rockwall");
    paddleFrontEntity->setMaterialName("Examples/Rockwall");
    paddleBackEntity->setMaterialName("Examples/Rockwall");

    // attach the entities to the nodes
    paddleLeftNode->attachObject(paddleLeftEntity);
    paddleRightNode->attachObject(paddleRightEntity);
    paddleBottomNode->attachObject(paddleBottomEntity);
    paddleTopNode->attachObject(paddleTopEntity);
    paddleFrontNode->attachObject(paddleFrontEntity);
    paddleBackNode->attachObject(paddleBackEntity);

    // add this node to the root node
    mSceneMgr->getRootSceneNode()->addChild(paddleNode);
}

/**
 * a helper function called in renderObject
 * this function creates the meshes used in the six components of the wall
 */
void Paddle::createMeshes()
{
    // create the six planes
    Ogre::Plane paddleLeftPlane(Ogre::Vector3::UNIT_X,   width / 2);
    Ogre::Plane paddleRightPlane(-Ogre::Vector3::UNIT_X, width / 2);

    Ogre::Plane paddleBottomPlane(Ogre::Vector3::UNIT_Y, height / 2);
    Ogre::Plane paddleTopPlane(-Ogre::Vector3::UNIT_Y,   height / 2);

    Ogre::Plane paddleFrontPlane(Ogre::Vector3::UNIT_Z,  length / 2);
    Ogre::Plane paddleBackPlane(-Ogre::Vector3::UNIT_Z,  length / 2);

    // get the mesh manager and group name (for convenience
    Ogre::MeshManager& meshMgr = Ogre::MeshManager::getSingleton();
    std::string& defaultGroupName = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME;

    // create the meshes from the planes
    meshMgr.createPlane(name+"Bottom"     , defaultGroupName , paddleBottomPlane, width  , length , 20 , 20 , true , 1 , 50 , 50 , Ogre::Vector3::UNIT_Z);
    meshMgr.createPlane(name+"Top"        , defaultGroupName , paddleTopPlane   , width  , length , 20 , 20 , true , 1 , 50 , 50 , Ogre::Vector3::UNIT_Z);
    meshMgr.createPlane(name+"Left"       , defaultGroupName , paddleLeftPlane  , height , length , 20 , 20 , true , 1 , 50 , 50 , Ogre::Vector3::UNIT_Z);
    meshMgr.createPlane(name+"Right"      , defaultGroupName , paddleRightPlane , height , length , 20 , 20 , true , 1 , 50 , 50 , Ogre::Vector3::UNIT_Z);
    meshMgr.createPlane(name+"Front"      , defaultGroupName , paddleFrontPlane , width  , height , 20 , 20 , true , 1 , 50 , 50 , Ogre::Vector3::UNIT_Y);
    meshMgr.createPlane(name+"Back"       , defaultGroupName , paddleBackPlane  , width  , height , 20 , 20 , true , 1 , 50 , 50 , Ogre::Vector3::UNIT_Y);
}

/**
 * initialize bullet
 * note that this function must be called after createScene
 */
void Paddle::initializeBullet(btDiscreteDynamicsWorld* dynamicsWorld)
{
    if (paddleNode == NULL) {
        return;
    }

    Ogre::Vector3 paddlePosition = paddleNode->getPosition();

    btVector3 position(paddlePosition.x,
                       paddlePosition.y,
                       paddlePosition.z);

    btCollisionShape* boxShape = new btBoxShape(btVector3(width / 2, height / 2, length / 2));
    btDefaultMotionState* boxMotionState = 
        new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), position));

    btScalar mass = 100;

    btRigidBody::btRigidBodyConstructionInfo boxRigidBodyCI(mass, boxMotionState, boxShape);
    boxRigidBody = new btRigidBody(boxRigidBodyCI);
    boxRigidBody->setRestitution(1.1f);

    dynamicsWorld->addRigidBody(boxRigidBody);
}

/**
 * that moves the paddle to the corresponding location
 */
void Paddle::setPosition(Ogre::Real x, Ogre::Real y, Ogre::Real z)
{
    if (boxRigidBody) {
        btTransform trans = boxRigidBody->getCenterOfMassTransform();
        trans.setOrigin(btVector3(x, y, z));
        boxRigidBody->setCenterOfMassTransform(trans);
    }

    paddleNode->setPosition(x, y, z);
}

/**
 * returns the position of the node
 */
Ogre::Vector3 Paddle::getPosition()
{
    btTransform trans = boxRigidBody->getCenterOfMassTransform();
    btVector3 pos = trans.getOrigin();
    return Ogre::Vector3(pos.x(), pos.y(), pos.z());
}

/**
 * sets the direction of the paddle
 * this is called in keyPressed and keyReleased
 */
void Paddle::modifyDirection(int z, int y)
{
    paddleDirection[0] += z;
    paddleDirection[1] += y;
}

/**
 * moves the Paddle
 * this is called in frameRenderingQueued
 */
void Paddle::move(const Ogre::FrameEvent& evt)
{
    // ball hasn't been rendered yet
    if (paddleNode == NULL) {
        return;
    }

    // move the ball
    Ogre::Vector3 pos = getPosition();

    Ogre::Real x = pos.x;
    Ogre::Real y = pos.y + paddleDirection[1] * paddleSpeed;
    Ogre::Real z = pos.z + paddleDirection[0] * paddleSpeed;

    setPosition(x, y, z);
}

/**
 * a getter for width
 */
Ogre::Real Paddle::getWidth()
{
    return width;
}

/**
 * a getter for length
 */
Ogre::Real Paddle::getLength()
{
    return length;
}

/**
 * a getter for height
 */
Ogre::Real Paddle::getHeight()
{
    return height;
}
