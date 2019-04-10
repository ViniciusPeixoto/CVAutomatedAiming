
# coding: utf-8

# In[1]:


import numpy as np
import cv2
import util


# In[16]:


# Define qual a fonte de captura da imagem
# 0: Webcam integrada
# 1: Webcam USB
vcap = cv2.VideoCapture(1)

# Obtenção de alguns parâmetros da fonte de captura de imagem

# FPS (frames per second): taxa de captura de imagens por segundo da fonte
fps = vcap.get(cv2.CAP_PROP_FPS)
# Width: tamanho em pixels da largura da imagem
w = int(vcap.get(cv2.CAP_PROP_FRAME_WIDTH))
# Height: tamanho em pixels da altura da imagem
h = int(vcap.get(cv2.CAP_PROP_FRAME_HEIGHT))

# Redução do tamanho da imagem do video para tratamento mais rápido
new_h = 180
new_w = 320

# Caso queira criar um arquivo de video
#codec = cv2.VideoWriter_fourcc(*'H264')
#video_writer = cv2.VideoWriter('testeT4.mp4', codec, fps, (w,h))

# Definição de imagens que guardarão os traços coloridos a serem escritos no vídeo
# Traços vermelhos serão salvos aqui
imagem_r = np.zeros([new_h, new_w])
# Traços verdes serão salvos aqui
imagem_g = np.zeros([new_h, new_w])
# Traços azuis serão salvos aqui
imagem_b = np.zeros([new_h, new_w])

# Define qual é o canal de cores que estará sendo usado para manipulação
# bem como qual a cor do traço que será exibido no vídeo
# 0 : Blue
# 1 : Green
# 2 : Red
color_of_paint = 0

# Controle do nível do brilho que será detectado para criar os desenhos
# Valores baixos demais geram ruídos causados por brilhos de cores que não são
# a desejada, enquanto valores altos demais falham em detectar objetos de cores
# corretas.
light_control = 240

# Texto que irá ficar um determinado número de frames indicando qual é o controle
# ce brilho atual
text = ""
timer = 15
        
while True:
    # curr_frame terá o frame que está sendo captado pela fonte neste momento
    ret, curr_frame = vcap.read()
    
    # Verifica se foi possível pegar um frame
    if ret == True:
        # Redimensiona o frame para que o trabalho seja mais rápido
        resized_frame = cv2.resize(curr_frame, (new_w, new_h))
        
        # Retira o canal da cor desejada para ser trabalhado
        # Conforme o código, o padrão é o canal Azul
        color_frame = resized_frame[:,:,color_of_paint]
        
        # Verifica cada pixel do frame do canal da cor desejada
        for row in range(new_h):
            for col in range(new_w):
                # Se o valor do pixel estiver dentro do limite de luminosidade definido
                if color_frame[row, col] > light_control and color_frame[row, col] < (light_control + 20):
                    # então o traço é desenhado nas imagens que guardam esses traços, respectivas ao
                    # canal de cor selecionado
                    if color_of_paint == 0:
                        imagem_b[row, col] = 255
                    elif color_of_paint == 1:
                        imagem_g[row, col] = 255
                    elif color_of_paint == 2:
                        imagem_r[row, col] = 255
                # Se um valor na imagem de traços é definido, então esse traço é colocado na imagem a ser exibida,
                # respeitando o canal de cor selecionado
                # Isso irá criar um traço da mesma cor do canal selecionado
                if imagem_b[row, col] == 255:
                    resized_frame[row, col, 0] = 180
                if imagem_g[row, col] == 255:
                    resized_frame[row, col, 1] = 180
                if imagem_r[row, col] == 255:
                    resized_frame[row, col, 2] = 180
        
        # A imagem é redimensionada novamente, agora para ficar mais visível, uma vez que não há mais trabalho
        # a ser feito
        new_resized_frame = cv2.resize(resized_frame, (w, h))
        
        # Verifica se há necessidade de colocar texto na tela
        if timer > 0:
            # O texto é mostrado durante alguns frames
            cv2.putText(new_resized_frame, text, (10,50), cv2.FONT_HERSHEY_SIMPLEX, 1, (0,0,0), 2, cv2.LINE_AA)
            timer = timer - 1
            if timer < 0:
                timer = 0
        else:
            text = ""
            cv2.putText(new_resized_frame, text, (10,50), cv2.FONT_HERSHEY_SIMPLEX, 1, (0,0,0), 2, cv2.LINE_AA)
        
        # Exibe o frame já com as alterações e traços coloridos
        cv2.imshow('frame', new_resized_frame)
        
        # Comando suporte para teste de obtenção de cor correspondente
        #cv2.imshow('color frame', color_frame)
        
        # Caso quiser criar o arquivo de video, este comando escreve no arquivo de video
        #video_writer.write(new_resized_frame)
    
    # Cria condições de parada e manutenção do video exibido através do teclado
    key = cv2.waitKey(34) & 0xFF
    # A tecla "ESC" interrompe a exibição e finaliza todo o processo
    if key == 27:
        break
    # A tecla "ESPAÇO" limpa todos os desenhos feitos, de todas as cores
    elif key == 32:
        imagem_r = np.zeros([h, w])
        imagem_g = np.zeros([h, w])
        imagem_b = np.zeros([h, w])
    # A tecla "B" seleciona o canal de cor AZUL
    elif key == 98:
        color_of_paint = 0
    # A tecla "g" seleciona o canal de cor VERDE
    elif key == 103:
        color_of_paint = 1
    # A tecla "G" seleciona o canal de cor VERMELHO
    elif key == 114:
        color_of_paint = 2
    # A tecla "1" seleciona como fonte de captura a webcam integrada
    elif key == 49:
        vcap = cv2.VideoCapture(0)
    # A tecla "2" seleciona como fonte de captura a webcam USB
    elif key == 50:
        vcap = cv2.VideoCapture(1)
    # A tecla "O" incrementa o valor limitante de brilho, dificultando a detecção e reduzindo ruídos
    elif key == 111:
        if light_control < 255:
            light_control = light_control + 5
            # O valor do brilho atual é exibido na tela como texto durante alguns frames
            text = str(light_control)
            timer = 15
    # A tecla "L" decrementa o valor limitante de brilho, facilitando a detecção e aumentando ruídos
    elif key == 108:
        if light_control > 0:
            light_control = light_control - 5
            # O valor do brilho atual é exibido na tela como texto durante alguns frames
            text = str(light_control)
            timer = 15
        
# Ao final da execução, libera a memória do video
vcap.release()
# Caso tenha feito um arquivo de vídeo, libera a memoria do escritor do arquivo
#video_writer.release()
# Libera e destrói a memória dedicada a exibição do vídeo
cv2.destroyAllWindows()

