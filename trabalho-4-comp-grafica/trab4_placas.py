import numpy as np
import matplotlib.pyplot as plt
from skimage.io import imread

def calcular_homografia(pts_src, pts_dst):
    """
    Calcula a matriz de homografia 3x3 usando o algoritmo DLT (Direct Linear Transformation).
    pts_src: 4 pontos na imagem original (x, y)
    pts_dst: 4 pontos correspondentes na imagem de destino (u, v)
    """
    A = []
    for (x, y), (u, v) in zip(pts_src, pts_dst):
        A.append([-x, -y, -1, 0, 0, 0, u * x, u * y, u])
        A.append([0, 0, 0, -x, -y, -1, v * x, v * y, v])
    
    A = np.array(A, dtype=float)
    # SVD para resolver Ah = 0
    _, _, Vt = np.linalg.svd(A)
    H = Vt[-1].reshape(3, 3)
    # Normalizar para H[2, 2] = 1
    H = H / H[2, 2]
    return H

def interpolacao_bilinear(img, x, y):
    """
    Realiza a interpolação bilinear para coordenadas (x, y) na imagem de origem `img`.
    Funciona para imagens 2D (grayscale) ou 3D (RGB).
    """
    H, W = img.shape[:2]
    
    # Arredondar para os 4 vizinhos inteiros
    x0 = np.floor(x).astype(int)
    x1 = x0 + 1
    y0 = np.floor(y).astype(int)
    y1 = y0 + 1
    
    # Máscara para pontos dentro dos limites da imagem
    dentro = (x0 >= 0) & (x1 < W) & (y0 >= 0) & (y1 < H)
    
    # Coordenadas seguras para evitar IndexError temporário
    x0_s = np.clip(x0, 0, W - 1)
    x1_s = np.clip(x1, 0, W - 1)
    y0_s = np.clip(y0, 0, H - 1)
    y1_s = np.clip(y1, 0, H - 1)
    
    dx = x - x0
    dy = y - y0
    
    # Pesos
    w00 = (1.0 - dx) * (1.0 - dy)
    w10 = dx * (1.0 - dy)
    w01 = (1.0 - dx) * dy
    w11 = dx * dy
    
    if img.ndim == 3:
        # Imagem colorida (RGB)
        res = np.zeros((*x.shape, img.shape[2]), dtype=img.dtype)
        for c in range(img.shape[2]):
            I = img[:, :, c]
            val = (w00 * I[y0_s, x0_s] + 
                   w10 * I[y0_s, x1_s] + 
                   w01 * I[y1_s, x0_s] + 
                   w11 * I[y1_s, x1_s])
            res[..., c] = np.where(dentro, val, 0)
        return res
    else:
        val = (w00 * img[y0_s, x0_s] + 
               w10 * img[y0_s, x1_s] + 
               w01 * img[y1_s, x0_s] + 
               w11 * img[y1_s, x1_s])
        return np.where(dentro, val, 0)

def reconstruir_imagem(altura_px, largura_px, caminho_imagem):
    """
    Reconstrói a imagem removendo distorções perspectivas.
    
    Parâmetros:
    - altura_px: Altura da imagem reconstruída em pixels.
    - largura_px: Largura da imagem reconstruída em pixels.
    - caminho_imagem: Caminho do arquivo da fotografia digital.
    """
    # 1. Carregar a imagem
    img_src = imread(caminho_imagem)
    
    # 2. Exibir a imagem e coletar 4 cliques do usuário
    fig, ax = plt.subplots(figsize=(8, 8))
    ax.imshow(img_src)
    ax.set_title("Clique nos 4 pontos na ordem:\n1. Inf. Esquerdo | 2. Inf. Direito | 3. Sup. Direito | 4. Sup. Esquerdo")
    
    # ginput aguarda 4 cliques do usuário
    pts_clicks = plt.ginput(4, timeout=0)
    plt.close(fig)
    
    pts_src = np.array(pts_clicks, dtype=float) # formato (x, y)
    
    # 3. Definir os pontos de destino na imagem retificada
    # Ordem correspondente:
    # 1. Canto inferior esquerdo: (0, altura_px - 1)
    # 2. Canto inferior direito: (largura_px - 1, altura_px - 1)
    # 3. Canto superior direito: (largura_px - 1, 0)
    # 4. Canto superior esquerdo: (0, 0)
    pts_dst = np.array([
        [0, altura_px - 1],
        [largura_px - 1, altura_px - 1],
        [largura_px - 1, 0],
        [0, 0]
    ], dtype=float)
    
    # 4. Calcular a matriz de homografia inversa (Mapeamento Reverso: Destino -> Origem)
    # Mapeamos de dst para src para preencher cada pixel da imagem de saída sem buracos
    H = calcular_homografia(pts_dst, pts_src)
    
    # 5. Criar grade de coordenadas para a imagem de destino
    uu, vv = np.meshgrid(np.arange(largura_px), np.arange(altura_px))
    ones = np.ones_like(uu)
    
    # Coordenadas homogêneas de destino empilhadas
    coords_dst = np.stack([uu.ravel(), vv.ravel(), ones.ravel()], axis=0) # Forma: (3, N)
    
    # Aplicar homografia para encontrar as coordenadas correspondentes na origem
    coords_src_hom = H @ coords_dst
    
    # Converter de homogêneas para cartesianas (x, y)
    x_src = coords_src_hom[0, :] / coords_src_hom[2, :]
    y_src = coords_src_hom[1, :] / coords_src_hom[2, :]
    
    x_src = x_src.reshape((altura_px, largura_px))
    y_src = y_src.reshape((altura_px, largura_px))
    
    # 6. Aplicar interpolação bilinear para obter os canais de cor
    img_reconstruida = interpolacao_bilinear(img_src, x_src, y_src)
    
    # 7. Exibir a nova figura com a placa reconstruída
    fig_rec, ax_rec = plt.subplots(figsize=(6, 6))
    if img_reconstruida.ndim == 3:
        ax_rec.imshow(img_reconstruida.astype(np.uint8))
    else:
        ax_rec.imshow(img_reconstruida.astype(np.uint8), cmap='gray')
    ax_rec.set_title("Placa Reconstruída (Sem Distorção Perspectiva)")
    ax_rec.axis('off')
    plt.show()
    
    return img_reconstruida.astype(np.uint8)

placa_estacionamento = reconstruir_imagem(altura_px=462, largura_px=662, caminho_imagem="Placa_Estacionamento_Marcas.png")
placa_pare = reconstruir_imagem(altura_px=615, largura_px=300, caminho_imagem="Placa_Pare_Marcas.png")