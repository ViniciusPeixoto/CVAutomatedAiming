#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <stdio.h>
#include <string.h>

using namespace cv;
using namespace std;

//Protótipo da função que adiciona texto na imagem
void checkTimer(Mat v_frame, int v_timer, String v_text);

int main(){

	//Espaço de memória para a imagem captada da câmera
	Mat curr_frame;
	//Espaço para imagens adicionais que serão alteradas
	Mat color_frame[3];
	Mat out_frame;

	//Estrutura de captura de vídeo
	VideoCapture vcap;

	int deviceID = 0;						//Identificação da câmera
	int apiID = CAP_ANY;		//Identificação da API

	vcap.set(CAP_PROP_FPS, 30);						//FPS = 30
	vcap.set(CAP_PROP_FRAME_WIDTH, 320);	//WIDTH = 320px
	vcap.set(CAP_PROP_FRAME_HEIGHT, 180);	//HIEGHT = 180px

	//Inicialização da câmera
	vcap.open(deviceID + apiID);

	//Testa se a câmera foi inicializada
	if (!vcap.isOpened()) {
		cerr << "ERROR! Unable to open camera\n";
		return -1;
	}

	int width = CAP_PROP_XI_WIDTH;
	int height = CAP_PROP_XI_HEIGHT;

	//Cria imagens que serão usadas para projetar o traço capturado em cada canal de cor
	Mat paint[3];
	for (int i = 0; i < 3; i++) {
		paint[i] = Mat::zeros(width, height, CV_8UC1);
	}
	Mat imagem_comp_a = Mat::zeros(width, height, CV_8UC1);
	Mat imagem_comp_b = Mat::zeros(width, height, CV_8UC1);


	//Canal de cor a ser usado como pincel
	int color_of_paint = 0;

	//valor do controle de luminosidade do canal de cor
	int light_control = 200;

	//Tempo que o texto será exibido
	int t_timer = 15;
	//Texto que será mostrado ao alterar a configuração de luminosidade
	String t_text = "";

	while(true){

		//Captura a imagem da câmera
		vcap.read(curr_frame);

		//Verifica se foi bem sucedida a captura da imagem
		if (curr_frame.empty()) {
			cerr << "ERROR! blank frame grabbed\n";
			break;
		}
		
		//Divide a imagem de 3 canais em 3 imagens de 1 canal para cada cor
		split(curr_frame, color_frame);

		//Compara o canal selecionado por color_of_paint com o valor de luminosidade definido por light_control
		//O resultado é uma imagem contendo o traço binário em um canal de cor
		compare(color_frame[color_of_paint], light_control, imagem_comp_a, CMP_GT);
		compare(color_frame[color_of_paint], light_control+20, imagem_comp_b, CMP_LT);
		compare(imagem_comp_a, imagem_comp_b, imagem_comp_a, CMP_EQ);

		//Usa a imagem do traço para sobrescrever o traço em cima da imagem capturada pela câmera
		//na cor do canal selecionado
		bitwise_or(imagem_comp_a, paint[color_of_paint], paint[color_of_paint]);
		bitwise_or(color_frame[color_of_paint], paint[color_of_paint], color_frame[color_of_paint]);

		//Une os canais de cores de volta em uma única imagem
		merge(color_frame, 3, out_frame);

		//Verifica se há necessidade de exibir informação do limite de luminosidade na tela
		checkTimer(out_frame, t_timer, t_text);

		//Exibe a imagem na tela
		//imshow("Paint-brush Camera", paint[0]);
		imshow("Paint-brush Camera", out_frame);

		//Implementar teste de teclado
		if (waitKey(5) >= 0){
			break;
		}
	}
	vcap.release();
	destroyAllWindows();
}

void checkTimer(Mat v_frame, int v_timer, String v_text) {

	//Só mostra texto na imagem se o tempo passado não for 0
	if (v_timer > 0) {
		//Escreve o v_text na imagem v_frame
		putText(v_frame, v_text, Point(10,50), FONT_HERSHEY_SIMPLEX, 1, Scalar(0,0,0), LINE_AA);
		//Decrementa o tempo de exibição
		v_timer--;
		if (v_timer < 0) {
			//O tempo não pode ser negativo
			v_timer = 0;
		}
	} else {
		//Se não há tempo para exibição, então mostra nada
		v_text = "";
		putText(v_frame, v_text, Point(10,50), FONT_HERSHEY_SIMPLEX, 1, Scalar(0,0,0), LINE_AA);
	}
}