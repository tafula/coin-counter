#include <opencv2/opencv.hpp>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include "processamento-de-imgs/Util.hpp"
#include "processamento-de-imgs/BlobDetector.hpp"

#define QTT_SIGMA 1.5
#define EROSION_DILATION_SIZE 3

using namespace cv;
using namespace std;

//Devolve caracteristicas do tipo de moeda analisada
vector<double> coinChars( int valCoin, vector<RotatedRect> blobList){
	double expBlobSz = 0, sigmaBlobSz = 0, qSigma = 1.5;

	for(int i=0; i < blobList.size(); i++){
		Size2f areaEll = blobList[i].size;
		double blobArea = areaEll.width * areaEll.height;
		expBlobSz += blobArea/blobList.size();
		sigmaBlobSz += (blobArea/blobList.size()) * blobArea;
	}
	sigmaBlobSz = sqrt(sigmaBlobSz - pow(expBlobSz, 2));
	return {double(valCoin), expBlobSz - (QTT_SIGMA * sigmaBlobSz), expBlobSz + (QTT_SIGMA * sigmaBlobSz)};
}


int main(int argc, char** argv){
	Mat frame;
	Mat orig;
	Mat fore;

	time_t camAdaptationStartTime = time(NULL);
	bool camAdapted = false;

	vector<vector<Point>> objects;
	
	//Configura janela
	cvNamedWindow("Calibrador", CV_WINDOW_NORMAL);
	cvSetWindowProperty("Calibrador", CV_WND_PROP_AUTOSIZE, CV_WINDOW_AUTOSIZE);
	ScreenSize ss = GetScreenSize();
	cvResizeWindow("Calibrador", ss.width, ss.height);

	//Pega camera
	EnableCameraAutoAdjust(GetVideoNum(argc, argv));
	VideoCapture cap(GetVideoNum(argc, argv));

	//Inicializa o BlobDetector
	BlobDetector detector(MORPH_ELLIPSE, Size(2 * EROSION_DILATION_SIZE + 1, 2 * EROSION_DILATION_SIZE + 1), Point(EROSION_DILATION_SIZE, EROSION_DILATION_SIZE), 729, 10E6, 125, 255);

	int k = 0;
	vector<int> coinValues = {5, 10, 25, 50, 100};
	vector<vector<double>> allCoins;

	while(k < coinValues.size()){
		cap >> frame;
		frame.copyTo(orig);

		/*** Main ***/
		/*********************************************/
		if( camAdapted){
			detector.morphologyOperations(frame, frame);
			detector.findBlobs(frame);
//			imshow("Treated Image", detector.findBlobs(frame)); /* mostra masks para fins de debug */
      
			detector.drawEllipses(orig, Scalar(255,255,0));

			//Printa quais moedas ja foram calibradas
			for(int i=0; i < coinValues.size()-1; i++){
				char buffCoin[11] = " ";
				if( i < k) sprintf(buffCoin, "%02dc: OK", coinValues[i]);
				else if( i == k) sprintf(buffCoin, "%02dc: --", coinValues[i]);
				putText(orig,buffCoin,Point(10, 20 + i*20), FONT_HERSHEY_PLAIN, 1,Scalar(255,255,255), 4);
			}
			char buffCoin[11] = " ";
			if( k > 4) sprintf(buffCoin, "R$1: OK");
			else if( k == 4) sprintf(buffCoin, "R$1: --");
			putText(orig,buffCoin,Point(10, 20 + 4*20), FONT_HERSHEY_PLAIN, 1,Scalar(255,255,255), 4);

			//Printa num de moedas identificadas
			char buffer[11];
			string tipoObjeto = "moeda";
			if( detector.getEllipses().size() != 1 && detector.getEllipses().size() != 0)
				tipoObjeto += "s";
			sprintf(buffer, "%lu %s", detector.getEllipses().size(), tipoObjeto.c_str());
			putText(orig,buffer,Point(20,orig.size().height-40),FONT_HERSHEY_PLAIN,6,Scalar(255,255,255),4);

		} /*******************************************/

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
		if ((key & 0xFF) == 27){       //Esc: sair
			exit(0);
		}
		else if ((key & 0xFF) == ' '){ //Space: recalibragem
			camAdapted = false;
			EnableCameraAutoAdjust(GetVideoNum(argc, argv));   
			camAdaptationStartTime = time(NULL);
		}
		else if ((key & 0xFF) == 'a' || (key & 0xFF) == 'A'){ //A: salva moeda
			allCoins.push_back( coinChars(coinValues[k], detector.getEllipses()) );			
//			camAdapted = false;
//			EnableCameraAutoAdjust(GetVideoNum(argc, argv));   
//			camAdaptationStartTime = time(NULL);
			k++;
		} /**********************************************/
	}

	/*** Salva mudancas no settings.txt ***/
	/************************************************/
	FILE* setter = fopen("settings.txt", "w+");
	if(setter == NULL){
		cout << "Problema lendo settings.txt!";
		exit(1);
	}
	
	fprintf(setter, "/********************************************/\n/*************** SETTINGS.TXT ***************/\n/***           ****************           ***/\n/***** Para adicionar uma moeda escreva: ****/\n/* \"coin 'igual' valor - areaMin - areaMax\" */\n/********************************************/\n\n");

	for(k=0; k < coinValues.size(); k++)
			fprintf(setter, "coin = %03d %.2f %.2f\n", int(allCoins[k][0]), allCoins[k][1], allCoins[k][2]);
	fclose(setter);
	/************************************************/	

}