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
	return moedinha.size.width * moedinha.size.height * PI;
#elif defined HOUGH
	return pow(moedinha[2], 2) * PI;
#endif
}

//Construtor do CoinCounter com valores especificos da moeda
CoinCounter::CoinCounter(char const* titulo){
	for(int i = 0; i < 32; i++) Titulo[i] = titulo[i];
	coinValues = {5, 10, 25, 50, 100};
	ratDiaCoin = {22.0, 20.0, 25.0, 23.0, 27.0};
	coinFactor = {pow(ratDiaCoin[0],2)/pow(ratDiaCoin[4],2), pow(ratDiaCoin[1],2)/pow(ratDiaCoin[4],2), pow(ratDiaCoin[2],2)/pow(ratDiaCoin[4],2),pow(ratDiaCoin[3],2)/pow(ratDiaCoin[4],2), 1}; /* Razao entre areas */

	for(int i = 0; i < coinValues.size(); i++) /* inicializa contador individual */
		coinCount.push_back(0);

	coinsGot.clear();
	keyPts.clear();
}

//Atualiza pivot
void CoinCounter::atualizaTrackCoin(coins_t* moedinha){
	for(int i=0; i < coinsGot.size(); i++){
		Point diff = CenterCoin(coinsGot[i]) - CenterCoin(*moedinha);
		if( pow(diff.x, 2) + pow(diff.y, 2) <= AreaCoin(*moedinha)/PI ){
			*moedinha = coinsGot[i];
			break;
		}
	}
}


