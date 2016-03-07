
#include "OSGCommon.h"
#include <iostream>
#include <osg/GL>
//#include <osg/GLExtensions>
//#include <osg/GL2Extensions>
//#include <osg/GLU>
#include <osg/FrameBufferObject>
//#include <osgViewer/ViewerEventHandlers>
#include "WindowCaptureCallback.h"
#include "test.h"
#include "MyCameraPostDrawCallback.h"

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

void render_image(osg::Image* image)
{
	auto fbo = osg::FBOExtensions::instance(0, true);
	if (!fbo->isSupported())
		return;
	GLuint framebuffer;
	GLuint depthRenderbuffer;
	GLuint texture;
	GLint texWidth = 256, texHeight = 256;
	GLint maxRenderbufferSize;
	glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE_EXT, &maxRenderbufferSize);
	// check if GL_MAX_RENDERBUFFER_SIZE is >= texWidth and texHeight
	if ((maxRenderbufferSize <= texWidth) ||
		(maxRenderbufferSize <= texHeight))
	{// cannot use framebuffer objects as we need to create
	 // a depth buffer as a render buffer object
	 // return with appropriate error
	}
	// generate the framebuffer, render buffer, and texture object names
	fbo->glGenFramebuffers(1, &framebuffer);
	fbo->glGenRenderbuffers(1, &depthRenderbuffer);
	glGenTextures(1, &texture);
	// bind texture and load the texture mip-level 0
	// texels are RGB565
	// no texels need to be specified as we are going to draw into
	// the texture
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight,
		0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// bind render buffer and create a 16-bit depth buffer
	// width and height of render buffer = width and height of
	// the texture
	fbo->glBindRenderbuffer(GL_RENDERBUFFER_EXT, depthRenderbuffer);
	fbo->glRenderbufferStorage(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT16,
		texWidth, texHeight);
	// bind the framebuffer
	fbo->glBindFramebuffer(GL_FRAMEBUFFER_EXT, framebuffer);
	// specify texture as color attachment
	fbo->glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
		GL_TEXTURE_2D, texture, 0);
	// specify depth_renderbufer as depth attachment
	fbo->glFramebufferRenderbuffer(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
		GL_RENDERBUFFER_EXT, depthRenderbuffer);
	// check for framebuffer complete
	auto status = fbo->glCheckFramebufferStatus(GL_FRAMEBUFFER_EXT);
	if (status == GL_FRAMEBUFFER_COMPLETE_EXT)
	{
		// render to texture using FBO
		// clear color and depth buffer
		//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// load uniforms for vertex and fragment shader
		// used to render to FBO. The vertex shader is the
		// ES 1.1 vertex shader described as Example 8-8 in
		// Chapter 8. The fragment shader outputs the color
		// computed by vertex shader as fragment color and
		// is described as Example 1-2 in Chapter 1.
		//set_fbo_texture_shader_and_uniforms();
		// drawing commands to the framebuffer object
		//draw_teapot();
		// render to window system provided framebuffer
		image->readImageFromCurrentTexture(texture, true);
		fbo->glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
		// Use texture to draw to window system provided framebuffer
		// We draw a quad that is the size of the view port.
		//
		// The vertex shader outputs the vertex position and texture
		// coordinates passed as inputs.
		//
		// The fragment shader uses the texture coordinate to sample
		// the texture and uses this as the per-fragment color value.
		//set_screen_shader_and_uniforms();
		//draw_screen_quad();
		//image->readImageFromCurrentTexture(texture, true);
	}
	// cleanup
	fbo->glDeleteRenderbuffers(1, &depthRenderbuffer);
	fbo->glDeleteFramebuffers(1, &framebuffer);
	glDeleteTextures(1, &texture);
}
void anotherGL()
{
//	GLenum readType, readFormat;
//	GLubyte *pixels;
//	glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_TYPE, &readType);
//	glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_FORMAT, &readFormat);
//	unsigned int bytesPerPixel = 0;
//	switch (readType)
//	{
//	case GL_UNSIGNED_BYTE:
//		switch (readFormat)
//		{
//		case GL_RGBA:
//			bytesPerPixel = 4;
//			break;
//		case GL_RGB:
//			bytesPerPixel = 3;
//			break;
//		case GL_LUMINANCE_ALPHA:
//			bytesPerPixel = 2;
//			break;
//		case GL_ALPHA:
//		case GL_LUMINANCE:
//			bytesPerPixel = 1;
//			break;
//}
//break;
//case GL_UNSIGNED_SHORT_4444: // GL_RGBA format
//case GL_UNSIGNED_SHORT_555_1: // GL_RGBA format
//case GL_UNSIGNED_SHORT_565: // GL_RGB format
//	bytesPerPixel = 2;
//	break;
//}
//pixels = (GLubyte*)malloc(width * height * bytesPerPixel);
//glReadPixels(0, 0, windowWidth, windowHeight, readFormat,
//	readType, pixels);

}

