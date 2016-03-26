#ifndef BLOB_DETECTOR_H
#define BLOB_DETECTOR_H

#define labNote 0

#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;


class BlobDetector{
private:
#if labNote
	Ptr<BackgroundSubtractorMOG2> backgroungSubtractor;
#else
	BackgroundSubtractorMOG2 backgroungSubtractor;
#endif
  Mat morphologicalKernel;
  vector< vector<Point> > hulls;
  vector< RotatedRect> minEllipse;
  int minAreaBlob;
  int maxAreaBlob;
  int MIN_THRESHOLD;
  int DEST_THRESHOLD;
  

public:
  BlobDetector(int kernelShape, Size kernelSize, Point kernelAnchorPoint, int minArea = 0, int maxArea = 1000000, int minT=240, int maxT=255);
  vector< vector<Point> > getBlobs();
  vector< RotatedRect > getEllipses();
  void boundBlobs(Mat frame, Scalar color);
  void drawBlobs(Mat frame, Scalar color);
  void drawEllipses(Mat frame, Scalar color);
  void morphologyOperations(Mat orig, Mat dest);
  Mat findBlobs(Mat orig);
  void resetBackground();
};

#endif
