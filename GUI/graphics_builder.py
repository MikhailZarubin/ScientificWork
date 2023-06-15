from operator import indexOf
import os.path as pth
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm
import sys

apiPath = '../api/'
dataPathsFileName = 'data_paths.txt'
nameContractFileName = 'name_contract.txt'
functionSuffix = '_function'
trialSuffix = '_trial'

class Point2D:
  def __init__(self, x, y):
        self.x = x
        self.y = y

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

def configureDataPaths():
  dataPaths = []

  dataPathsFile = open(apiPath + dataPathsFileName)
  dataPaths = dataPathsFile.read().split('\n')

  return dataPaths


def unparsePointsFromFile(fileName):
  pointsFile = open(fileName)
  pointsInfo = pointsFile.read().split('\n')
  points = []
  
  for point in pointsInfo:
       coordinates = point.split(' ')
       if coordinates.__len__() == 2:
          point2D = Point2D(coordinates[0], coordinates[1])
          points.append(point2D)
       if coordinates.__len__() == 3:
          point3D = Point3D(coordinates[0], coordinates[1], coordinates[2])
          points.append(point3D)

  return points

def convertPoints2DtoArrays(points2D):
  x = []
  y = []

  for point2D in points2D:
    x.append(point2D.x)
    y.append(point2D.y)
    
  x = np.array(x, dtype=float)
  y = np.array(y, dtype=float)

  return x, y

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
     print('Task number not recivied. Please pass only argument with the number of the task for which you want to build a graphic.')
     exit()
  
  functionPoints = []
  algorithmPoints = []
  optimumPoint = []
  expectedOptimum = []
  invalidPoints = []
  constraintFunctionPoints = []
  constraintTrialPoints = []

  try:
    pointsfileName = configurePointsFileName(sys.argv[1])
    dataPaths = configureDataPaths()

    algorithmPoints = unparsePointsFromFile(dataPaths[0] + pointsfileName)
    functionPoints = unparsePointsFromFile(dataPaths[1] + pointsfileName)
    invalidPoints = unparsePointsFromFile(dataPaths[2] + pointsfileName)

    constantNumber = 0
    while pth.isfile(dataPaths[3] + configurePointsFileName(sys.argv[1] + functionSuffix + str(constantNumber))) and pth.isfile(dataPaths[3] + configurePointsFileName(sys.argv[1] + trialSuffix + str(constantNumber))):
       constraintFunctionPoints.append(unparsePointsFromFile(dataPaths[3] + configurePointsFileName(sys.argv[1] + functionSuffix + str(constantNumber))))
       constraintTrialPoints.append(unparsePointsFromFile(dataPaths[3] + configurePointsFileName(sys.argv[1] + trialSuffix + str(constantNumber))))
       constantNumber = constantNumber + 1

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

  try:
    if functionPoints.__len__() > 0 and isinstance(functionPoints[0], Point2D):
      funcX, funcY = convertPoints2DtoArrays(functionPoints)
      algX, algY = convertPoints2DtoArrays(algorithmPoints)
      optX, optY = convertPoints2DtoArrays(optimumPoint)
      expectedOptX, expectedOptY = convertPoints2DtoArrays(expectedOptimum)
      invalidX, invalidY = convertPoints2DtoArrays(invalidPoints)

      constraintFunctionPointsX = []
      constraintFunctionPointsY = []
      constraintTrialPointsX = []
      constraintTrialPointsY = []
      for i in range(len(constraintFunctionPoints)):
         constraintFunctionPointsArray = convertPoints2DtoArrays(constraintFunctionPoints[i])
         constraintTrialPointsArray = convertPoints2DtoArrays(constraintTrialPoints[i])
         constraintFunctionPointsX.append(constraintFunctionPointsArray[0])
         constraintFunctionPointsY.append(constraintFunctionPointsArray[1])
         constraintTrialPointsX.append(constraintTrialPointsArray[0])
         constraintTrialPointsY.append(constraintTrialPointsArray[1])

      plt.scatter(funcX, funcY, c = 'gray', s = 0.5) 
      plt.text(funcX.max(), funcY.max(),'f(x)')
      plt.scatter(invalidX, invalidY, c = 'blue', alpha=0.3, s = 0.5)
      plt.scatter(algX, algY, c = 'red', s =5)
      plt.scatter(optX, optY, c = 'yellow', s = 50, alpha=0.5)
      plt.scatter(expectedOptX, expectedOptY, c = 'green', s = 50, alpha=0.5)

      for i in range(len(constraintFunctionPoints)):
          plt.scatter(constraintFunctionPointsX[i], constraintFunctionPointsY[i], c = 'tan', s = 0.5) 
          plt.scatter(constraintTrialPointsX[i], constraintTrialPointsY[i], c = 'red', s = 5)
          maxConstraintFunctionPointX = constraintFunctionPointsX[i].max()
          plt.text(maxConstraintFunctionPointX, constraintFunctionPointsY[i][indexOf(constraintFunctionPointsX[i],maxConstraintFunctionPointX)], 'g' + str(i) + '(x)')
      if (len(constraintFunctionPoints) > 0):
        plt.plot([funcX.min(), funcX.max(), None], [0, 0, None], 'b--', c = 'pink', label='y=0')
        plt.text(funcX.max(), 0, 'y=0')

      plt.show()
    elif functionPoints.__len__() > 0 and isinstance(functionPoints[0], Point3D):
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
    else: 
      raise IndexError()
  except IndexError:
    print('Non supported dimension of task. Please verify that you input the number of the two-dimensional or three-dimensional task.')
    exit()
