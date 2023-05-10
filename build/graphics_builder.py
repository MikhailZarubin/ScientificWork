import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm

functionPointsPath = '../data/function_points/'
algorithmPointsPath = '../data/algorithm_points/'
invalidPointsPath = '../data/invalid_points/'

class Point3D:
  def __init__(self, x, y, z):
        self.x = x
        self.y = y
        self.z = z

def unparse3DPointsFromFile(fileName):
  pointsFile = open(fileName)
  pointsInfo = pointsFile.read().split('\n')
  points = []
  
  for point in pointsInfo:
       coordinates = point.split(' ')
       if coordinates.__len__() == 3:
             point3D = Point3D(coordinates[0], coordinates[1], coordinates[2])
             points.append(point3D)

  return points

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
  pointsfileName = 'points_55.txt'
  functionPoints = []
  algorithmPoints = []
  optimumPoint = []
  expectedOptimum = []
  invalidPoints = []
  try:
    functionPoints = unparse3DPointsFromFile(functionPointsPath + pointsfileName)
    algorithmPoints = unparse3DPointsFromFile(algorithmPointsPath + pointsfileName)
    invalidPoints = unparse3DPointsFromFile(invalidPointsPath + pointsfileName)

    expectedOptimum.append(algorithmPoints[algorithmPoints.__len__() - 1])
    optimumPoint.append(algorithmPoints[algorithmPoints.__len__() - 2])

    algorithmPoints.remove(expectedOptimum[0])
    algorithmPoints.remove(optimumPoint[0])
  except FileNotFoundError:
    print('Non existing file with name', pointsfileName)

  xgrid, ygrid, zgrid = convertPoints3Dto2DArrays(functionPoints)
  algX, algY, algZ = convertPoints3DtoArrays(algorithmPoints)
  optX, optY, optZ = convertPoints3DtoArrays(optimumPoint)
  expectedOptX, expectedOptY, expectedOptZ = convertPoints3DtoArrays(expectedOptimum)
  invalidX, invalidY, invalidZ = convertPoints3DtoArrays(invalidPoints)

  plt.contour(xgrid, ygrid, zgrid, cmap = cm.cool) 
  plt.scatter(invalidX, invalidY, c = 'blue', alpha=0.3)
  plt.scatter(algX, algY, c = 'red', s = 1)
  plt.scatter(optX, optY, c = 'yellow', s = 50, alpha=0.5)
  plt.scatter(expectedOptX, expectedOptY, c = 'green', s = 50, alpha=0.5)

  plt.show()
