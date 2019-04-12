//Inclusão de bibliotecas
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <stdio.h>
#include <string.h>

using namespace cv;
using namespace std;

//Protótipo das funções extra

//Verifica o tempo para adicionar texto na imagem
bool checkTimer(Mat v_frame, int &v_timer, String &v_text);
//Verifica qual tecla foi pressionada
bool checkKeyboard(Mat v_cf[], int &v_w, int &v_h, int &v_cop, VideoCapture v_vcap, int &v_lc, int &v_timer, String &v_text);

int main(){

	//Estrutura de captura de vídeo
	VideoCapture vcap;

	//Identificação da câmera
	int deviceID = 0;
	//Identificação da API
	int apiID = CAP_ANY;

	//Inicialização da câmera
	vcap.open(deviceID + apiID);
	//FPS = 30
	vcap.set(CAP_PROP_FPS, 30);	

	//Testa se a câmera foi inicializada
	if (!vcap.isOpened()) {
		cerr << "ERROR! Unable to open camera\n";
		return -1;
	}

	//Proporção da imagem na tela
	int width = 480;
	int height = 640;

	//Espaço de memória para a imagem captada da câmera
	Mat curr_frame;
	//Espaço para os canais de cores que serão alteradas
	Mat color_frame[3];
	//Espaço de memória para a imagem que será exibida
	Mat out_frame;

	//Cria imagens que serão usadas para projetar o traço capturado em cada canal de cor
	Mat paint[3];
	//São inicializadas zeradas para não haver nenhum risco inicial nas imagens
	for (int i = 0; i < 3; i++) {
		paint[i] = Mat::zeros(width, height, CV_8UC1);
	}

	//Imagens de comparação para criação de persistência na pintura da imagem
	Mat imagem_comp_a;
	Mat imagem_comp_b;

	//Canal de cor a ser usado como pincel
	int color_of_paint = 0;

	//valor do controle de luminosidade do canal de cor
	int light_control = 200;

	//Tempo que o texto será exibido
	int t_timer = 15;
	//Texto que será mostrado ao alterar a configuração de luminosidade
	String t_text = " ";

	while(true){

		//Aguarda a captura de uma imagem ao menos. Isso evita erros de inicialização da câmera
		while (!vcap.read(curr_frame));

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
		max(imagem_comp_a, paint[color_of_paint],paint[color_of_paint]);
		for (int i = 0; i < 3; i++) {
			max(color_frame[i], paint[i], color_frame[i]);
		}

		//Une os canais de cores de volta em uma única imagem
		merge(color_frame, 3, out_frame);

		//Verifica se há necessidade de exibir informação do limite de luminosidade na tela
		if (!checkTimer(out_frame, t_timer, t_text)) {
			cerr << "Bad frame" << endl;
			break;
		}

		//Exibe a imagem na tela
		imshow("Paint-brush Camera", out_frame);

		//Verifica se alguma tecla foi pressionada para executar uma funcionalidade
		if (!checkKeyboard(paint, width, height, color_of_paint, vcap, light_control, t_timer, t_text)) {
			break;
		}
	}
	//Libera a memória usada pelo dispositivo de captura
	vcap.release();
	//Libera a memória usada pela janela de exibição
	destroyAllWindows();

	return 0;
}

//Função para verificar se há algum texto para ser escrito na imagem
bool checkTimer(Mat v_frame, int &v_timer, String &v_text) {

	//Se não há frame, não há exibição
	if (v_frame.empty()) {
		return false;
	}

	//Só mostra texto na imagem se o tempo passado não for 0
	if (v_timer > 0) {
		//Escreve o v_text na imagem v_frame
		putText(v_frame, v_text, Point(10,50), FONT_HERSHEY_SIMPLEX, 1, Scalar(255,255,255), 1, LINE_AA);
		//Decrementa o tempo de exibição
		v_timer--;
		if (v_timer < 0) {
			//O tempo não pode ser negativo
			v_timer = 0;
		}
	} else {
		//Se não há tempo para exibição, então mostra nada
		v_text = "";
		putText(v_frame, v_text, Point(10,50), FONT_HERSHEY_SIMPLEX, 1, Scalar(255,255,255), 1, LINE_AA);
	}

	return true;
}

//Verifica se alguma tecla foi pressionada e realiza a função correspondente
bool checkKeyboard(Mat v_cf[], int &v_w, int &v_h, int &v_cop, VideoCapture v_vcap, int &v_lc, int &v_timer, String &v_text) {

	int key;
	key = waitKey(34);

	switch (key) {
		//A tecla ESC fecha o programa
		case 27:	return false;
		//A tecla ESPAÇO limpa todos os traços de todas as cores
		case 32:	for (int i = 0; i < 3; i++) {
						v_cf[i] = Mat::zeros(v_w, v_h, CV_8UC1);
					}
					v_text = "IMAGEM LIMPA";
					v_timer = 15;
					return true;
		//A tecla B seleciona o canal de cor AZUL
		case 98:	v_cop = 0;
					v_text = "Canal AZUL";
					v_timer = 15;
					return true;
		//A tecla G seleciona o canal de cor VERDE
		case 103:	v_cop = 1;
					v_text = "Canal VERDE";
					v_timer = 15;
					return true;
		//A tecla R seleciona o canal de cor VERMELHO
		case 114:	v_cop = 2;
					v_text = "Canal VERMELHO";
					v_timer = 15;
			return true;
		//A tecla 1 seleciona o dispositivo de captura padrão
		case 49:	v_vcap.open(0);
					if (!v_vcap.isOpened()) {
						cerr << "ERROR! Unable to open camera\n";
						return false;
					}
					v_text = "Dispositivo Padrao";
					v_timer = 15;
					break;
		//A tecla 2 seleciona o dispositivo de captura alternativo
		case 50:	v_vcap.open(1);
					if (!v_vcap.isOpened()) {
						cerr << "ERROR! Unable to open camera\n";
						return false;
					}
					v_text = "Dispositivo Alternativo";
					v_timer = 15;
					break;
		//A tecla O aumenta o valor de luminosidade
		case 111:	if (v_lc < 255) {
						v_lc = v_lc + 5;
						v_text = "LUM " + to_string(v_lc);
						v_timer = 15;
						return true;
					}break;
		//A tecla L diminui o valor de luminosidade
		case 108:	if (v_lc < 255) {
						v_lc = v_lc - 5;
						v_text = "LUM " + to_string(v_lc);
						v_timer = 15;
						return true;
					}break;
		//Teclas não listadas não fazem nada
		default:	return true;
	}
	
	return true;
}