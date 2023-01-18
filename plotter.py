def read_data_from_file(filepath):
  column1 = []
  column2 = []
  column3 = []
  column4 = []
  with open(filepath, 'r') as f:
    for line in f:
      line_data = line.strip().split()
      if len(line_data) == 4:
        column1.append(line_data[0])
        column2.append(line_data[1])
        column3.append(line_data[2])
        column4.append(line_data[3])
  return column1, column2, column3, column4

filepath = '$YOUR_WORKING_DIR$/lorenz.dat'
column1, column2, column3, column4 = read_data_from_file(filepath)
column2 = [float(x) for x in column2]
column3 = [float(x) for x in column3]
column4 = [float(x) for x in column4]
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
# Assume x, y, and z are arrays of data
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.plot(column2, column3, column4)
plt.show()

