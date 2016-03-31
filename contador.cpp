/***********************************************/
/************* CONTADOR DE MOEDAS **************/
/***         **********************          ***/
/***   Contador de objetos general-purpose   ***/
/*** modificado para identificacao de moedas ***/
/***         e contagem de dinheiro.         ***/
/***********************************************/

#include "CoinCounter.hpp"

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

	time_t camAdaptationStartTime = time(NULL);
	bool camAdapted = false;

	vector<int> coinCount(coinNumbers.size(), 0); /* inicializa contador de moedas individual */
	
	//Configura janela do CoinCounter
	CoinCounter* contador = new CoinCounter("Contador de Moedas");
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
			contador->LeMoedas(orig, frame);
			vector<coins_t> coinsGot = contador->getCoins();

			for(int i=0; i < coinCount.size(); i++) //Limpa contador individual de moedas
				coinCount[i] = 0;

			double totalMoney = 0;
			for(int i=0; i < coinsGot.size(); i++){ //Conta moedas e soma dinheiro
				int coinID = contador->coinIdentifier(coinAreas, coinsGot[i]);
				coinCount[coinID]++;
				totalMoney += contador->coinValue( coinNumbers, coinID);

				char buffCoin[11]; //Printa o valor da moeda em cima desta
				if(coinID >= 0 && coinID < coinCount.size()-1) sprintf(buffCoin, "%02dc", coinNumbers[coinID]);
				else if(coinID == coinCount.size()-1) sprintf(buffCoin, "R$1");
				else sprintf(buffCoin, "???");
				putText(orig, buffCoin, CenterCoin(coinsGot[i]), FONT_HERSHEY_PLAIN, 1,Scalar(255,255,0), 4);
			}

			//Printa qtde de moedas individuais na tela
			for(int i=0; i < coinCount.size()-1; i++){
				char buffCoin[11];
				sprintf(buffCoin, "%02dc: %d", coinNumbers[i], coinCount[i]);
				putText(orig,buffCoin,Point(10, 20 + i*20), FONT_HERSHEY_PLAIN, 1,Scalar(255,255,255), 4);
			}
			char buffCoin[11];
			sprintf(buffCoin, "R$1: %d", coinCount[4]);
			putText(orig, buffCoin, Point(10, 20 + 4*20), FONT_HERSHEY_PLAIN, 1,Scalar(255,255,255), 4);

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
		}	/**********************************************/

	}
}
