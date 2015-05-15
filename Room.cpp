#include "Room.h"
#include <iostream>

#define roomWidth 2050.0f
#define roomLength 1350.0
#define roomHeight 1000.0f

/**
 * the main constructor for Room
 * each Room can contain a Ball, and thus a Ball will have a pointer to a Room
 */
Room::Room(Paddle* paddle, Paddle* paddle2, Ball* ball, Ogre::Real width, Ogre::Real length, Ogre::Real height) :
    paddle(paddle),
    paddle2(paddle2),
    ball(ball),
    width(width),
    length(length),
    height(height),
    score(0),
    gameOver(false)
{
}

Room::Room(Paddle* paddle, Paddle* paddle2, Ball* ball) :
    paddle(paddle),
    paddle2(paddle2),
    ball(ball),
    width(roomWidth),
    length(roomLength),
    height(roomHeight),
    score(0),
    gameOver(false)
{
}

Room::Room(Paddle* paddle, Ball* ball) :
    paddle(paddle),
    paddle2(NULL),
    ball(ball),
    width(roomWidth),
    length(roomLength),
    height(roomHeight),
    score(0),
    gameOver(false)
{
}

/**
 * the destructor for Room
 */
Room::~Room()
{
}

/**
 * createScene initializes the Room and makes it visible to the sceneMgr passed
 * in
 */
void Room::createScene(Ogre::SceneManager* sceneMgr)
{
    // create the lights for the room
    createLights(sceneMgr);

    // create the meshes to be used for the room (the four walls, ceiling, and floor)
    createMeshes();

    // create the entities
    Ogre::Entity *floorEntity     = sceneMgr->createEntity("FloorEntity"     , "floor");
    Ogre::Entity *ceilingEntity   = sceneMgr->createEntity("CeilingEntity"   , "ceiling");
    Ogre::Entity *wallLeftEntity  = sceneMgr->createEntity("WallLeftEntity"  , "wallLeft");
    Ogre::Entity *wallRightEntity = sceneMgr->createEntity("WallRightEntity" , "wallRight");
    Ogre::Entity *wallFrontEntity = sceneMgr->createEntity("WallFrontEntity" , "wallFront");
    Ogre::Entity *wallBackEntity  = sceneMgr->createEntity("WallBackEntity"  , "wallBack");

    // create the corresponding nodes
    Ogre::SceneNode *floorNode     = sceneMgr->getRootSceneNode()->createChildSceneNode("FloorNode");
    Ogre::SceneNode *ceilingNode   = sceneMgr->getRootSceneNode()->createChildSceneNode("CeilingNode");
    Ogre::SceneNode *wallLeftNode  = sceneMgr->getRootSceneNode()->createChildSceneNode("WallLeftNode");
    Ogre::SceneNode *wallRightNode = sceneMgr->getRootSceneNode()->createChildSceneNode("WallRightNode");
    Ogre::SceneNode *wallFrontNode = sceneMgr->getRootSceneNode()->createChildSceneNode("WallFrontNode");
    Ogre::SceneNode *wallBackNode  = sceneMgr->getRootSceneNode()->createChildSceneNode("WallBackNode");

    // set the material of each entity
    floorEntity->setMaterialName("");
    ceilingEntity->setMaterialName("");
    wallLeftEntity->setMaterialName("");
    wallRightEntity->setMaterialName("");
    wallFrontEntity->setMaterialName("");
    wallBackEntity->setMaterialName("");

    // turn shadows off for each entity
    floorEntity->setCastShadows(false);
    ceilingEntity->setCastShadows(false);
    wallLeftEntity->setCastShadows(false);
    wallRightEntity->setCastShadows(false);
    wallFrontEntity->setCastShadows(false);
    wallBackEntity->setCastShadows(false);

    // set the positions of each node
    floorNode->setPosition(0, -height/2, 0);
    ceilingNode->setPosition(0, height/2, 0);

    wallLeftNode->setPosition(-width/2, 0, 0);
    wallRightNode->setPosition(width/2, 0, 0);

    wallFrontNode->setPosition(0, 0, -length/2);
    wallBackNode->setPosition(0, 0, length/2);

    // attach the entities to the nodes
    floorNode->attachObject(floorEntity);
    ceilingNode->attachObject(ceilingEntity);
    wallLeftNode->attachObject(wallLeftEntity);
    wallRightNode->attachObject(wallRightEntity);
    wallFrontNode->attachObject(wallFrontEntity);
    wallBackNode->attachObject(wallBackEntity);
}

/**
 * a helper function called in createScene
 * this function creates the meshes used in the six components of the wall
 */
