#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <stdio.h>
#include <string.h>

using namespace cv;
using namespace std;

int main(){

	//Espaço de memória para a imagem captada da câmera
	Mat curr_frame;
	//Espaço para imagens adicionais que serão alteradas
	Mat color_frame[3];

	//Estrutura de captura de vídeo
	VideoCapture vcap;

	int deviceID = 0;				//Identificação da câmera
	int apiID = cv::CAP_ANY;		//Identificação da API

	vcap.set(CAP_PROP_FPS, 30);				//FPS = 30
	vcap.set(CAP_PROP_FRAME_WIDTH, 320);	//WIDTH = 320px
	vcap.set(CAP_PROP_FRAME_HEIGHT, 180);	//HIEGHT = 180px

	//Inicialização da câmera
	vcap.open(deviceID + apiID);

	//Testa se a câmera foi inicializada
	if (!cap.isOpened()) {
		cerr << "ERROR! Unable to open camera\n";
		return -1;
	}

	//int new_h;
	//int new_w;

	Mat paint[3] = Mat::zeros(CAP_PROP_FRAME_WIDTH, CAP_PROP_FRAME_HEIGHT, CV_8UC1);
	Mat imagem_comp = Mat::zeros(CAP_PROP_FRAME_WIDTH, CAP_PROP_FRAME_HEIGHT, CV_8UC1);


	//Canal de cor a ser usado como pincel
	int color_of_paint = 0;

	//valor do controle de luminosidade do canal de cor
	int light_control = 240;

	//Tempo que o texto será exibido
	int timer = 15;

	//Texto que será mostrado ao alterar a configuração de luminosidade
	String text = "";

	while(true){

		//Captura a imagem da câmera
		vcap.read(curr_frame);

		//Verifica se foi bem sucedida a captura da imagem
		if (curr_frame.empty()) {
			cerr << "ERROR! blank frame grabbed\n";
			break;
		}
		
		//Transfere o canal de cor definido em color_of_paint para o color_frame
		cv::split(curr_frame, color_frame);

		cv::compare(color_frame[color_of_paint], light_control, paint[color_of_paint], CMP_GT);
		cv::compare(color_frame[color_of_paint], light_control+20, imagem_comp, CMP_LT);
		cv::compare(imagem_comp, paint[color_of_paint], paint[color_of_paint], CMP_EQ);

		cv::bitwise_or(color_frame[color_of_paint], paint[color_of_paint]);


	}

}
