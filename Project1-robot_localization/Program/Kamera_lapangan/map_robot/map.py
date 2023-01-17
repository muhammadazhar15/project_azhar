import numpy as np
import matplotlib.pyplot as plt

datax = np.load('datax.npy')
datay = np.load('datay.npy')

plt.figure()
plt.plot(datax, datay, 'r--')
plt.show()