void Room::createMeshes()
{
    // create the six planes
    Ogre::Plane floorPlane(Ogre::Vector3::UNIT_Y, 0);
    Ogre::Plane ceilingPlane(-Ogre::Vector3::UNIT_Y, 0);

    Ogre::Plane wallLeftPlane(Ogre::Vector3::UNIT_X, 0);
    Ogre::Plane wallRightPlane(-Ogre::Vector3::UNIT_X, 0);

    Ogre::Plane wallFrontPlane(Ogre::Vector3::UNIT_Z, 0);
    Ogre::Plane wallBackPlane(-Ogre::Vector3::UNIT_Z, 0);

    // get the mesh manager and group name (for convenience
    Ogre::MeshManager& meshMgr = Ogre::MeshManager::getSingleton();
    std::string& defaultGroupName = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME;

    // create the meshes from the planes
    meshMgr.createPlane("floor"          , defaultGroupName , floorPlane     , width  , length , 20 , 20 , true , 1 , 5, 5 , Ogre::Vector3::UNIT_Z);
    meshMgr.createPlane("ceiling"        , defaultGroupName , ceilingPlane   , width  , length , 20 , 20 , true , 1 , 5, 5 , Ogre::Vector3::UNIT_Z);
    meshMgr.createPlane("wallLeft"       , defaultGroupName , wallLeftPlane  , height , length , 20 , 20 , true , 1 , 5, 5 , Ogre::Vector3::UNIT_Z);
    meshMgr.createPlane("wallRight"      , defaultGroupName , wallRightPlane , height , length , 20 , 20 , true , 1 , 5, 5 , Ogre::Vector3::UNIT_Z);
    meshMgr.createPlane("wallFront"      , defaultGroupName , wallFrontPlane , width  , height , 20 , 20 , true , 1 , 5, 5 , Ogre::Vector3::UNIT_Y);
    meshMgr.createPlane("wallBack"       , defaultGroupName , wallBackPlane  , width  , height , 20 , 20 , true , 1 , 5, 5 , Ogre::Vector3::UNIT_Y);
}

/**
 * a helper function called in createScene
 * this function initializes the lights
 */
void Room::createLights(Ogre::SceneManager* mSceneMgr)
{
    // point light
    Ogre::Light* pointLight = mSceneMgr->createLight("pointLight");
    pointLight->setType(Ogre::Light::LT_POINT);
    pointLight->setPosition(Ogre::Vector3(0, 150, 250));
    pointLight->setDiffuseColour(1.0, 1.0, 1.0);
    pointLight->setSpecularColour(1.0, 1.0, 1.0);

    // directional light
    Ogre::Light* directionalLight = mSceneMgr->createLight("directionalLight");
    directionalLight->setType(Ogre::Light::LT_DIRECTIONAL);
    directionalLight->setDiffuseColour(Ogre::ColourValue(.25, .25, 0));
    directionalLight->setSpecularColour(Ogre::ColourValue(.25, .25, 0));
    directionalLight->setDirection(Ogre::Vector3( 0, -1, 1 ));

    // spot light
    Ogre::Light* spotLight = mSceneMgr->createLight("spotLight");
    spotLight->setType(Ogre::Light::LT_SPOTLIGHT);
    spotLight->setDiffuseColour(0, 0, 1.0);
    spotLight->setSpecularColour(0, 0, 1.0);
    spotLight->setDirection(-1, -1, 0);
    spotLight->setPosition(Ogre::Vector3(300, 300, 0));
    spotLight->setSpotlightRange(Ogre::Degree(35), Ogre::Degree(50));
}

/**
 * initialize bullet
 * note that this function must be called after createScene
 */
void Room::initializeBullet(btDiscreteDynamicsWorld* dynamicsWorld)
{
    createWall(dynamicsWorld, height, width, btVector3(0, 0,  length / 2));
    createWall(dynamicsWorld, height, width, btVector3(0, 0, -length / 2));
    createWall(dynamicsWorld, length, width, btVector3(0,  height / 2, 0));
    createWall(dynamicsWorld, length, width, btVector3(0, -height / 2, 0));
    createWall(dynamicsWorld, height, length,btVector3( width / 2, 0, 0));
    createWall(dynamicsWorld, height, length,btVector3(-width / 2, 0, 0));
}

/**
 * initialize a wall within Bullet
 */
void Room::createWall(btDiscreteDynamicsWorld* dynamicsWorld, Ogre::Real length, Ogre::Real width, btVector3 location)
{
    btDefaultMotionState* groundMotionState =
        new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), location));
    btCollisionShape* groundShape = new btStaticPlaneShape(-1.0f * location.normalized(), 0);

    btRigidBody::btRigidBodyConstructionInfo
                groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
    btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);

    groundRigidBody->setRestitution(1.0f);
    dynamicsWorld->addRigidBody(groundRigidBody);
}

