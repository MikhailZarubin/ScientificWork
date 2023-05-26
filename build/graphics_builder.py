import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm
import sys

apiPath = '../api/'
pathsFileName = 'paths.txt'
nameContractFileName = 'name_contract.txt'

class Point3D:
  def __init__(self, x, y, z):
        self.x = x
        self.y = y
        self.z = z


def configurePointsFileName(numberPointsFile):
  nameContractFile = open(apiPath + nameContractFileName)
  nameContract = nameContractFile.read().split('{}')
    
  pointsfileName = nameContract[0] + numberPointsFile + nameContract[1]

  return pointsfileName

def configurePaths():
  paths = []

  pathsFile = open(apiPath + pathsFileName)
  paths = pathsFile.read().split('\n')

  return paths


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
  z = []

  yPositionDictionary = {}

  for point3D in points3D:
    if point3D.x not in x:
      x.append(point3D.x)
    if point3D.y not in y:
      y.append(point3D.y)
      yPositionDictionary[point3D.y] = y.__len__() - 1
      z.append([])
    z[yPositionDictionary[point3D.y]].append(point3D.z)

  x = np.array(x, dtype=float)
  y = np.array(y, dtype=float)
  x, y = np.meshgrid(x, y)
  z = np.array(z, dtype=float)
  
  return x, y, z


if __name__ == '__main__':
  if sys.argv.__len__() != 2:
     print('Task number received. Please pass only argument with the number of the task for which you want to build a graphic.')
     exit()
  
  functionPoints = []
  algorithmPoints = []
  optimumPoint = []
  expectedOptimum = []
  invalidPoints = []

  try:
    pointsfileName = configurePointsFileName(sys.argv[1])
    paths = configurePaths()

    algorithmPoints = unparse3DPointsFromFile(paths[0] + pointsfileName)
    functionPoints = unparse3DPointsFromFile(paths[1] + pointsfileName)
    invalidPoints = unparse3DPointsFromFile(paths[2] + pointsfileName)

    expectedOptimum.append(algorithmPoints[algorithmPoints.__len__() - 1])
    optimumPoint.append(algorithmPoints[algorithmPoints.__len__() - 2])

    algorithmPoints.remove(expectedOptimum[0])
    algorithmPoints.remove(optimumPoint[0])
  except FileNotFoundError:
    print('Non existing file with name', pointsfileName)
    exit()
  except IndexError:
    print('Api changed! The program cannot work')
    exit()

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
