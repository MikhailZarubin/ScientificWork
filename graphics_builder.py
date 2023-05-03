import numpy as np
import tkinter as tk
import matplotlib.pyplot as plt

class Point3D:
  def __init__(self, x, y, z):
        self.x = x
        self.y = y
        self.z = z

if __name__ == '__main__':
  fileName = 'data/point_test_0.txt'
  dimension = -1
  points = []
  try:
    pointsFile = open(fileName)

    pointInfo = pointsFile.read().split('\n')
    dimension = int(pointInfo[0])

    del pointInfo[0]

    for point in pointInfo:
       coordinates = point.split(' ')
       if coordinates.__len__() == dimension:
          if dimension == 3:
             point3D = Point3D(coordinates[0], coordinates[1], coordinates[2])
             points.append(point3D)
    
    pointsFile.close()
  except FileNotFoundError:
    print('Non existing file with name', fileName)

if dimension == 3:
  x = []
  y = []
  yDict = {}
  for point3D in points:
    if point3D.y not in yDict:
       yDict[point3D.y] = []
    extractZ = yDict[point3D.y]
    extractZ.append(point3D.z)
    yDict[point3D.y] = extractZ
    if point3D.x not in x:
      x.append(point3D.x)
    if point3D.y not in y:
       y.append(point3D.y)
  x = np.array(x, dtype=float)
  y = np.array(y, dtype=float)
  xgrid, ygrid = np.meshgrid(x, y)

  zgrid = ygrid.copy()
  for i in range(0, yDict.values().__len__()):
     zgrid[i] = np.array(list(yDict.values())[i], dtype=float)

  fig = plt.figure(figsize =(16, 9)) 
  ax = plt.axes(projection ='3d') 
  my_cmap = plt.get_cmap('cool') 
  surf = ax.plot_surface(xgrid, ygrid, zgrid, cmap = my_cmap, edgecolor ='none') 
  fig.colorbar(surf, ax = ax, shrink = 0.7, aspect = 7) 
  ax.set_title('Task of Global Optimization') 
  plt.show()
else:
  print('Not supported visualization of a task with dimension is', dimension)
