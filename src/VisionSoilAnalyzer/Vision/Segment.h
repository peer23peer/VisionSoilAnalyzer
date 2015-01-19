#pragma once
#define SEGMENT_VERSION 1

#include <vector>
#include <string>

#include "ImageProcessing.h"
#include "../SoilMath/SoilMath.h"

namespace Vision
{
	class Segment :
		public ImageProcessing
	{
	public:
		/*! Enumerator to indicate what kind of object to extract  */
		enum TypeOfObjects
		{
			Bright,		/*!< Enum value Bright object */
			Dark		/*!< Enum value Dark object. */
		};

		/*! Enumerator to indicate how the pixel correlate between each other in a blob*/
		enum Connected
		{
			Four,	/*!< Enum Four connected, relation between Center, North, East, South and West*/
			Eight	/*!< Enum Eight connected, relation between Center, North, NorthEast, East, SouthEast, South, SouthWest, West and NorthWest */
		};

		cv::Mat LabelledImg;	/*!< Image with each individual blob labeled with a individual number */
		uint16_t MaxLabel = 0;	/*!< Maximum labels found in the labelled image*/
		uint16_t noOfFilteredBlobs = 0;	/*!< Total numbers of blobs that where filtered beacuse the where smaller than the minBlobArea*/

		/*! Coordinates for the region of interest*/
		typedef struct Rect
		{
			uint16_t leftX;		/*!< Left X coordinate*/
			uint16_t leftY;		/*!< Left Y coordinate*/
			uint16_t rightX;	/*!< Right X coordinate*/
			uint16_t rightY;	/*!< Right Y coordinate*/
		} Rect;

		/*! Individual blob*/
		typedef struct Blob
		{
			ushort Label;	/*!< ID of the blob*/
			cv::Mat Img;	/*!< BW image of the blob all the pixel belonging to the blob are set to 1 others are 0*/
			Rect ROI;		/*!< Coordinates for the blob in the original picture*/
			cv::Rect cvROI;	/*!< Coordinates for the blob in the original picture as a cv::Rect*/
		} Blob;

		vector<Blob> BlobList; /*!< vector with all the individual blobs*/

		SoilMath::ucharStat_t OriginalImgStats; /*!< Statistical data from the original image*/
		uint8_t ThresholdLevel = 0;				/*!< Current calculated threshold level*/
		
		Segment(); 
		Segment(const Mat &src); 
		~Segment(); 

		void ConvertToBW(TypeOfObjects Typeobjects);
		void ConvertToBW(const Mat &src, Mat &dst, TypeOfObjects Typeobjects);

		void GetEdges(Connected conn = Eight, bool chain = false);
		void GetEdges(const Mat &src, Mat &dst, Connected conn = Eight,  bool chain = false);

		void GetBlobList(Connected conn = Eight, bool chain = false);
		void GetBlobList(const Mat &src, Mat &dst, Connected conn = Eight, bool chain = false);

		void Threshold(uchar t, TypeOfObjects Typeobjects);

		void LabelBlobs(uint16_t minBlobArea = 25, Connected conn = Eight, bool chain = false);

		void makeConsecutive(uint16_t LastLabelUsed, uint16_t * tempLUT, uint16_t * &LUT_newVal);

		void RemoveBorderBlobs(Connected conn = Eight, bool chain = false);

	private:

		uint8_t GetThresholdLevel(TypeOfObjects TypeObject);
		void SetBorder(uchar *P, uchar setValue);
	};
}