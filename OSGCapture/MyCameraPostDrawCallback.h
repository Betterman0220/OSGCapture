#pragma once
#include "OSGCommon.h"

struct MyCameraPostDrawCallback : public osg::Camera::DrawCallback
{
	MyCameraPostDrawCallback(osg::Image* image, const std::string& file_name) :
		_image(image),
		_file(file_name)
	{
	}

	virtual void operator () (const osg::Camera& /*camera*/) const
	{
		osgDB::writeImageFile(*_image, _file);

		//if (_image && _image->getPixelFormat() == GL_RGBA && _image->getDataType() == GL_UNSIGNED_BYTE)
		//{
		//	// we'll pick out the center 1/2 of the whole image,
		//	int column_start = _image->s() / 4;
		//	int column_end = 3 * column_start;

		//	int row_start = _image->t() / 4;
		//	int row_end = 3 * row_start;


		//	// and then invert these pixels
		//	for (int r = row_start; r < row_end; ++r)
		//	{
		//		unsigned char* data = _image->data(column_start, r);
		//		for (int c = column_start; c < column_end; ++c)
		//		{
		//			(*data) = 255 - (*data); ++data;
		//			(*data) = 255 - (*data); ++data;
		//			(*data) = 255 - (*data); ++data;
		//			(*data) = 255; ++data;
		//		}
		//	}


		//	// dirty the image (increments the modified count) so that any textures
		//	// using the image can be informed that they need to update.
		//	_image->dirty();
		//}
		//else if (_image && _image->getPixelFormat() == GL_RGBA && _image->getDataType() == GL_FLOAT)
		//{
		//	// we'll pick out the center 1/2 of the whole image,
		//	int column_start = _image->s() / 4;
		//	int column_end = 3 * column_start;

		//	int row_start = _image->t() / 4;
		//	int row_end = 3 * row_start;

		//	// and then invert these pixels
		//	for (int r = row_start; r < row_end; ++r)
		//	{
		//		float* data = (float*)_image->data(column_start, r);
		//		for (int c = column_start; c < column_end; ++c)
		//		{
		//			(*data) = 1.0f - (*data); ++data;
		//			(*data) = 1.0f - (*data); ++data;
		//			(*data) = 1.0f - (*data); ++data;
		//			(*data) = 1.0f; ++data;
		//		}
		//	}

		//	// dirty the image (increments the modified count) so that any textures
		//	// using the image can be informed that they need to update.
		//	_image->dirty();
		//}

	}

	osg::Image* _image;
	const std::string _file;
};