void getImageByGLCalls(osg::RenderInfo &renderInfo, GLubyte **data)//
{
	//auto fbo = osg::FBOExtensions::instance(0, true);
	//if (!fbo->isSupported())
	//	return false;
	int width;
	int height;

	osg::Camera * camera = renderInfo.getCurrentCamera();

	width = camera->getViewport()->width();
	height = camera->getViewport()->height();

	// Bind the color render buffer used to render the OpenGL ES view

	// If your application only creates a single color render buffer which is already bound at this point,

	// this call is redundant, but it is needed if you're dealing with multiple render buffers.

	// Note, replace "viewRenderbuffer" with the actual name of the render buffer object defined in your class.
	//fbo->glBindRenderbuffer(GL_RENDERBUFFER_EXT, viewRenderbuffer);

	// Get the size of the backing CAEAGLLayer

	//fbo->glGetRenderbufferParameteriv(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_WIDTH_EXT, &backingWidth);

	//fbo->glGetRenderbufferParameteriv(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_HEIGHT_EXT, &backingHeight);

	int x = 0, y = 0;

	int dataLength = width * height * 4;

	*data = new GLubyte[dataLength * sizeof(GLubyte)];

	// Read pixel data from the frame buffer

	//glPixelStorei(GL_PACK_ALIGNMENT, 4);

	auto fbo = osg::FBOExtensions::instance(0, true);
	if (!fbo->isSupported())
		return;
	//Somewhere at initialization
	GLuint buffer_id, render_buf;
	fbo->glGenFramebuffers(1, &buffer_id);
	fbo->glGenRenderbuffers(1, &render_buf);
	fbo->glBindRenderbuffer(GL_FRAMEBUFFER_EXT, render_buf);
	fbo->glRenderbufferStorage(GL_RENDERBUFFER_EXT, GL_BGRA, width, height);
	fbo->glBindFramebuffer(GL_FRAMEBUFFER_EXT, buffer_id);
	fbo->glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, render_buf);

	//At ed init:
	fbo->glDeleteFramebuffers(1, &buffer_id);
	fbo->glDeleteRenderbuffers(1, &render_buf);

	//Before drawing
	fbo->glBindFramebuffer(GL_FRAMEBUFFER_EXT, buffer_id);
	//after drawing
	//std::vector<unsigned int> data(width*height * 4);
	glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glReadPixels(x, y, width, height, GL_BGRA, GL_UNSIGNED_BYTE, *data);
	// Return to on screen rendering:
	fbo->glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);

	//glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, *data);

}

class SnapshotCallback : public osg::Camera::DrawCallback
{
public:
	std::string file_name;
	mutable unsigned char* m_data;
	//osg::ref_ptr<osg::Texture2D> m_texture;

	SnapshotCallback() :
		file_name("./saved_image.bmp")/*,*/
		//m_texture(texture)
	{
		m_data = nullptr;
	}

	~SnapshotCallback()
	{
		delete[] m_data;
	}

	unsigned char *getImageData()
	{
		return m_data;
	}
	bool glGenTextureFromFramebuffer(GLuint *t, GLsizei w, GLsizei h)
	{
		auto fbo = osg::FBOExtensions::instance(0, true);
		if (!fbo->isSupported())
			return false;

		//fbo->glGenFramebuffers(1, 0);
		glGenTextures(1, t);

		//fbo->glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);

		glBindTexture(GL_TEXTURE_2D, *t);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		fbo->glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, *t, 0);

		GLuint depthbuffer;
		fbo->glGenRenderbuffers(1, &depthbuffer);
		fbo->glBindRenderbuffer(GL_RENDERBUFFER_EXT, depthbuffer);
		fbo->glRenderbufferStorage(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT16, w, h);
		fbo->glFramebufferRenderbuffer(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthbuffer);

