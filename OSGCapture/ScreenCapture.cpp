/****************************************************************************
*
*
*
****************************************************************************/

#include "ScreenCapture.h"



CScreenCapture::CScreenCapture(CaptureOperation* defaultOperation /*= 0*/, int numFrames/* = 1*/) :
ScreenCaptureHandler(defaultOperation, numFrames)
{

}

bool CScreenCapture::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
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

	//case (osgGA::GUIEventAdapter::USER) ://自定义的事件，需要进入OSG的事件队列
	//{
	//	const OwnDefineEventAdpater* adpater = dynamic_cast<const MyEventAdpater *>(ea.getUserData());
	//	switch (adpater->m_eventType)
	//	{
	//	case SAVE_IMAGE:// 保存图片的事件
	//	{
	//		if (getFramesToCapture() == 0)
	//		{
	//			setFramesToCapture(1);
	//		}
	//		addCallbackToViewer(*viewer);
	//		return true;
	//	}
	//	default:
	//		break;
	//	}
	//	//if (adpater->m_eventType == SaveImage )
	//	//{
	//	//	// Check that we will capture at least one frame.
	//	//	// Just check for ==0, because >0 is means we're already
	//	//	// capturing and <0 means it will capture all frames.
	//	//	if (getFramesToCapture() == 0)
	//	//	{
	//	//		setFramesToCapture(1);
	//	//	}
	//	//	addCallbackToViewer(*viewer);
	//	//	return true;
	//	//}
	//}
	default:
		break;
	}
	return false;
}

void CScreenCapture::setFileName(const std::string& filename)
{
	CScreenCapture::WriteToImageFile* wtf = dynamic_cast<CScreenCapture::WriteToImageFile*>(getCaptureOperation());
	wtf->setFileName(filename);
}

// 仿函数实现图片保存
void CScreenCapture::WriteToImageFile::operator()(const osg::Image& image, const unsigned int context_id)
{
	if (!_filename.empty())
	{
		osgDB::writeImageFile(image, _filename);
	}
}

CScreenCapture::WriteToImageFile::WriteToImageFile(const std::string& filename, const std::string& extension, SavePolicy savePolicy /*= SEQUENTIAL_NUMBER*/)
	: _filename(filename),
	_extension(extension),
	_savePolicy(savePolicy)
{

}
