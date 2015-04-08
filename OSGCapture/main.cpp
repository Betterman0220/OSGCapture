
#include "OSGCommon.h"
#include <iostream>

osg::ref_ptr<osg::Node> createQuad()
{
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;

	osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array;
	geom->setVertexArray(v.get());
	v->push_back(osg::Vec3(-1.f, 0.f, -1.f));
	v->push_back(osg::Vec3(1.f, 0.f, -1.f));
	v->push_back(osg::Vec3(1.f, 0.f, 1.f));
	v->push_back(osg::Vec3(-1.f, 0.f, 1.f));

	osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array;
	geom->setColorArray(c.get());
	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	c->push_back(osg::Vec4(1.f, 0.f, 0.f, 1.f));
	c->push_back(osg::Vec4(0.f, 1.f, 0.f, 1.f));
	c->push_back(osg::Vec4(0.f, 0.f, 1.f, 1.f));
	c->push_back(osg::Vec4(1.f, 1.f, 1.f, 1.f));

	osg::ref_ptr<osg::Vec3Array> n = new osg::Vec3Array;
	geom->setNormalArray(n.get());
	geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
	n->push_back(osg::Vec3(0.f, -1.f, 0.f));

	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(geom.get());
	return geode.get();
}

class SnapshotCallback : public osg::Camera::DrawCallback
{
public:
	std::string file_name;

	SnapshotCallback() :
		file_name("./saved_image.bmp")
	{

	}

	void	operator() (osg::RenderInfo &renderInfo) const
	{
		int width;
		int height;

		osg::Camera * camera = renderInfo.getCurrentCamera();

		width = camera->getViewport()->width();
		height = camera->getViewport()->height();
		std::cerr << "W : " << width << "H : " << height;
		osg::ref_ptr< ::osg::Image> image = new ::osg::Image();
		image->readPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE);
		if (osgDB::writeImageFile(*image, file_name))
		{
			std::cout << "Saved screen image to '" << file_name << "'" << std::endl;
		}
		else
		{
			std::cerr << "Could not save image!";
		}
	}
};

class CaptureHandler :public osgGA::GUIEventHandler
{
public:
	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
	{
		switch (ea.getEventType())
		{
		case osgGA::GUIEventAdapter::KEYDOWN:
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_S)
			{
				osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
				viewer->getCamera()->setFinalDrawCallback(new SnapshotCallback());
				viewer->renderingTraversals();
				viewer->getCamera()->setFinalDrawCallback(NULL);
				return true;
			}
		}
		}
	}
};

void main()
{
	osgViewer::Viewer viewer;
	osg::Group* root = new osg::Group();
	root->addChild(createQuad().get());
	viewer.setSceneData(root);
	viewer.addEventHandler(new CaptureHandler());
	viewer.realize();
	viewer.run();
}