//Devolve caracteristicas do tipo de moeda analisada
vector<double> CoinCounter::coinChars( int coinNumber, vector<coins_t> blobList){
	double expBlobSz = 0, sigmaBlobSz = 0;

	for(int i=0; i < blobList.size(); i++){
		double blobArea;

#ifdef BLOB
		blobArea = countorArea(blobList[i]) * coinFactor[coinNumber];
#elif defined ELLIPSE
		blobArea = (blobList[i].size.width * blobList[i].size.height) * PI * coinFactor[coinNumber];
#elif defined HOUGH
		blobArea = pow(blobList[i][2], 2) * PI * coinFactor[coinNumber];
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

	return {expBlobSz - sigmaBlobSz/2, expBlobSz + sigmaBlobSz/2};
}


//Identifica qual moeda mais se aproxima do blob identificado
int CoinCounter::coinIdentifier( coins_t blobCoin, double topCorrection){
	double area = AreaCoin(blobCoin) * topCorrection;

	cout << topCorrection << "\n";

	for(int i=0; i < coinAreas.size(); i++){
		if( area >= coinAreas[i][0] && area <= coinAreas[i][1])
			return i;
	}
	return -1;
}

//Cruza a moeda com seu valor monetario
double CoinCounter::coinAreaToValue( vector<int> coinNumbers, int idCents){
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

//Le moedas da camera
void CoinCounter::LeMoedas(Mat orig, Mat frame, Scalar color){
	detector->morphologyOperations(frame, frame); /* operacoes morfologicas */
	
	vector<coins_t> tempCoinsGot;
	tempCoinsGot.clear();
#ifdef BLOB
	detector->findBlobs(frame); /* identifica e guarda blobs */
	coinsGot = detector->getBlobs();
	detector->drawBlobs(orig, color); /* desenha blobs dos blobs na copia da imagem original */
//	imshow("Treated Image", detector.findBlobs(frame)); /* mostra masks para fins de debug */
#elif defined ELLIPSE
	detector->findEllipses(frame);
	coinsGot = detector->getEllipses();
	detector->drawEllipses(orig, color);
	imshow("Treated Image", detector->findEllipses(frame));
#elif defined HOUGH
	detector->findHough(frame);
	tempCoinsGot = detector->getHough();

	imshow("Treated Image", detector->findHough(frame));
#endif

	/* trackeia o pivot conforme a camera nao muda muito */
	for(int i = 0; i < coinsGot.size(); i++){
		bool PodePoppar = true;
		for(int j=0; j < tempCoinsGot.size(); j++){
			Point diff = CenterCoin(coinsGot[i]) - CenterCoin(tempCoinsGot[j]);
			if( pow(diff.x, 2) + pow(diff.y, 2) <= AreaCoin(coinsGot[i])/PI ){
				coinsGot[i] = tempCoinsGot[j];
				tempCoinsGot.erase( tempCoinsGot.begin() + j);
				PodePoppar = false;
				break;
			}
		}
		if( PodePoppar == true){
			coinsGot.erase( coinsGot.begin() + i );
			i--;
		}
	}
	int i = 0;
	for (auto el : tempCoinsGot) {
		coinsGot.push_back(el);
	}

	detector->drawHough(orig, color, coinsGot);

	circle( orig, CenterCoin(pivot), sqrt(AreaCoin(pivot)/PI), Scalar(0,255,0), 2, 8, 0);
	if(!keyPts.empty() ){
		circle( orig, CenterCoin(keyPts[0]), sqrt(AreaCoin(keyPts[0])/PI), Scalar(0,0,255), 2, 8, 0);
		circle( orig, CenterCoin(keyPts[1]), sqrt(AreaCoin(keyPts[1])/PI), Scalar(0,0,255), 2, 8, 0);
	}
} 

//retorna moedinhas
vector< coins_t > CoinCounter::getCoins(){
	return coinsGot;
}

//mostra imagem na tela
void CoinCounter::show(Mat orig){
	imshow(Titulo, orig);
}

//reseta o background via BlobDetector
void CoinCounter::resetBackground(){
	detector->resetBackground();
}

//Tela antes de settar tamanhos das moedas
void CoinCounter::preSettings(Mat orig){
	char buffer[11];
	string tipoObjeto = "moeda";
	if( coinsGot.size() != 1 && coinsGot.size() != 0)
		tipoObjeto += "s";
	sprintf(buffer, "%lu %s", coinsGot.size(), tipoObjeto.c_str());
	putText(orig, buffer, Point(20,orig.size().height-40), FONT_HERSHEY_PLAIN, 6, Scalar(255,255,255), 4);
}

//Configura os valores das areas
void CoinCounter::settaSettings(){
	coinAreas.clear();
	for(int i = 0; i < coinValues.size(); i++)
		coinAreas.push_back( coinChars(i, {coinsGot[0]}) );

	pivot = coinsGot[0]; /* getta o pivot */

	keyPts.clear(); /* getta os key pts */
	keyPts.push_back(coinsGot[1]);	
	keyPts.push_back(coinsGot[2]);
}

//Limpa valores das areas
void CoinCounter::resetaSettings(){
	coinsGot.clear();
	coinAreas.clear();
}

//Tela depois de settar tamanhos das moedas
void CoinCounter::posSettings(Mat orig){
	pivot = coinsGot[0]; /* getta o pivot */
	keyPts[0] = coinsGot[1];	
	keyPts[1] = coinsGot[2];

	coinAreas.clear();
	for(int i = 0; i < coinValues.size(); i++)
		coinAreas.push_back( coinChars(i, {pivot}) );

	ToposCorrector mesa( CenterCoin(pivot), AreaCoin(pivot), {CenterCoin(keyPts[0]), CenterCoin(keyPts[1])}, {AreaCoin(keyPts[0]), AreaCoin(keyPts[1])} ); /* inicializa o corretor da topologia da mesa */

	for(int i=0; i < coinCount.size(); i++) /* Limpa contador individual de moedas */
		coinCount[i] = 0;

	totalMoney = 0;
	for(int i=0; i < coinsGot.size(); i++){ /* Conta moedas e soma dinheiro */
		int coinID = coinIdentifier( coinsGot[i], mesa.correctTopos(CenterCoin( coinsGot[i] )) );
		coinCount[coinID]++;
		totalMoney += coinAreaToValue( coinValues, coinID);

		char buffCoin[11]; /* Printa o valor da moeda em cima desta */
		if(coinID >= 0 && coinID < coinCount.size()-1)
			sprintf(buffCoin, "%02dc", coinValues[coinID]);
		else if(coinID == coinCount.size()-1)
			sprintf(buffCoin, "R$1");
		else
			sprintf(buffCoin, "???");
		putText(orig, buffCoin, CenterCoin(coinsGot[i]), FONT_HERSHEY_PLAIN, 1, Scalar(255,255,0), 4);
	}

	/* Printa qtde de moedas individuais na tela */
	for(int i=0; i < coinCount.size()-1; i++){
		char buffCoin[11];
		sprintf(buffCoin, "%02dc: %d", coinValues[i], coinCount[i]);
		putText(orig, buffCoin, Point(10, 20 + i*20), FONT_HERSHEY_PLAIN, 1, Scalar(255,255,255), 4);
	}
	char buffCoin[11];
	sprintf(buffCoin, "R$1: %d", coinCount[4]);
	putText(orig, buffCoin, Point(10, 20 + 4*20), FONT_HERSHEY_PLAIN, 1, Scalar(255,255,255), 4);

	/* Printa valor em dinheiro na tela */
	char buffer[11];
//	string tipoObjeto = "moeda";
//	if( detector.getBlobs().size() != 1 && detector.getBlobs().size() != 0)
//		tipoObjeto += "s";
//	sprintf(buffer, "%lu %s", detector.getBlobs().size(), tipoObjeto.c_str());
	sprintf(buffer, "R$ %0.2f", totalMoney/100);
	putText(orig, buffer, Point(20,orig.size().height-40), FONT_HERSHEY_PLAIN, 6, Scalar(255,255,255), 4);
}
/***************************************/

