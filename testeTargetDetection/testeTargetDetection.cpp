#include <opencv2/opencv_modules.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

Scalar cor = (255);

int main() {

	// Setup SimpleBlobDetector parameters.
	SimpleBlobDetector::Params params;

	// Change thresholds
	params.minThreshold = 10;
	params.maxThreshold = 100;
	params.thresholdStep = 5;

	//Filter by Color
	params.filterByColor = true;
	//params.blobColor = 255;
	params.blobColor = 0;

	// Filter by Area.
	params.filterByArea = true;
	params.minArea = 200;  //precisa ser ajustável
	params.maxArea = 900000;

	// Filter by Circularity
	params.filterByCircularity = true;
	params.minCircularity = 0.01;   //encontra problemas se for muito alta e o círculo for pequeno
	params.maxCircularity = 1;

	// Filter by Convexity
	params.filterByConvexity = true;
	params.minConvexity = 0.01;
	params.maxConvexity = 1;

	// Filter by Inertia
	params.filterByInertia = true;
	params.minInertiaRatio = 0.8;  //pode ser alta para círculos
	params.maxInertiaRatio = 1;

	// Set up the detector with default parameters.
	Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);

	cout << "Diametro minimo: " << sqrt(params.minArea * 4 / (float)3.1415926535) << endl;

	// Detect blobs.
	vector<KeyPoint> keypoints;
	vector<Mat> possiveisAlvos;

	vector<Mat> for_match;
	Mat imagem_capturada;
	Mat weird, view, result;
	String nome_arquivo;
	vector<String> nome_alvo;

	nome_alvo.push_back("official_10m_Air_Pistol_Target");
	nome_alvo.push_back("official_10m_Air_Rifle_Target");
	nome_alvo.push_back("official_25m_50m_Precision_Pistol_Target");
	nome_alvo.push_back("official_25m_Rapid_Fire_Pistol_Target");
	nome_alvo.push_back("official_50m_Rifle_Target");
	nome_alvo.push_back("official_300m_Rifle_Target");

	for (int i = 0; i < nome_alvo.size(); i++) {
			for_match.push_back(imread("modelos/"+nome_alvo[i]+".jpg", IMREAD_COLOR));
		}

	cout << "Entre com o nome do arquivo: ";
	cin >> nome_arquivo;
	
	imagem_capturada = imread(nome_arquivo+".jpg", IMREAD_COLOR | IMREAD_ANYDEPTH);

	if (imagem_capturada.empty()) {
		cout << "Imagem nao encontrada" << endl;
		return -1;
	}
	if (for_match.empty()) {
		cout << "Modelos nao encontrados" << endl;
		return -1;
	}

	cvtColor(imagem_capturada, imagem_capturada, COLOR_BGR2RGB);

	for (int i = 0; i < for_match.size(); i++) {
		cvtColor(for_match[i], for_match[i], COLOR_BGR2RGB);
	}

	cout << "Detectando blobs pretos..." << endl;
	detector->detect(imagem_capturada, keypoints);

	for (int i = 0; i < keypoints.size(); i++) {
		cout << "pt:" << keypoints[i].pt << " sz:" << keypoints[i].size << endl;
	}
	cout << endl;


	weird = imagem_capturada;

	/*
	cout << "Desenhando circulos..." << endl;
	for (int i = 0; i < keypoints.size(); i++) {
		circle(weird, keypoints[i].pt, (int)keypoints[i].size/2, Scalar(255));
	}
	*/

	/*
	int winH = 800;
	int winW = 800;

	if (winH >= weird.rows) winH = weird.rows - 1;
	if (winW >= weird.cols) winW = weird.cols - 1;

	int scrolHeight = 0;
	int scrolWidth = 0;

	createTrackbar("H scroll", "Imagem Capturada", &scrolHeight, (weird.rows - winH));
	createTrackbar("W scroll", "Imagem Capturada", &scrolWidth, (weird.cols - winW));
	*/

	float topo_x, topo_y;
	float fundo_x, fundo_y;
	vector<float> ajuste;

	ajuste.push_back(0.77);
	ajuste.push_back(1.34);
	ajuste.push_back(0.80);
	ajuste.push_back(2.00);
	ajuste.push_back(1.46);
	ajuste.push_back(1.20);

	cout << "Separando alvos individuais..." << endl;
	for (int i = 0; i < keypoints.size(); i++) {
		for (int j = 0; j < ajuste.size(); j++) {

			topo_x = keypoints[i].pt.x - keypoints[i].size / ajuste[j];
			if (topo_x < 0) topo_x = 0;
			topo_y = keypoints[i].pt.y - keypoints[i].size / ajuste[j];
			if (topo_y < 0) topo_y = 0;
			fundo_x = keypoints[i].pt.x + keypoints[i].size / ajuste[j];
			if (fundo_x > weird.cols) fundo_x = weird.cols;
			fundo_y = keypoints[i].pt.y + keypoints[i].size / ajuste[j];
			if (fundo_y > weird.rows) fundo_y = weird.rows;

			possiveisAlvos.push_back(weird(Rect(Point2f(topo_x, topo_y), Point2f(fundo_x, fundo_y))));
		}
	}

	int indice = 0,  i_achou, alvo_id;
	vector<int> v_achou, v_id;

	if (possiveisAlvos.size() == 0) {
		cout << "Nao foram encontrados alvos" << endl;
		return -1;
	}

	vector<double> valor;
	double menor, achou = 1;
	//Point v_menor, v_achou;

	cout << "Buscando alvos..." << endl;
	while (indice != possiveisAlvos.size())
	{

		//cout << "Alvo estimado: " << nome_alvo[indice%6] << endl;
		//cout << endl;
		for (int i = 0; i < for_match.size(); i++) {

			//cout << "Comparando " << nome_alvo[i] << endl;

			resize(possiveisAlvos[indice], possiveisAlvos[indice], Size(for_match[i].cols, for_match[i].rows));

			matchTemplate(possiveisAlvos[indice], for_match[i], result, TM_SQDIFF_NORMED);

			minMaxLoc(result, &menor, NULL, NULL, NULL);


			if (menor <= achou) {
				cout << menor << " e menor que " << achou << endl;
				achou = menor;
				//v_achou = v_menor;
				//cout << "Sua posicao e " << v_achou << endl;
				i_achou = indice;
				alvo_id = i;
			}

		}

		if ((indice+1) % 6 == 0) {
			valor.push_back(achou);
			v_achou.push_back(i_achou);
			v_id.push_back(alvo_id);
			achou = 1;
		}
		
		//imshow("Verificacao de Alvo", view);
		indice++;
	}

	cout << "Tamanho do vetor de IDs: " << valor.size() << endl;

	for (int j = 0; j < v_id.size(); j++) {
		cout << "Alvo " << nome_alvo[v_id[j]] << " com " << (1 - valor[j]) * 100 << "% de chance." << endl;
		view = possiveisAlvos[v_achou[j]];
		imshow("Imagem Capturada", view);
		waitKey(0);
	}
	

	waitKey(0);

	cout << "Alvos encontrados..." << endl;
	for (int i = 0; i < possiveisAlvos.size(); i++) {
		cout << nome_alvo[i % 6] << endl;
		imshow("Imagem Capturada", possiveisAlvos[i]);
		if (waitKey(0) == 27) {
			break;
		}
	}
		

	return 0;


}