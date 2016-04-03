#include "ToposDetector.hpp"

ToposDetector::ToposDetector(Point pPivot, double ApPivot, vector<Point> coordP, vector<double> Apts){
	pivot = pPivot;
	Apivot = ApPivot;
	basisGot = {coordP[0] - pivot, coordP[1] - pivot};
	double basisDet = (basisGot[0].x * basisGot[1].y) - (basisGot[0].y * basisGot[1].x);
	changeBasis = { {basisGot[1].y/basisDet, -basisGot[0].y/basisDet},
	                {-basisGot[1].x/basisDet, basisGot[0].x/basisDet} };

	linGrad = {sqrt(Apts[0])- sqrt(Apivot), sqrt(Apts[1])-sqrt(Apivot)};	
}

//Corrige area sobre plano inclinado
double ToposDetector::correctTopos(Point p){
	vector<double> pInBasis = {(changeBasis[0][0] + changeBasis[0][1]) * (p-pivot).x,
	                          (changeBasis[1][0] + changeBasis[1][1]) * (p-pivot).y };
	
	return pow(sqrt(Apivot) + pInBasis[0]*linGrad[0] + pInBasis[1]*linGrad[1], 2) / Apivot;
}

//Get coisas
