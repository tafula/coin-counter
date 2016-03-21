/***********************************************/
/************* CONTADOR DE MOEDAS **************/
/***         **********************          ***/
/***   Contador de objetos general-purpose   ***/
/*** modificado para identificacao de moedas ***/
/***         e contagem de dinheiro.         ***/
/***********************************************/

#include <opencv2/opencv.hpp>
#include <unistd.h>
#include <sys/time.h>
#include "processamento-de-imgs/Util.hpp"
#include "processamento-de-imgs/BlobDetector.hpp"

#define EROSION_DILATION_SIZE 3

using namespace cv;
using namespace std;

//Identifica qual moeda mais se aproxima do blob identificado
int coinIdentifier( vector<vector<double>> coinAreas, vector<Point> hull){
	double area = contourArea(hull);
	for(int i=0; i < coinAreas.size(); i++){
		if( area >= coinAreas[i][0] && area <= coinAreas[i][1])
			return i;
	}
	return -1;
}

//Cruza a moeda com seu valor monetario
double coinValue( vector<int> coinNumbers, int idCents){
	if( idCents < 0)
		return 0;
	return coinNumbers[idCents];
}


int main(int argc, char** argv){
	
	/*** Abre settings.txt para configurar o programa ***/
	/****************************************************/
	FILE* setter = fopen("settings.txt", "r");
	if(setter == NULL){
		cout << "Problema lendo settings.txt!";
		exit(1);
	}
	
	vector<int> coinNumbers;
	vector<vector<double>> coinAreas;
		
	char c;
	while( (c = fgetc(setter)) != EOF){
		if(c == '='){
			int nCoin;
			fscanf(setter, "%d", &nCoin);
			coinNumbers.push_back(nCoin);

			vector<double> coinSz(2,0);
			fscanf(setter, "%lf %lf", &coinSz[0], &coinSz[1]);
			coinAreas.push_back(coinSz);
		}
	}
	fclose(setter);
//	for(int j=0; j < coinNumbers.size(); j++) cout << coinNumbers[j] << "\n";
//	for(int j=0; j < coinAreas.size(); j++) cout << coinAreas[j][0] << " " << coinAreas[j][1] << "\n";
	/****************************************************/

	Mat frame;
	Mat orig;
	Mat fore;

	time_t camAdaptationStartTime = time(NULL);
	bool camAdapted = false;

	vector<int> coinCount(coinNumbers.size(), 0); /* inicializa contador de moedas individual */
	vector<vector<Point>> objects;
	
	//Configura janela
	cvNamedWindow("Contador de moedas", CV_WINDOW_NORMAL);
	cvSetWindowProperty("Contador de moedas", CV_WND_PROP_AUTOSIZE, CV_WINDOW_AUTOSIZE);
	ScreenSize ss = GetScreenSize();
	cvResizeWindow("Contador de moedas", ss.width, ss.height);

	//Pega camera
	EnableCameraAutoAdjust(GetVideoNum(argc, argv));
	VideoCapture cap(GetVideoNum(argc, argv));

	//Inicializa o BlobDetector
	BlobDetector detector(MORPH_RECT, Size(2 * EROSION_DILATION_SIZE + 1, 2 * EROSION_DILATION_SIZE + 1), Point(EROSION_DILATION_SIZE, EROSION_DILATION_SIZE), 729, 10E6, 125, 255);

	while(true){
		cap >> frame;
		frame.copyTo(orig); /* salva copia da imagem original */

		/*** Main ***/
		/************************************************************************/
		if( camAdapted){
			detector.morphologyOperations(frame, frame); /* operacoes morfologicas */
			detector.findBlobs(frame); /* identifica e guarda blobs */
			imshow("Treated Image", detector.findBlobs(frame)); /* mostra masks para fins de debug */
      
			detector.drawBlobs(orig, Scalar(255,255,0)); /* desenha convex hull dos blobs na copia da imagem original */

			for(int i=0; i < coinCount.size(); i++) //Limpa contador individual de moedas
				coinCount[i] = 0;

			double totalMoney = 0;
			for(int i=0; i < detector.getBlobs().size(); i++){ //Conta moedas e soma dinheiro
				int coinID = coinIdentifier(coinAreas, detector.getBlobs()[i]);
				coinCount[coinID]++;
				totalMoney += coinValue( coinNumbers, coinID);
			}

			//Printa qtde de moedas individuais na tela
			for(int i=0; i < coinCount.size()-1; i++){
				char buffCoin[11];
				sprintf(buffCoin, "%02dc: %d", coinNumbers[i], coinCount[i]);
				putText(orig,buffCoin,Point(10, 20 + i*20), FONT_HERSHEY_PLAIN, 1,Scalar(255,255,255), 4);
			}
			char buffCoin[11];
			sprintf(buffCoin, "R$1: %d", coinCount[4]);
			putText(orig,buffCoin,Point(10, 20 + 4*20), FONT_HERSHEY_PLAIN, 1,Scalar(255,255,255), 4);

			//Printa valor em dinheiro na tela
			char buffer[11];
//			string tipoObjeto = "moeda";
//			if( detector.getBlobs().size() != 1 && detector.getBlobs().size() != 0)
//				tipoObjeto += "s";
//			sprintf(buffer, "%lu %s", detector.getBlobs().size(), tipoObjeto.c_str());
			sprintf(buffer, "R$ %0.2f", totalMoney/100);
			putText(orig,buffer,Point(20,orig.size().height-40), FONT_HERSHEY_PLAIN, 6,Scalar(255,255,255),4);
		} /**********************************************************************/


		else{
			if( (time(NULL) - camAdaptationStartTime) > ADAPTATION_TIME_SEC){
				camAdapted = true;
				detector.resetBackground();
				DisableCameraAutoAdjust(GetVideoNum(argc, argv));
			}
			else{
				string text = "Configurando...";
				Size txtSz = getTextSize(text, FONT_HERSHEY_PLAIN, 4, 4, NULL);
				putText(orig,text,Point(orig.size().width / 2 - txtSz.width / 2, orig.size().height /2 - 2* txtSz.height), FONT_HERSHEY_PLAIN, 4, Scalar(0, 0, 255), 4);
                
				char buff[3];
				sprintf(buff, "%ld", ADAPTATION_TIME_SEC - abs(time(NULL) -camAdaptationStartTime));
				txtSz = getTextSize(buff, FONT_HERSHEY_PLAIN, 4, 4, NULL);
				cv::putText(orig, buff, Point(orig.size().width / 2 - txtSz.width / 2, orig.size().height /2 ), cv::FONT_HERSHEY_PLAIN, 4, Scalar(0, 0, 255), 4);
			}
		}

		imshow("Contador de moedas", orig);


		/*** Acoes do teclado ***/
		/************************************************/
		int key = cv::waitKey(30);
		if ((key & 0xFF) == 27) {      //Esc: sair
			exit(0);
		}
		else if ((key & 0xFF) == ' '){ //Space: recalibragem
			camAdapted = false;
			EnableCameraAutoAdjust(GetVideoNum(argc, argv));   
			camAdaptationStartTime = time(NULL);
		}	/**********************************************/

	}
}
