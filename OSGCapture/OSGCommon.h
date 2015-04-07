#pragma once

#ifdef _DEBUG
#pragma comment(lib, "osgViewerd.lib")
#pragma comment(lib, "osgDBd.lib")
#pragma comment(lib, "OpenThreadsd.lib")
#pragma comment(lib, "osgd.lib")
#pragma comment(lib, "osgGAd.lib")
#pragma comment(lib, "osgFXd.lib")
#pragma comment(lib, "osgUtild.lib")
#else
#pragma comment(lib, "osgViewer.lib")
#pragma comment(lib, "osgDB.lib")
#pragma comment(lib, "OpenThreads.lib")
#pragma comment(lib, "osg.lib")
#pragma comment(lib, "osgGA.lib")
#pragma comment(lib, "osgFX.lib")
#pragma comment(lib, "osgUtil.lib")
#endif
#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osg/Node>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/ShapeDrawable>
#include <osgGA/GUIEventHandler>
#include <osgUtil/LineSegmentIntersector>
#include <osgFX/Scribe>
#include <osg/Camera>
#include <osg/GraphicsContext>
#include <osg/MatrixTransform>