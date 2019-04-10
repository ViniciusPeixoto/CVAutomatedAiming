import numpy as np

def non_max_suppression(boxes, weights, overlap_threshold=0.3):
    '''Elimina caixas possuindo alta intersecção. Quando duas ou mais caixas
       possuem intersecção maior que overlap_threshold, a caixa possuindo maior
       valor de weights é mantida. Um maior valor de weights indica uma maior 
       chance da caixa conter a imagem de uma pessoa.'''
    
    # Calcula área de cada caixa
    num_boxes = len(boxes)
    box_areas = np.zeros(num_boxes)
    for box_index, box in enumerate(boxes):
        x1 = box[0]
        y1 = box[1]
        x2 = box[2]
        y2 = box[3]    
        box_areas[box_index] = (x2 - x1)*(y2 - y1)

    # Ordena as caixas e as áreas em ordem decrescente do respectivo
    # valor de weights. Isto é, boxes[0] e box_areas[0] passam a corresponder
    # à caixa possuindo maior chance de possuir uma pessoa.
    index_for_sorting = np.argsort(weights)[::-1]
    boxes = boxes[index_for_sorting]
    box_areas = box_areas[index_for_sorting]
    
    # Converte arrays numpy para listas, pois iremos remover elementos
    boxes = list(boxes)
    box_areas = list(box_areas)
    
    # Armazena caixas a serem mantidas
    picked_boxes = []
    while len(boxes) > 0:

        # Elimina primeira caixa da lista, e salva na lista final
        best_box = boxes.pop(0)
        area = box_areas.pop(0)
        picked_boxes.append(best_box)        
        
        # Calcula área de intersecção entre a caixa a ser mantida e
        # as demais caixas
        overlap_area = np.zeros(len(boxes))
        for box_index, box in enumerate(boxes):
           
            # Coordenadas do quadrado de intersecção entre as duas caixas
            x1_overlap = np.maximum(best_box[0], box[0])
            y1_overlap = np.maximum(best_box[1], box[1])
            x2_overlap = np.minimum(best_box[2], box[2])
            y2_overlap = np.minimum(best_box[3], box[3])

            # Se não houver intersecção, x2_overlap - x1_overlap será
            # menor que 0, o mesmo ocorre para a coordenada y
            width_overlap = np.maximum(0, x2_overlap - x1_overlap)
            height_overlap = np.maximum(0, y2_overlap - y1_overlap)

            overlap = (width_overlap*height_overlap)/area
            overlap_area[box_index] = overlap

        # Armazena índices das caixas a serem removidas
        boxes2rem = []
        for box_index, overlap in enumerate(overlap_area):
            if overlap > overlap_threshold:
                boxes2rem.append(box_index)
                
        # Elimina caixas
        for box_index in boxes2rem[::-1]:
            boxes.pop(box_index)
            box_areas.pop(box_index)

    picked_boxes = np.array(picked_boxes)
    
    return picked_boxes