		GLenum status = fbo->glCheckFramebufferStatus(GL_FRAMEBUFFER_EXT);
		if (status != GL_FRAMEBUFFER_COMPLETE_EXT)
			osg::notify(osg::NOTICE) << "Frame buffer status: " << (int)status;
	}

	void	operator() (osg::RenderInfo &renderInfo)const
	{
		//getImageByGLCalls(renderInfo, &m_data);
		//
		//return;
		auto fbo = osg::FBOExtensions::instance(renderInfo.getContextID(), true);
		if (!fbo->isSupported())
			return;
		fbo->glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
		int width;
		int height;

		osg::Camera * camera = renderInfo.getCurrentCamera();

		width = camera->getViewport()->width();
		height = camera->getViewport()->height();
		std::cerr << "W : " << width << "H : " << height;
		auto m_image = new osg::Image();
		m_image->readPixels(0, 0, 80, 80, GL_RGB, GL_UNSIGNED_BYTE);
		//render_image(m_image);
		osg::notify(osg::NOTICE) << "Read pixels done." << std::endl;

		auto data = static_cast<const unsigned char*>(m_image->getDataPointer());
		auto size = m_image->getTotalDataSize();
		osg::notify(osg::NOTICE) << "image data:" << std::endl;
		m_data = new unsigned char[size];

		for (unsigned int i = 0; i < size; i++)
			m_data[i] = data[i];

		osg::notify(osg::NOTICE) << std::endl << "image data ended." << std::endl;

		//m_texture = new osg::Texture2D();
		//m_texture->setTextureSize(width, height);
		//m_texture->setInternalFormat(GL_RGBA);
		//camera->attach(osg::Camera::COLOR_BUFFER, m_texture);
		//camera->getStateSet()->setTextureAttributeAndModes(1, m_texture);

		//m_image->readImageFromCurrentTexture(1, false);
		if (osgDB::writeImageFile(*m_image, file_name))
		{
			osg::notify(osg::NOTICE) << "Saved screen image to '" << file_name << "'" << std::endl;
		}
		else
		{
			osg::notify(osg::FATAL) << "Could not save image!" << std::endl;
		}
		//osg::Camera * camera = renderInfo.getCurrentCamera();

		//auto numFrames = 1;
		//osg::ref_ptr<WindowCaptureCallback> _callback = new WindowCaptureCallback(numFrames,
		//	WindowCaptureCallback::READ_PIXELS,
		//	//                                          WindowCaptureCallback::SINGLE_PBO,
		//	//                                          WindowCaptureCallback::DOUBLE_PBO,
		//	//                                          WindowCaptureCallback::TRIPLE_PBO,
		//	WindowCaptureCallback::END_FRAME, GL_BACK);
		//camera->setFinalDrawCallback(_callback.get());
		//width = camera->getViewport()->width();
		//height = camera->getViewport()->height();
		//std::cerr << "W : " << width << "H : " << height << std::endl;
		//osg::ref_ptr< ::osg::Image> image = new ::osg::Image();
		//image->readPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE);
		////osg::notify(osg::NOTICE) << "Read pixels done." << std::endl;
		////camera->attach(osg::Camera::COLOR_BUFFER, m_texture);

		////image->readImageFromCurrentTexture(0, false, GL_RGBA);

		//if (osgDB::writeImageFile(*image, file_name))
		//{
		//	osg::notify(osg::NOTICE) << "Saved screen image to '" << file_name << "'" << std::endl;
		//	//std::cout << "Saved screen image to '" << file_name << "'" << std::endl;
		//}
		//else
		//{
		//	osg::notify(osg::FATAL) << "Could not save image!" << std::endl;
		//	//std::cerr << "Could not save image!";
		//}
	}
};

class SaveCallback : public osg::Camera::DrawCallback
{
public:
	std::string file_name;
	mutable unsigned char* m_data;
	osg::Image* m_image;
	//osg::ref_ptr<osg::Texture2D> m_texture;

	SaveCallback(osg::Image* image) :
		file_name("./saved_image.bmp"),
		m_image(image)
									  //m_texture(texture)
	{
		m_data = nullptr;
	}

	~SaveCallback()
	{
	}

	void	operator() (osg::RenderInfo &renderInfo)const
	{
		if (osgDB::writeImageFile(*m_image, file_name))
		{
			osg::notify(osg::NOTICE) << "Saved screen image to '" << file_name << "'" << std::endl;
		}
		else
		{
			osg::notify(osg::FATAL) << "Could not save image!" << std::endl;
		}
	}
};

