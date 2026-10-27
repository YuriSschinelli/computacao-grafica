import numpy as np
import matplotlib.pyplot as plt

# Definição dos pontos originais da casa
po = np.array([[1,1,1,1],[3,1,1,1],[3,3,1,1],[2,4,1,1],[1,3,1,1], # house: front points
               [1,1,4,1],[3,1,4,1],[3,3,4,1],[2,4,4,1],[1,3,4,1], # house: back points
               [1.7,1,1,1],[1.7,2.3,1,1],[2.3,2.3,1,1],[2.3,1,1,1]]) # house: door points

pFront = np.array([po[0],po[1],po[2],po[3],po[4],po[0]])
pBack = np.array([po[5],po[6],po[7],po[8],po[9],po[5]])
pCeil = np.array([po[4],po[2],po[7],po[9],po[4]])
pFloor = np.array([po[0],po[1],po[6],po[5],po[0]])
pRoof = np.array([po[3],po[8]])
pDoor = np.array([po[10],po[11],po[12],po[13]])

pFront = np.transpose(pFront)
pBack = np.transpose(pBack)
pCeil = np.transpose(pCeil)
pFloor = np.transpose(pFloor)
pRoof = np.transpose(pRoof)
pDoor = np.transpose(pDoor)


end_loop = False
projection_plane = 'XY'
perspective_x = 0.
perspective_y = 0.
perspective_z = 0
scale_object = 1.
theta_x = 0
theta_y = 0

def on_key(event):
    global end_loop, projection_plane, perspective_x, perspective_y, perspective_z, scale_object, theta_x, theta_y

    if event.key == 'escape':  
        end_loop = True
        plt.close('all')
    elif event.key == '1':  
        projection_plane = 'XY'
    elif event.key == '2':  
        projection_plane = 'ZX'
    elif event.key == '3':  
        projection_plane = 'ZY'
    elif event.key == 'x':  
        perspective_x += 0.05
    elif event.key == 'X':  
        perspective_x -= 0.05
    elif event.key == 'y':  
        perspective_y += 0.05
    elif event.key == 'Y':  
        perspective_y -= 0.05
    elif event.key == 'z':  
        perspective_z += 0.05
    elif event.key == 'Z':  
        perspective_z -= 0.05
    elif event.key == 'up':
        theta_x += np.radians(5)
    elif event.key == 'down':  
        theta_x -= np.radians(5)
    elif event.key == 'left':     
        theta_y += np.radians(5)
    elif event.key == 'right':  
        theta_y -= np.radians(5)
        
def on_press(event):
    global scale_object
    if event.button == 1: # Botão esquerdo do mouse: diminui a escala
        scale_object = max(0.1, scale_object - 0.5)
    elif event.button == 3: # Botão direito do mouse: aumenta a escala
        scale_object += 0.5
            
def transform_points(pts, center, theta_x, theta_y, scale, px, py, pz, plane):
    # 1. Transladar para a origem
    pts_trans = pts.copy()
    pts_trans[0, :] -= center[0]
    pts_trans[1, :] -= center[1]
    pts_trans[2, :] -= center[2]
    
    # 2. Aplicar escala
    pts_scaled = pts_trans * scale
    pts_scaled[3, :] = 1.0 # manter coordenada homogênea
    
    # 3. Matrizes de rotação
    cos_x, sin_x = np.cos(theta_x), np.sin(theta_x)
    Rx = np.array([
        [1, 0, 0, 0],
        [0, cos_x, -sin_x, 0],
        [0, sin_x, cos_x, 0],
        [0, 0, 0, 1]
    ])
    
    cos_y, sin_y = np.cos(theta_y), np.sin(theta_y)
    Ry = np.array([
        [cos_y, 0, sin_y, 0],
        [0, 1, 0, 0],
        [-sin_y, 0, cos_y, 0],
        [0, 0, 0, 1]
    ])
    
    # Aplicar rotações
    pts_rot = Rx @ (Ry @ pts_scaled)
    
    # 4. Aplicar perspectiva simplificada
    x = pts_rot[0, :]
    y = pts_rot[1, :]
    z = pts_rot[2, :]
    
    factor_x = 1.0 + px * x
    factor_y = 1.0 + py * y
    factor_z = 1.0 + pz * z
    
    # Evitar divisão por zero
    factor_x[factor_x == 0] = 1e-5
    factor_y[factor_y == 0] = 1e-5
    factor_z[factor_z == 0] = 1e-5
    
    x_p = x / factor_x
    y_p = y / factor_y
    z_p = z / factor_z
    
    # 5. Selecionar plano de projeção
    if plane == 'XY':
        return x_p, y_p
    elif plane == 'ZX':
        return z_p, x_p
    elif plane == 'ZY':
        return z_p, y_p
    return x_p, y_p

def anima(pBack, pFront, pCeil, pFloor, pRoof, pDoor):
    global end_loop, projection_plane, perspective_x, perspective_y, perspective_z, scale_object, theta_x, theta_y

    # Encontrar o centro da casa com base nos pontos originais (ignorando a coordenada homogênea)
    center = np.mean(po[:, :3], axis=0)

    plt.close('all')    
    fig, ax = plt.subplots(figsize=(8, 8))
   
    # Registrar funções de callback
    fig.canvas.mpl_connect('key_press_event', on_key)
    fig.canvas.mpl_connect('button_press_event', on_press)
    
    parts = [pBack, pFront, pCeil, pFloor, pRoof, pDoor]
    colors = ['b', 'r', 'g', 'm', 'c', 'k']
    styles = ['-', '-', '-', '-', '--', '-']

    while not end_loop:
        ax.clear()
        if not plt.fignum_exists(fig.number):
            break
        # Desenhar cada parte da casa transformada
        for part, color, style in zip(parts, colors, styles):
            xp, yp = transform_points(part, center, theta_x, theta_y, scale_object, 
                                      perspective_x, perspective_y, perspective_z, projection_plane)
            if part.shape[1] == 2: # Caso do telhado (linha única entre 2 pontos)
                ax.plot(xp, yp, color=color, linestyle=style, linewidth=2)
            else:
                ax.plot(xp, yp, color=color, linestyle=style, marker='o', linewidth=2)

        # Configurações de limites e visualização
        ax.set_xlim(-10, 10)
        ax.set_ylim(-10, 10)
        ax.axhline(0, color='grey', linewidth=0.5, linestyle='--')
        ax.axvline(0, color='grey', linewidth=0.5, linestyle='--')
        ax.grid(True, linestyle=':', alpha=0.6)

        # Exibir informações solicitadas na figura (Plano, Ângulos e Perspectiva)
        info_text = (f"Plano: {projection_plane} | "
                     f"Theta X: {np.degrees(theta_x):.1f}° | "
                     f"Theta Y: {np.degrees(theta_y):.1f}°\n"
                     f"Perspectiva (x,y,z): ({perspective_x:.2f}, {perspective_y:.2f}, {perspective_z:.2f}) | "
                     f"Escala: {scale_object:.1f}x \n"
                     f"Pressione ESC para sair")
        ax.set_title(info_text, fontsize=10, fontweight='bold')

        plt.draw()
        plt.pause(0.01)

# Executar animação
anima(pBack, pFront, pCeil, pFloor, pRoof, pDoor)