import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

if __name__ == '__main__':
  fileName = 'data/point_0.txt'
  dimension = -1
  points = []
  try:
    pointsFile = open(fileName)

    pointInfo = pointsFile.read().split('\n')
    dimension = int(pointInfo[0])

    del pointInfo[0]

    for i in range(0, dimension):
      points.append([])

    for point in pointInfo:
       coordinates = point.split(' ')
       if coordinates.__len__() == dimension:
          coordinateNumber = 0
          for coordinate in coordinates:
            points[coordinateNumber].append(coordinate)
            coordinateNumber+=1
    
    pointsFile.close()
  except FileNotFoundError:
    print('Non existing file with name', fileName)

  if dimension == 2:
    x = np.array(points[0], dtype=float)
    y = np.array(points[1], dtype=float)
    fig = plt.figure()
    ax = fig.add_subplot(11, projection='2d')
    ax.scatter(x, y)
    plt.savefig('data/graphic_2D.pdf')
    plt.show()
  elif dimension == 3:
    x = np.array(points[0], dtype=float)
    y = np.array(points[1], dtype=float)
    z = np.array(points[2], dtype=float)
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    ax.scatter(x, y, z)
    plt.savefig('data/graphic_3D.pdf')
    plt.show()
  else:
    print('Not supported visualization of a task with dimension is', dimension)