void setCameraRenderImage(osgViewer::Viewer * viewer, const std::string& file_name)
{
	auto camera = viewer->getCamera();
	// origin_viewPort	origin_renderOrder	origin_renderImp

	auto tex_width = 256;
	auto tex_height = 256;
	unsigned int samples = 0;
	unsigned int colorSamples = 0;
	auto renderImplementation = osg::Camera::FRAME_BUFFER;

	// restore view port param.
	auto origin_viewPort = camera->getViewport();
	auto ori_w = origin_viewPort->width();
	auto ori_h = origin_viewPort->height();
	auto ori_x = origin_viewPort->x();
	auto ori_y = origin_viewPort->y();
	// set view port
	camera->setViewport(0, 0, tex_width, tex_height);

	auto origin_renderOrder = camera->getRenderOrder();
	// set the camera to render before the main camera.
	camera->setRenderOrder(osg::Camera::PRE_RENDER);

	auto origin_renderImp = camera->getRenderTargetImplementation();
	// tell the camera to use OpenGL frame buffer object where supported.
	camera->setRenderTargetImplementation(renderImplementation);

	osg::Image* image = new osg::Image;
	image->allocateImage(tex_width, tex_height, 1, GL_RGBA, GL_UNSIGNED_BYTE);
	//image->allocateImage(tex_width, tex_height, 1, GL_RGBA, GL_FLOAT);

	// attach the image so its copied on each frame.
	camera->attach(osg::Camera::COLOR_BUFFER, image,
		samples, colorSamples);

	//camera->setPostDrawCallback(new MyCameraPostDrawCallback(image, file_name));

	// Rather than attach the texture directly to illustrate the texture's ability to
	// detect an image update and to subload the image onto the texture.  You needn't
	// do this when using an Image for copying to, as a separate camera->attach(..)
	// would suffice as well, but we'll do it the long way round here just for demonstration
	// purposes (long way round meaning we'll need to copy image to main memory, then
	// copy it back to the graphics card to the texture in one frame).
	// The long way round allows us to manually modify the copied image via the callback
	// and then let this modified image by reloaded back.
	//texture->setImage(0, image);

	//osgDB::writeImageFile(*image, "saved_image1.bmp");
	osg::ref_ptr<SaveCallback> callback = new SaveCallback(image);
	viewer->getCamera()->setFinalDrawCallback(callback);//m_texture
	viewer->renderingTraversals();
	camera->setFinalDrawCallback(NULL);

	// recover camera
	camera->setViewport(ori_x, ori_y, ori_w, ori_h);
	camera->setRenderOrder(origin_renderOrder);
	camera->setRenderTargetImplementation(origin_renderImp);
	camera->detach(osg::Camera::COLOR_BUFFER);
}

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
				osg::Camera * camera = viewer->getCamera();
				setCameraRenderImage(viewer, "./saved_image.bmp");
				return true;
				//
				//auto file_name = "saved_image.bmp";
				//auto width = camera->getViewport()->width();
				//auto height = camera->getViewport()->height();
				//std::cerr << "W : " << width << "H : " << height;
				//auto m_image = new osg::Image();
				////m_image->readPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE);
				//render_image(m_image);
				//if (osgDB::writeImageFile(*m_image, file_name))
				//{
				//	osg::notify(osg::NOTICE) << "Saved screen image to '" << file_name << "'" << std::endl;
				//}
				//else
				//{
				//	osg::notify(osg::FATAL) << "Could not save image!" << std::endl;
				//}

				////auto numFrames = 1;
				//auto file_name = "saved_image";
				//auto extension = "bmp";
				//osg::ref_ptr<WindowCaptureCallback> _callback = new WindowCaptureCallback(
				//	file_name,
				//	extension,
				//	WindowCaptureCallback::READ_PIXELS,
				//	//                                          WindowCaptureCallback::SINGLE_PBO,
				//	//                                          WindowCaptureCallback::DOUBLE_PBO,
				//	//                                          WindowCaptureCallback::TRIPLE_PBO,
				//	WindowCaptureCallback::END_FRAME, GL_BACK);
				//camera->setFinalDrawCallback(_callback.get());
				osg::ref_ptr<SnapshotCallback> callback = new SnapshotCallback();
				viewer->getCamera()->setFinalDrawCallback(callback);//m_texture
				viewer->renderingTraversals();
				camera->setFinalDrawCallback(NULL);
				auto data = callback->getImageData();
				return true;
			}
		}
		default:
			return false;
		}
	}
};

void main()
{
	osgViewer::Viewer viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group();
	root->addChild(createQuad().get());
	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D();
	viewer.setSceneData(root);
	viewer.addEventHandler(new CaptureHandler());
	viewer.realize();
	viewer.run();

	//auto port = viewer.getCamera()->getViewport();
	//auto width = port->width();
	//auto height = port->height();
	//texture->setTextureSize(width, height);
	//texture->setInternalFormat(GL_RGBA);
	//root->getStateSet()->setTextureAttributeAndModes(0, texture);
}