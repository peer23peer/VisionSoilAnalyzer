/*! \class ImageProcessing
\brief Core class of all the image classes
Core class of all the image classes with a few commonly shared functions and variables
*/
#include "ImageProcessing.h"

namespace Vision
{
	/*! Constructor of the core class*/
	ImageProcessing::ImageProcessing()	{	}

	/*! De-constructor of the core class*/
	ImageProcessing::~ImageProcessing() {	}

	/*! Create a LUT indicating which iteration variable i is the end of an row
	\param nData an int indicating total pixels
	\param hKsize int half the size of the kernel, if any. which acts as an offset from the border pixels
	\param nCols int number of columns in a row
	\return array of uchars where a zero is a middle column and a 1 indicates an end of an row minus the offset from half the kernel size
	*/
	uchar* ImageProcessing::GetNRow(int nData, int hKsize, int nCols)
	{
		// Create LUT to determine when there is an new row
		uchar *nRow = new uchar[nData] {};
		int i = 0;
		i = OriginalImg.rows + 1;
		while (i-- > 1)	{ nRow[i * nCols - hKsize] = 1; }
		return nRow;
	}
}