#include "CoinCounter.hpp"

int main(int argc, char** argv){
	Mat frame;
	Mat orig;

	time_t camAdaptationStartTime = time(NULL);
	bool camAdapted = false;
	
	//Configura janela
	CoinCounter* calibrador = new CoinCounter("Calibrador do CoinCounter");
	calibrador->AbreJanela();

	//Pega camera
	EnableCameraAutoAdjust(GetVideoNum(argc, argv));
	VideoCapture cap(GetVideoNum(argc, argv));

	int k = 0;
	vector<vector<double>> allCoins;

	while(k < 1){
		cap >> frame;
		frame.copyTo(orig);

		/*** Main ***/
		/*********************************************/
		if( camAdapted){
			calibrador->LeMoedas(orig, frame);
			vector<coins_t> coinsGot = calibrador->getCoins();
      
			//Printa quais moedas ja foram calibradas
/***			for(int i=0; i < coinValues.size()-1; i++){
				char buffCoin[11] = " ";
				if( i < k) sprintf(buffCoin, "%02dc: OK", coinValues[i]);
				else if( i == k) sprintf(buffCoin, "%02dc: --", coinValues[i]);
				putText(orig,buffCoin,Point(10, 20 + i*20), FONT_HERSHEY_PLAIN, 1,Scalar(255,255,255), 4);
			}
			char buffCoin[11] = " ";
			if( k > 4) sprintf(buffCoin, "R$1: OK");
			else if( k == 4) sprintf(buffCoin, "R$1: --");
			putText(orig,buffCoin,Point(10, 20 + 4*20), FONT_HERSHEY_PLAIN, 1,Scalar(255,255,255), 4);  ***/

			//Printa num de moedas identificadas
			char buffer[11];
			string tipoObjeto = "moeda";
			if( coinsGot.size() != 1 && coinsGot.size() != 0)
				tipoObjeto += "s";
			sprintf(buffer, "%lu %s", coinsGot.size(), tipoObjeto.c_str());
			putText(orig,buffer,Point(20,orig.size().height-40),FONT_HERSHEY_PLAIN,6,Scalar(255,255,255),4);

		} /*******************************************/

		else{
			if( (time(NULL) - camAdaptationStartTime) > ADAPTATION_TIME_SEC){
				camAdapted = true;
				calibrador->resetBackground();
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

		calibrador->show(orig);


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
		else if ((key & 0xFF) == 's' || (key & 0xFF) == 'S'){ //S: salva
			for(int i = 0; i < 5; i++)
				allCoins.push_back( calibrador->coinChars(i, calibrador->coinValues[i], calibrador->getCoins()) );			
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

	for(k=0; k < calibrador->coinValues.size(); k++)
			fprintf(setter, "coin = %03d %.2f %.2f\n", int(allCoins[k][0]), allCoins[k][1], allCoins[k][2]);
	fclose(setter);
	/************************************************/	

}
