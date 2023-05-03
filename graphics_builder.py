import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm

functionPointsPath = 'data/function_points/'
algorithmPointsPath = 'data/algorithm_points/'

class Point3D:
  def __init__(self, x, y, z):
        self.x = x
        self.y = y
        self.z = z

def convertPoints3DtoArrays(points3D):
  x = []
  y = []
  z = []

  for point3D in points3D:
    x.append(point3D.x)
    y.append(point3D.y)
    z.append(point3D.z)
    
  x = np.array(x, dtype=float)
  y = np.array(y, dtype=float)
  z = np.array(z, dtype=float)

  return x, y, z

def convertPoints3Dto2DArrays(points3D):
  x = []
  y = []
  yDict = {}

  for point3D in points3D:
    if point3D.y not in yDict:
       yDict[point3D.y] = []
    yDict[point3D.y].append(point3D.z)
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

  return xgrid, ygrid, zgrid

if __name__ == '__main__':
  pointsfileName = 'points_2.txt'
  dimension = -1
  optimumPoint = []
  functionPoints = []
  algorithmPoints = []
  try:
    functionPointsFile = open(functionPointsPath + pointsfileName)
    algorithmPointsFile = open(algorithmPointsPath + pointsfileName)

    functionPointsInfo = functionPointsFile.read().split('\n')
    algorithmPointsInfo = algorithmPointsFile.read().split('\n')
    
    dimension = int(functionPointsInfo[0])
    del functionPointsInfo[0]

    for functionPoint in functionPointsInfo:
       coordinates = functionPoint.split(' ')
       if coordinates.__len__() == dimension:
          if dimension == 3:
             point3D = Point3D(coordinates[0], coordinates[1], coordinates[2])
             functionPoints.append(point3D)

  
    for algorithmPoint in algorithmPointsInfo:
       coordinates = algorithmPoint.split(' ')
       if coordinates.__len__() == dimension:
          if dimension == 3:
             point3D = Point3D(coordinates[0], coordinates[1], coordinates[2])
             algorithmPoints.append(point3D)

    optimumPoint.append(algorithmPoints[algorithmPoints.__len__() - 1])
    algorithmPoints.remove(algorithmPoints[algorithmPoints.__len__() - 1])
    
    functionPointsFile.close()
    algorithmPointsFile.close()
  except FileNotFoundError:
    print('Non existing file with name', functionPointsFile)

if dimension == 3:
  xgrid, ygrid, zgrid = convertPoints3Dto2DArrays(functionPoints)
  x, y, z = convertPoints3DtoArrays(algorithmPoints)
  optX, optY, optZ = convertPoints3DtoArrays(optimumPoint)

  ax = plt.axes(projection='3d')             
  
  ax.plot_surface(xgrid, ygrid, zgrid, cmap = cm.cool) 
  #ax.scatter(x, y, z, c = 'red')
  ax.scatter(optX, optY, optZ, c='green')

  ax.set_title('Task of Global Optimization') 

  ax.set_xlabel('X')
  ax.set_ylabel('Y')
  ax.set_zlabel('Z')

  plt.show()
else:
  print('Not supported visualization of a task with dimension is', dimension)
