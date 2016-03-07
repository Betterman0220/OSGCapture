#pragma once
#include "OSGCommon.h"

/** Event handler that will capture the screen on key press. */
class ScreenCaptureHandler : public osgGA::GUIEventHandler
{
public:

	/** Abstract base class for what to do when a screen capture happens. */
	class CaptureOperation : public osg::Referenced
	{
	public:
		virtual void operator()(const osg::Image& image, const unsigned int context_id) = 0;
	};

	/** Concrete implementation of a CaptureOperation that writes the screen capture to a file. */
	class WriteToFile : public CaptureOperation
	{
	public:
		enum SavePolicy
		{
			OVERWRITE,
			SEQUENTIAL_NUMBER
			// ... any others?
		};

		WriteToFile(const std::string& filename, const std::string& extension, SavePolicy savePolicy = SEQUENTIAL_NUMBER);

		virtual void operator()(const osg::Image& image, const unsigned int context_id);

		void setSavePolicy(SavePolicy savePolicy) { _savePolicy = savePolicy; }
		SavePolicy getSavePolicy() const { return _savePolicy; }

	protected:

		WriteToFile& operator = (const WriteToFile&) { return *this; }

		const std::string _filename;
		const std::string _extension;

		SavePolicy _savePolicy;

		std::vector<unsigned int> _contextSaveCounter;
	};

	/** @param numFrames >0: capture that number of frames. <0: capture all frames, call stopCapture() to stop it. */
	ScreenCaptureHandler(CaptureOperation* defaultOperation = 0, int numFrames = 1);

	void setKeyEventTakeScreenShot(int key) { _keyEventTakeScreenShot = key; }
	int getKeyEventTakeScreenShot() const { return _keyEventTakeScreenShot; }

	void setKeyEventToggleContinuousCapture(int key) { _keyEventToggleContinuousCapture = key; }
	int getKeyEventToggleContinuousCapture() const { return _keyEventToggleContinuousCapture; }

	void setCaptureOperation(CaptureOperation* operation);
	CaptureOperation* getCaptureOperation() const;

	// aa will point to an osgViewer::View, so we will take a screenshot
	// of that view's graphics contexts.
	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	/** Capture the given viewer's views on the next frame. */
	virtual void captureNextFrame(osgViewer::ViewerBase& viewer);

	/** Set the number of frames to capture.
	@param numFrames >0: capture that number of frames. <0: capture all frames, call stopCapture() to stop it. */
	void setFramesToCapture(int numFrames);

	/** Get the number of frames to capture. */
	int getFramesToCapture() const;

	/** Start capturing any viewer(s) the handler is attached to at the
	end of the next frame. */
	void startCapture();

	/** Stop capturing. */
	void stopCapture();

	/** Get the keyboard and mouse usage of this manipulator.*/
	virtual void getUsage(osg::ApplicationUsage& usage) const;

protected:
	bool _startCapture;
	bool _stopCapture;

	int _keyEventTakeScreenShot;
	int _keyEventToggleContinuousCapture;
	// there could be a key to start taking screen shots every new frame

	osg::ref_ptr<CaptureOperation>          _operation;
	osg::ref_ptr<osg::Camera::DrawCallback> _callback;

	void addCallbackToViewer(osgViewer::ViewerBase& viewer);
	void removeCallbackFromViewer(osgViewer::ViewerBase& viewer);
	osg::Camera* findAppropriateCameraForCallback(osgViewer::ViewerBase& viewer);
};
