#ifndef TOPOS_CORRECTOR_H
#define TOPOS_CORRECTOR_H

#include <cmath>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class ToposCorrector{
private:
	Point pivot;
	double Apivot;
	vector<Point> basisGot;
	vector< vector<double> > changeBasis;
	vector<double> linGrad;
  

public:
	ToposCorrector(Point pPivot, double Apivot, vector<Point> coordP, vector<double> Apts);
	double correctTopos(Point p);
};

#endif
