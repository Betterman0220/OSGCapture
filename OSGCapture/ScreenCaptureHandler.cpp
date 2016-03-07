#include "ScreenCaptureHandler.h"

#include "WindowCaptureCallback.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ScreenCaptureHandler::WriteToFile
//
ScreenCaptureHandler::WriteToFile::WriteToFile(const std::string& filename,
	const std::string& extension,
	SavePolicy savePolicy)
	: _filename(filename), _extension(extension), _savePolicy(savePolicy)
{
}

void ScreenCaptureHandler::WriteToFile::operator () (const osg::Image& image, const unsigned int context_id)
{
	if (_savePolicy == SEQUENTIAL_NUMBER)
	{
		if (_contextSaveCounter.size() <= context_id)
		{
			unsigned int oldSize = _contextSaveCounter.size();
			_contextSaveCounter.resize(context_id + 1);
			// Initialize all new values to 0 since context ids may not be consecutive.
			for (unsigned int i = oldSize; i <= context_id; i++)
				_contextSaveCounter[i] = 0;
		}
	}

	std::stringstream filename;
	filename << _filename << "_" << context_id;

	if (_savePolicy == SEQUENTIAL_NUMBER)
		filename << "_" << _contextSaveCounter[context_id];

	filename << "." << _extension;

	osgDB::writeImageFile(image, filename.str());

	OSG_INFO << "ScreenCaptureHandler: Taking a screenshot, saved as '" << filename.str() << "'" << std::endl;

	if (_savePolicy == SEQUENTIAL_NUMBER)
	{
		_contextSaveCounter[context_id]++;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ScreenCaptureHandler
//
ScreenCaptureHandler::ScreenCaptureHandler(CaptureOperation* defaultOperation,
	int numFrames)
	: _startCapture(false),
	_stopCapture(false),
	_keyEventTakeScreenShot('c'),
	_keyEventToggleContinuousCapture('C'),
	_callback(new WindowCaptureCallback(numFrames,
	WindowCaptureCallback::READ_PIXELS,
	//                                          WindowCaptureCallback::SINGLE_PBO,
	//                                          WindowCaptureCallback::DOUBLE_PBO,
	//                                          WindowCaptureCallback::TRIPLE_PBO,
	WindowCaptureCallback::END_FRAME, GL_BACK))
{
	if (defaultOperation)
		setCaptureOperation(defaultOperation);
	else
		setCaptureOperation(new WriteToFile("screen_shot", "jpg"));
}

void ScreenCaptureHandler::setCaptureOperation(CaptureOperation* operation)
{
	WindowCaptureCallback* callback = static_cast<WindowCaptureCallback*>(_callback.get());
	callback->setCaptureOperation(operation);
}

ScreenCaptureHandler::CaptureOperation* ScreenCaptureHandler::getCaptureOperation() const
{
	WindowCaptureCallback* callback = static_cast<WindowCaptureCallback*>(_callback.get());
	return callback->getCaptureOperation();
}

void ScreenCaptureHandler::addCallbackToViewer(osgViewer::ViewerBase& viewer)
{
	osg::Camera* camera = findAppropriateCameraForCallback(viewer);
	if (!camera) return;

	WindowCaptureCallback* callback = static_cast<WindowCaptureCallback*>(_callback.get());
	if (callback && callback->getFramePosition() == WindowCaptureCallback::START_FRAME)
	{
		camera->setInitialDrawCallback(_callback.get());
	}
	else
	{
		camera->setFinalDrawCallback(_callback.get());
	}
}

void ScreenCaptureHandler::removeCallbackFromViewer(osgViewer::ViewerBase& viewer)
{
	osg::Camera* camera = findAppropriateCameraForCallback(viewer);
	if (!camera) return;

	WindowCaptureCallback* callback = static_cast<WindowCaptureCallback*>(_callback.get());
	if (callback && callback->getFramePosition() == WindowCaptureCallback::START_FRAME)
	{
		camera->setInitialDrawCallback(0);
	}
	else
	{
		camera->setFinalDrawCallback(0);
	}
}

osg::Camera* ScreenCaptureHandler::findAppropriateCameraForCallback(osgViewer::ViewerBase& viewer)
{
	// Select either the first or the last active camera, depending on the
	// frame position set in the callback.
	// One case where testing the node mask is important is when the stats
	// handler has been initialized, but stats are not displayed. In that
	// case, there is a post render camera on the viewer, but its node mask
	// is zero, so the callback added to that camera would never be called.
	WindowCaptureCallback* callback = static_cast<WindowCaptureCallback*>(_callback.get());

	if (callback->getFramePosition() == WindowCaptureCallback::START_FRAME)
	{
		osgViewer::ViewerBase::Contexts contexts;
		viewer.getContexts(contexts);
		for (osgViewer::ViewerBase::Contexts::iterator itr = contexts.begin();
		itr != contexts.end();
			++itr)
		{
			osg::GraphicsContext* context = *itr;
			osg::GraphicsContext::Cameras& cameras = context->getCameras();
			osg::Camera* firstCamera = 0;
			for (osg::GraphicsContext::Cameras::iterator cam_itr = cameras.begin();
			cam_itr != cameras.end();
				++cam_itr)
			{
				if (firstCamera)
				{
					if ((*cam_itr)->getRenderOrder() < firstCamera->getRenderOrder())
					{
						if ((*cam_itr)->getNodeMask() != 0x0)
							firstCamera = (*cam_itr);
					}
					if ((*cam_itr)->getRenderOrder() == firstCamera->getRenderOrder() &&
						(*cam_itr)->getRenderOrderNum() < firstCamera->getRenderOrderNum())
					{
						if ((*cam_itr)->getNodeMask() != 0x0)
							firstCamera = (*cam_itr);
					}
				}
				else
				{
					if ((*cam_itr)->getNodeMask() != 0x0)
						firstCamera = *cam_itr;
				}
			}

			if (firstCamera)
			{
				//OSG_NOTICE<<"ScreenCaptureHandler: First camera "<<firstCamera<<std::endl;

				return firstCamera;
			}
			else
			{
				OSG_NOTICE << "ScreenCaptureHandler: No camera found" << std::endl;
			}
		}
	}
	else
	{
		osgViewer::ViewerBase::Contexts contexts;
		viewer.getContexts(contexts);
		for (osgViewer::ViewerBase::Contexts::iterator itr = contexts.begin();
		itr != contexts.end();
			++itr)
		{
			osg::GraphicsContext* context = *itr;
			osg::GraphicsContext::Cameras& cameras = context->getCameras();
			osg::Camera* lastCamera = 0;
			for (osg::GraphicsContext::Cameras::iterator cam_itr = cameras.begin();
			cam_itr != cameras.end();
				++cam_itr)
			{
				if (lastCamera)
				{
					if ((*cam_itr)->getRenderOrder() > lastCamera->getRenderOrder())
					{
						if ((*cam_itr)->getNodeMask() != 0x0)
							lastCamera = (*cam_itr);
					}
					if ((*cam_itr)->getRenderOrder() == lastCamera->getRenderOrder() &&
						(*cam_itr)->getRenderOrderNum() >= lastCamera->getRenderOrderNum())
					{
						if ((*cam_itr)->getNodeMask() != 0x0)
							lastCamera = (*cam_itr);
					}
				}
				else
				{
					if ((*cam_itr)->getNodeMask() != 0x0)
						lastCamera = *cam_itr;
				}
			}

			if (lastCamera)
			{
				//OSG_NOTICE<<"ScreenCaptureHandler: Last camera "<<lastCamera<<std::endl;

				return lastCamera;
			}
			else
			{
				OSG_NOTICE << "ScreenCaptureHandler: No camera found" << std::endl;
			}
		}
	}

	return 0;
}

// aa will point to an osgViewer::View, so we will take a screenshot
// of that view's graphics contexts.
bool ScreenCaptureHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	osgViewer::ViewerBase* viewer = dynamic_cast<osgViewer::View*>(&aa)->getViewerBase();
	if (!viewer) return false;

	switch (ea.getEventType())
	{
	case (osgGA::GUIEventAdapter::FRAME) :
	{
		// Booleans aren't the best way of doing this, but I want to do
		// the actual adding here because I don't want to require
		// startCapture() take a viewer as argument, which could not be
		// the right one.
		if (_startCapture)
		{
			// Start capturing with the currently set number of frames.
			// If set to -1 it will capture continuously, if set to >0
			// it will capture that number of frames.
			_startCapture = false;
			addCallbackToViewer(*viewer);
		}
		else if (_stopCapture)
		{
			_stopCapture = false;
			removeCallbackFromViewer(*viewer);
		}
		break;
	}

	case(osgGA::GUIEventAdapter::KEYUP) :
	{
		if (ea.getKey() == _keyEventTakeScreenShot)
		{
			// Check that we will capture at least one frame.
			// Just check for ==0, because >0 is means we're already
			// capturing and <0 means it will capture all frames.
			WindowCaptureCallback* callback = static_cast<WindowCaptureCallback*>(_callback.get());
			if (callback->getFramesToCapture() == 0)
			{
				setFramesToCapture(1);
			}
			addCallbackToViewer(*viewer);
			return true;
		}

		if (ea.getKey() == _keyEventToggleContinuousCapture)
		{
			if (getFramesToCapture() < 0)
			{
				setFramesToCapture(0);
				removeCallbackFromViewer(*viewer);
			}
			else
			{
				setFramesToCapture(-1);
				addCallbackToViewer(*viewer);
			}
			return true;
		}
		break;
	}
	default:
		break;
	}

	return false;
}

/** Capture the given viewer's views on the next frame. */
void ScreenCaptureHandler::captureNextFrame(osgViewer::ViewerBase& viewer)
{
	addCallbackToViewer(viewer);
}

/** Set the number of frames to capture. */
void ScreenCaptureHandler::setFramesToCapture(int numFrames)
{
	WindowCaptureCallback* callback = static_cast<WindowCaptureCallback*>(_callback.get());
	callback->setFramesToCapture(numFrames);
}

/** Get the number of frames to capture. */
int ScreenCaptureHandler::getFramesToCapture() const
{
	WindowCaptureCallback* callback = static_cast<WindowCaptureCallback*>(_callback.get());
	return callback->getFramesToCapture();
}

/** Start capturing at the end of the next frame. */
void ScreenCaptureHandler::startCapture()
{
	if (getFramesToCapture() != 0)
		_startCapture = true;
}

/** Stop capturing. */
void ScreenCaptureHandler::stopCapture()
{
	_stopCapture = true;
}

/** Get the keyboard and mouse usage of this manipulator.*/
void ScreenCaptureHandler::getUsage(osg::ApplicationUsage& usage) const
{
	usage.addKeyboardMouseBinding(_keyEventTakeScreenShot, "Take screenshot.");
	usage.addKeyboardMouseBinding(_keyEventToggleContinuousCapture, "Toggle continuous screen capture.");
}