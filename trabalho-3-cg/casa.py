import numpy as np
import matplotlib.pyplot as plt

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
perspective_z = 0.
scale_object = 1.
theta_y = 0
theta_x = 0

def on_key(event):
    global end_loop
    global projection_plane
    global perspective_x
    global perspective_y
    global perspective_z
    global scale_object
    global points
    global theta_x
    global theta_y

    if event.key == 'escape': 
        end_loop = True
    elif event.key == '0': 
        print('you pressed', event.key)
    elif event.key == '1': 
        print('you pressed', event.key)
    elif event.key == '2': 
        print('you pressed', event.key)
    elif event.key == '3': 
        print('you pressed', event.key)
    elif event.key == 'x': 
        print('you pressed', event.key)
    elif event.key == 'X': 
        print('you pressed', event.key)
    elif event.key == 'y': 
        print('you pressed', event.key)
    elif event.key == 'Y': 
        print('you pressed', event.key)
    elif event.key == 'z': 
        print('you pressed', event.key)
    elif event.key == 'Z': 
        print('you pressed', event.key)
    elif event.key == 'up':
        print('you pressed', event.key)
    elif event.key == 'down':  
        print('you pressed', event.key)
    elif event.key == 'left':     
        print('you pressed', event.key)
    elif event.key == 'right': 
        print('you pressed', event.key)
        
def on_press(event):
    global scale_object
    if event.button==1: #pressed LEFT button
        print('you pressed left mouse button', event.xdata, event.ydata)
    elif event.button==3: #pressed RIGHT button
        print('you pressed right mouse button', event.xdata, event.ydata)
            
def anima(pBack,pFront,pCeil,pFloor,pRoof,pDoor):

    # find house center

    # transformation to translate center of house to origin
    
    # closes all existing figures
    plt.close('all')    
    fig, ax = plt.subplots()
   
    # register callback functions
    cid = fig.canvas.mpl_connect('key_press_event', on_key)
    cid = fig.canvas.mpl_connect('button_press_event', on_press)
    
    while not end_loop:
        # assemble transformation matrixes
        
        # apply transformations to house points
        
        # plot transformed house points
            
        
        plt.show()
        plt.pause(0.01)

# execute animation
anima(pBack,pFront,pCeil,pFloor,pRoof,pDoor)
plt.close('all')    # Closes all existing figures        
