/***********************************************/
/************* CONTADOR DE MOEDAS **************/
/***         **********************          ***/
/***   Contador de objetos general-purpose   ***/
/*** modificado para identificacao de moedas ***/
/***         e contagem de dinheiro.         ***/
/***********************************************/

#include "CoinCounter.hpp"

int main(int argc, char** argv){
	CoinCounter* contador = new CoinCounter("Contador de Moedas"); /* inicializa o CoinCounter */	

	Mat frame;
	Mat orig;

	time_t camAdaptationStartTime = time(NULL);
	bool camAdapted = false;
	bool SETTADO = false;
	
	//Configura janela do CoinCounter
	contador->AbreJanela();

	//Pega camera
	EnableCameraAutoAdjust(GetVideoNum(argc, argv));
	VideoCapture cap(GetVideoNum(argc, argv));

	while(true){
		cap >> frame;
		frame.copyTo(orig); /* salva copia da imagem original */

		/*** Main ***/
		/************************************************************************/
		if( camAdapted){
			contador->LeMoedas(orig, frame, Scalar(255,255,0));
			vector<coins_t> coinsGot = contador->getCoins();

			if(SETTADO == false)
				contador->preSettings(orig);
			if(SETTADO == true)
				contador->posSettings(orig);
		} /**********************************************************************/


		else{
			if( (time(NULL) - camAdaptationStartTime) > ADAPTATION_TIME_SEC){
				camAdapted = true;
				contador->resetBackground();
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

		contador->show(orig);


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
		}
		else if (((key & 0xFF) == 's' || (key & 0xFF) == 'S') && SETTADO == false){ //S: setta settings
			contador->settaSettings();
			SETTADO = true;
//			camAdapted = false;
//			EnableCameraAutoAdjust(GetVideoNum(argc, argv));   
//			camAdaptationStartTime = time(NULL);
		}
		else if (((key & 0xFF) == 'r' || (key & 0xFF) == 'R') && SETTADO == true){ //S: reseta settings
			contador->resetaSettings();
			SETTADO = false;
//			camAdapted = false;
//			EnableCameraAutoAdjust(GetVideoNum(argc, argv));   
//			camAdaptationStartTime = time(NULL);
		} /**********************************************/

	}
}