/**
 * handle collision detection
 */
 int Room::collision(const Ogre::FrameEvent& evt)
 {
    int ret = 0;
    Ogre::Vector3 ballPosition = ball->ballNode->getPosition();
    Ogre::Vector3 ballDirection = ball->ballDirection;
    Ogre::Real ballRadius = ball->ballRadius;

    wallCollision(ballPosition, ballDirection, ballRadius);
    if(paddleCollision(ballPosition, ballDirection, ballRadius))
        ret = 1;

    //handle collisions between the room and the paddle
    Ogre::Vector3 paddlePosition = paddle->getPosition();
    Ogre::Real paddleHeight = paddle->getHeight();
    Ogre::Real paddleLength = paddle->getLength();

    paddlePosition = paddleWallCollision(paddlePosition, paddleHeight, paddleLength);

    ball->ballDirection = ballDirection;
    ball->move(evt);
    paddle->move(evt);
    paddle2->move(evt);
    return ret;
}

/**
 * a helper function that detects collision between the ball and the walls
 */
void Room::wallCollision(Ogre::Vector3 ballPosition,
                         Ogre::Vector3 ballDirection,
                         Ogre::Real ballRadius)
{
    // std::cout << ballPosition.x << " " << -(width/2.0f) + ballRadius << std::endl;
    if(ballPosition.x <= -(width/2.0f) + ballRadius)
      gameOver = true;
}

/**
 * a helper function that detects collision between the ball and the paddle
 */
bool Room::paddleCollision(Ogre::Vector3 ballPosition,
                           Ogre::Vector3 ballDirection,
                           Ogre::Real ballRadius)
{
    const Ogre::Vector3& paddlePosition = paddle->getPosition();

    // implement sound and score counter here
    if (abs(ballPosition.x - paddlePosition.x) < ballRadius + paddle->getWidth()/2) {
        if (abs(ballPosition.y - paddlePosition.y) < ballRadius + paddle->getHeight()/2) {
            if (abs(ballPosition.z - paddlePosition.z) < ballRadius + paddle->getLength()/2) {
                score++;
                return true;
            }
        }
    }
    return false;
}

/**
 * a helper function that detects collisions between the paddle and the room
 */
 Ogre::Vector3 Room::paddleWallCollision(Ogre::Vector3 position,
                                         Ogre::Real paddleHeight,
                                         Ogre::Real paddleLength)
 {

    if (position.y + paddleHeight/2 > height/2)
        position.y = height / 2 - paddleHeight / 2;
    else if (position.y - paddleHeight/2 < -height/2)
        position.y = -height / 2 + paddleHeight / 2;
    if (position.z + paddleLength/2 > length/2)
        position.z = length /2 - paddleLength /2;
    else if (position.z - paddleLength/2 < -length/2)
        position.z = -length /2 + paddleLength / 2;

    paddle->setPosition(position.x, position.y, position.z);
    return position;
 }

/**
 * the main key handler for the game
 * this function is called in keyPressed in Assignment1.cpp
 * as of now, it's functionality only include moving the paddle with WASD keys
 */
bool Room::keyPressed(const OIS::KeyEvent &arg, int type)
{
    if (!paddle || !paddle2) {
        return false;
    }
    Paddle* p = NULL;
    if ( type ) //the game is currently in client mode
    {
      p = paddle2;
    }
    else //not in client mode: either server or single player
    {
      p = paddle;
    }


    switch (arg.key) {
        case OIS::KC_W:
            p->modifyDirection(0, 1);
            break;
        case OIS::KC_A:
            p->modifyDirection(-1, 0);
            break;
        case OIS::KC_S:
            p->modifyDirection(0, -1);
            break;
        case OIS::KC_D:
            p->modifyDirection(1, 0);
            break;
        default:
            break;
    }
    return true;
}

/**
 * the main key handler for the game
 * this function is called in keyReleased in Assignment1.cpp
 * as of now, it's functionality only include moving the paddle with WASD keys
 */
bool Room::keyReleased(const OIS::KeyEvent &arg)
{
    if (paddle == NULL) {
        return false;
    }

    switch (arg.key) {
        case OIS::KC_W:
            paddle->modifyDirection(0, -1);
            break;
        case OIS::KC_A:
            paddle->modifyDirection(1, 0);
            break;
        case OIS::KC_S:
            paddle->modifyDirection(0, 1);
            break;
        case OIS::KC_D:
            paddle->modifyDirection(-1, 0);
            break;
        default:
            break;
    }
    return true;
}

Ogre::Real Room::getWidth()
{
    return width;
}

Ogre::Real Room::getLength()
{
    return length;
}

Ogre::Real Room::getHeight()
{
    return height;
}

int Room::getScore()
{
    return score;
}
