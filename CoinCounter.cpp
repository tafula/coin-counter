#include "CoinCounter.hpp"

//Centro de um coins_t
Point CenterCoin( coins_t moedinha){
#ifdef BLOB
		RotatedRect ellMoeda = fitEllipse( Mat(moedinha) );
		return ellMoeda.center;
#elif defined ELLIPSE
		return moedinha.center;
#elif defined HOUGH
		return {cvRound(moedinha[0]), cvRound(moedinha[1])};
#endif
}

//Area de um coins_t
double AreaCoin( coins_t moedinha){
#ifdef BLOB
		return countorArea(moedinha);
#elif defined ELLIPSE
		return moedinha.size.width * moedinha.size.height;
#elif defined HOUGH
		return pow(moedinha[2], 2);
#endif
}

//Construtor do CoinCounter com valores especificos da moeda
CoinCounter::CoinCounter(char const* titulo){
	for(int i = 0; i < 32; i++) Titulo[i] = titulo[i];
  coinValues = {5, 10, 25, 50, 100};
	ratDiaCoin = {22.0, 20.0, 25.0, 23.0, 27.0};
	coinFactor = {pow(ratDiaCoin[0],2)/pow(ratDiaCoin[4],2), pow(ratDiaCoin[1],2)/pow(ratDiaCoin[4],2), pow(ratDiaCoin[2],2)/pow(ratDiaCoin[4],2),pow(ratDiaCoin[3],2)/pow(ratDiaCoin[4],2), 1}; //Razao entre areas
}

//Devolve caracteristicas do tipo de moeda analisada
vector<double> CoinCounter::coinChars( int coinNumber, int valCoin, vector<coins_t> blobList){
	double expBlobSz = 0, sigmaBlobSz = 0;

	for(int i=0; i < blobList.size(); i++){
		double blobArea;

#ifdef BLOB
		blobArea = countorArea(blobList[i]) * coinFactor[coinNumber];
#elif defined ELLIPSE
		blobArea = (blobList[i].size.width * blobList[i].size.height) * coinFactor[coinNumber];
#elif defined HOUGH
		blobArea = pow(blobList[i][2], 2) * coinFactor[coinNumber];
#endif

		expBlobSz += blobArea/blobList.size();
	}
  int nextCoin = 4, prevCoin = 1;
	for(int i=0; i < ratDiaCoin.size(); i++){
		if (ratDiaCoin[i] > ratDiaCoin[coinNumber] && ratDiaCoin[i] <= ratDiaCoin[nextCoin]) nextCoin = i;
		if (ratDiaCoin[i] < ratDiaCoin[coinNumber] && ratDiaCoin[i] >= ratDiaCoin[prevCoin]) prevCoin = i;
	}
	if(nextCoin == coinNumber || prevCoin == coinNumber)
		sigmaBlobSz = expBlobSz * max(abs(coinFactor[coinNumber]- coinFactor[prevCoin]), abs(coinFactor[nextCoin]-coinFactor[coinNumber]));
	else sigmaBlobSz = expBlobSz * min(abs(coinFactor[coinNumber]- coinFactor[prevCoin]), abs(coinFactor[nextCoin]-coinFactor[coinNumber]));

	return {double(valCoin), expBlobSz - sigmaBlobSz/2, expBlobSz + sigmaBlobSz/2};
}


//Identifica qual moeda mais se aproxima do blob identificado
int CoinCounter::coinIdentifier( vector< vector<double> > coinAreas, coins_t blobCoin){
	double area = AreaCoin(blobCoin);

	for(int i=0; i < coinAreas.size(); i++){
		if( area >= coinAreas[i][0] && area <= coinAreas[i][1])
			return i;
	}
	return -1;
}

//Cruza a moeda com seu valor monetario
double CoinCounter::coinValue( vector<int> coinNumbers, int idCents){
	if( idCents < 0)
		return 0;
	return coinNumbers[idCents];
}

//Configura janela
void CoinCounter::AbreJanela(){
	cvNamedWindow(Titulo, CV_WINDOW_NORMAL);
	cvSetWindowProperty(Titulo, CV_WND_PROP_AUTOSIZE, CV_WINDOW_AUTOSIZE);
	ScreenSize ss = GetScreenSize();
	cvResizeWindow(Titulo, ss.width, ss.height);
}

void CoinCounter::LeMoedas(Mat orig, Mat frame){
	detector->morphologyOperations(frame, frame); /* operacoes morfologicas */

	coinsGot.clear();
#ifdef BLOB
	detector->findBlobs(frame); /* identifica e guarda blobs */
	coinsGot = detector->getBlobs();
#elif defined ELLIPSE
	detector->findEllipses(frame);
	coinsGot = detector->getEllipses();
#elif defined HOUGH
	detector->findHough(frame);
	coinsGot = detector->getHough();
#endif

	Scalar color = Scalar(255,255,0);
#ifdef BLOB
	detector->drawBlobs(orig, color); /* desenha blobs dos blobs na copia da imagem original */
//	imshow("Treated Image", detector.findBlobs(frame)); /* mostra masks para fins de debug */
#elif defined ELLIPSE
	detector->drawEllipses(orig, color);
	imshow("Treated Image", detector->findEllipses(frame));
#elif defined HOUGH
	detector->drawHough(orig, color);
	imshow("Treated Image", detector->findHough(frame));
#endif
}

vector< coins_t > CoinCounter::getCoins(){
	return coinsGot;
}

void CoinCounter::show(Mat orig){
	imshow(Titulo, orig);
}

void CoinCounter::resetBackground(){
	detector->resetBackground();
}

/***************************************/

