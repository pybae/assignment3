ACLOCAL_AMFLAGS= -I m4
noinst_HEADERS= NetManager.h BaseApplication.h TutorialApplication.h Ball.h Room.h Paddle.h

bin_PROGRAMS= OgreApp
OgreApp_CPPFLAGS= -std=c++11 -I$(top_srcdir)
OgreApp_SOURCES= NetManager.cpp BaseApplication.cpp TutorialApplication.cpp Ball.cpp Room.cpp Paddle.cpp
OgreApp_CXXFLAGS= $(OGRE_CFLAGS) $(OIS_CFLAGS) $(bullet_CFLAGS) $(CEGUI_CFLAGS)
OgreApp_LDADD= $(OGRE_LIBS) $(OIS_LIBS) $(bullet_LIBS) $(CEGUI_LIBS) $(CEGUI_OGRE_LIBS)
OgreApp_LDFLAGS= -lOgreOverlay -lboost_system -lSDL -lSDL_mixer -lSDL_net

EXTRA_DIST= buildit makeit
AUTOMAKE_OPTIONS= foreign
