#pragma once

#ifndef SCREENCAPTURE_H
#define SCREENCAPTURE_H

#include <string>

#include "OSGCommon.h"
#include <osgDB/WriteFile>
#include <osgViewer/ViewerEventHandlers>

class CScreenCapture : public osgViewer::ScreenCaptureHandler
{
public:
	class  WriteToImageFile : public osgViewer::ScreenCaptureHandler::CaptureOperation
	{
	public:
		enum class SavePolicy
		{
			OVERWRITE,
			SEQUENTIAL_NUMBER
			// ... any others?
		};

		WriteToImageFile(const std::string& filename, const std::string& extension, SavePolicy savePolicy = SavePolicy::SEQUENTIAL_NUMBER);

		virtual void operator()(const osg::Image& image, const unsigned int context_id);

		void setFileName(const std::string &filename) { _filename = filename; }

		void setSavePolicy(SavePolicy savePolicy) { _savePolicy = savePolicy; }
		SavePolicy getSavePolicy() const { return _savePolicy; }

	protected:

		WriteToImageFile& operator = (const WriteToImageFile&) { return *this; }

		std::string _filename;
		std::string _extension;

		SavePolicy _savePolicy;

		std::vector<unsigned int> _contextSaveCounter;
	};

	CScreenCapture(CaptureOperation* defaultOperation = 0, int numFrames = 1);
	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
	void setFileName(const std::string& filename);
};

#endif // SCREENCAPTURE_H