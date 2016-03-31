#ifndef COINCOUNTER_HPP
#define COINCOUNTER_HPP

/*** METODO ***/
#define HOUGH
/**************/

#include <opencv2/opencv.hpp>
#include <unistd.h>
#include <sys/time.h>
#include "processamento-de-imgs/Util.hpp"
#include "processamento-de-imgs/BlobDetector.hpp"

#define EROSION_DILATION_SIZE 3

using namespace cv;
using namespace std;

#ifdef BLOB
	typedef vector<Point> coins_t;
#elif defined ELLIPSE
	typedef RotatedRect coins_t;
#elif defined HOUGH
	typedef Vec3f coins_t;
#endif


//Funcoes de coins_t
double AreaCoin( coins_t moedinha);
Point CenterCoin( coins_t moedinha);


class CoinCounter{
private:
	//Vars
	vector<coins_t> coinsGot;
	vector<double> ratDiaCoin;
	vector<double> coinFactor;
	char Titulo[32];

public:
	CoinCounter(char const* titulo);

	//Vars
	vector<int> coinValues;
	BlobDetector* detector = new BlobDetector(MORPH_RECT, Size(2 * EROSION_DILATION_SIZE + 1, 2 * EROSION_DILATION_SIZE + 1), Point(EROSION_DILATION_SIZE, EROSION_DILATION_SIZE), 729, 10E6, 125, 255);

	//Funcs
	vector<double> coinChars( int coinNumber, int valCoin, vector<coins_t> blobList);
	int coinIdentifier( vector< vector<double> > coinAreas, coins_t blobCoin);
	double coinValue( vector<int> coinNumbers, int idCents);
	void AbreJanela();
	void LeMoedas(Mat orig, Mat frame);
	vector<coins_t> getCoins();
	void show(Mat orig);
	void resetBackground();
};
#endif
