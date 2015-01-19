#include <iostream>
#include <sys/time.h>
#include <string>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "../../src/VisionSoilAnalyzer/Vision/Vision.h"

using namespace std;
using namespace cv;
using namespace Vision;

void DisplayHelp()
{
	cout << "usage is --testfunction --testfunctionOptional --inputimg-filename" << endl;
	cout << "--Blur" << endl;
	cout << "--AdaptiveContrastStretch" << endl;
	cout << "--ConvertToBW" << endl;
	cout << "--RemoveBorderBlobs" << endl;
	cout << "--LabelBlobs" << endl;
	cout << "--Conversion" << endl;
	cout << "--GetEdges" << endl;
	cout << "--GetBlobList" << endl;
}

void TestBlur(const Mat &origImg, Mat &procImg)
{
	timespec tstart, tstop, tdiff;
	unsigned int elapsedTime;

	Enhance Test(origImg);

	clock_gettime(CLOCK_REALTIME, &tstart);
	Test.Blur(9);
	clock_gettime(CLOCK_REALTIME, &tstop);
	elapsedTime = (unsigned int)(tstop.tv_nsec - tstart.tv_nsec);
	cout << "Execution time for Blur : " << elapsedTime << " [ns] " << endl;

	procImg = Test.ProcessedImg;
}

void TestAdaptContrastStretch(const Mat &origImg, Mat &procImg)
{
	timespec tstart, tstop, tdiff;
	unsigned int elapsedTime;

	Enhance Test(origImg);
	Test.Blur(9);

	clock_gettime(CLOCK_REALTIME, &tstart);
	Test.AdaptiveContrastStretch(5, 2.5, true);
	clock_gettime(CLOCK_REALTIME, &tstop);
	elapsedTime = (unsigned int)(tstop.tv_nsec - tstart.tv_nsec);
	cout << "Execution time for AdaptiveContrastStretch : " << elapsedTime << " [ns] " << endl;

	procImg = Test.ProcessedImg;
}

void TestConvertToBW(const Mat &origImg, Mat &procImg)
{
	timespec tstart, tstop, tdiff;
	unsigned int elapsedTime;

	Segment Test(origImg);

	clock_gettime(CLOCK_REALTIME, &tstart);
	Test.ConvertToBW(Segment::Dark);
	clock_gettime(CLOCK_REALTIME, &tstop);
	elapsedTime = (unsigned int)(tstop.tv_nsec - tstart.tv_nsec);
	cout << "Execution time for ConvertToBW : " << elapsedTime << " [ns] " << endl;

	procImg = Test.ProcessedImg;
}

void TestRemoveBorderBlobs(const Mat &origImg, Mat &procImg)
{
	timespec tstart, tstop, tdiff;
	unsigned int elapsedTime;

	Segment Test(origImg);
	Test.ConvertToBW(Segment::Dark);

	clock_gettime(CLOCK_REALTIME, &tstart);
	Test.RemoveBorderBlobs(Segment::Eight, true);
	clock_gettime(CLOCK_REALTIME, &tstop);
	elapsedTime = (unsigned int)(tstop.tv_nsec - tstart.tv_nsec);
	cout << "Execution time for RemoveBorderBlobs : " << elapsedTime << " [ns] " << endl;

	procImg = Test.ProcessedImg;
}

void TestLabelBlobs(const Mat &origImg, Mat &procImg)
{
	timespec tstart, tstop, tdiff;
	unsigned int elapsedTime;

	Segment Test(origImg);
	Test.ConvertToBW(Segment::Dark);
	Test.RemoveBorderBlobs(Segment::Eight, true);

	clock_gettime(CLOCK_REALTIME, &tstart);
	Test.LabelBlobs(25, Segment::Eight, true);
	clock_gettime(CLOCK_REALTIME, &tstop);
	elapsedTime = (unsigned int)(tstop.tv_nsec - tstart.tv_nsec);
	cout << "Execution time for LabelBlobs : " << elapsedTime << " [ns] " << endl;

	procImg = Test.LabelledImg;
}

void TestConversion(const Mat &origImg, Mat &procImg, Conversion::ColorSpace dstColorspace)
{
	timespec tstart, tstop, tdiff;
	unsigned int elapsedTime;

	Conversion Test(origImg);
	clock_gettime(CLOCK_REALTIME, &tstart);
	Test.Convert(Conversion::RGB, dstColorspace);
	clock_gettime(CLOCK_REALTIME, &tstop);
	elapsedTime = (unsigned int)(tstop.tv_nsec - tstart.tv_nsec);
	cout << "Execution time for conversion from RGB 2 : " << dstColorspace << " took :" << elapsedTime << " [ns] " << endl;

	procImg = Test.ProcessedImg;
}

void TestGetEdges(const Mat &origImg, Mat &procImg)
{
	timespec tstart, tstop, tdiff;
	unsigned int elapsedTime;

	Segment Test(origImg);
	Test.ConvertToBW(Segment::Dark);
	clock_gettime(CLOCK_REALTIME, &tstart);
	Test.GetEdges(Segment::Eight, true);
	clock_gettime(CLOCK_REALTIME, &tstop);
	elapsedTime = (unsigned int)(tstop.tv_nsec - tstart.tv_nsec);
	cout << "Execution time for get Edges took :" << elapsedTime << " [ns] " << endl;

	procImg = Test.ProcessedImg;
}

void TestGetBlobList(const Mat&origImg)
{
	timespec tstart, tstop, tdiff;
	unsigned int elapsedTime;

	Segment Test(origImg);
	Test.ConvertToBW(Segment::Dark);
	clock_gettime(CLOCK_REALTIME, &tstart);
	Test.GetBlobList(Segment::Eight, true);
	clock_gettime(CLOCK_REALTIME, &tstop);
	elapsedTime = (unsigned int)(tstop.tv_nsec - tstart.tv_nsec);
	cout << "Execution time for get BlobList took :" << elapsedTime << " [ns] " << endl;
	imwrite("LabelBlobs.ppm", Test.LabelledImg);

	uint32_t i = 1;
	string filen;
	while (i < Test.MaxLabel)
	{
		ostringstream ss;
		ss << i;
		filen = ss.str() + ".ppm";
		imwrite(filen, Test.BlobList[i].Img);
		ss.str("");
		ss.clear();
		ss << i;
		filen = ss.str() + "L.ppm";
		imwrite(filen, Test.LabelledImg(Test.BlobList[i++].cvROI));
	}
}


int main(int argc, char *argv[])
{
	Mat origImg;
	Mat procImg;

	if (argc > 1)
	{
		std::string arg = argv[1];
		std::string filename = argv[argc - 1];
		filename = filename.substr(11, filename.length() - 11);

		if (arg == "--help") DisplayHelp();
		else
		{
			for (uint8_t i = 1; i < (argc-1); i++)
			{
				arg = argv[i];
				if (arg == "--Blur")
				{
					origImg = imread(filename, 0);
					TestBlur(origImg, procImg);
					imwrite("Blur.ppm", procImg);
				}
				else if (arg == "--AdaptiveContrastStretch")
				{
					origImg = imread(filename, 0);
					TestAdaptContrastStretch(origImg, procImg);
					imwrite("AdaptiveContrastStretch.ppm", procImg);
				}
				else if (arg == "--ConvertToBW")
				{
					origImg = imread(filename, 0);
					TestConvertToBW(origImg, procImg);
					imwrite("ConvertToBW.ppm", procImg);
				}
				else if (arg == "--RemoveBorderBlobs")
				{
					origImg = imread(filename, 0);
					TestRemoveBorderBlobs(origImg, procImg);
					imwrite("RemoveBorderBlobs.ppm", procImg);
				}
				else if (arg == "--LabelBlobs")
				{
					origImg = imread(filename, 0);
					TestLabelBlobs(origImg, procImg);
					imwrite("LabelBlobs.ppm", procImg);
				}
				else if (arg == "--Conversion")
				{
					origImg = imread(filename, 1);
					TestConversion(origImg, procImg, Conversion::Intensity);
					imwrite("Intensity.ppm", procImg);

					TestConversion(origImg, procImg, Conversion::CIE_XYZ);
					imwrite("XYZ.ppm", procImg);

					TestConversion(origImg, procImg, Conversion::CIE_lab);
					imwrite("Lab.ppm", procImg);

				}
				else if (arg == "--GetEdges")
				{
					origImg = imread(filename, 0);
					TestGetEdges(origImg, procImg);
					imwrite("Edges.ppm", procImg);
				}
				else if (arg == "--GetBlobList")
				{
					origImg = imread(filename, 0);
					TestGetBlobList(origImg);
				}
				else
				{
					DisplayHelp();
				}
			}
		}
	}
	else {	DisplayHelp(); }

	return 0;
}