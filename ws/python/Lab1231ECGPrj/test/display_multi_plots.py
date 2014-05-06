import matplotlib.pyplot as plt
f1 = plt.figure()
f2 = plt.figure()
ax1 = f1.add_subplot(111)
ax1.plot(range(0,10))
ax2 = f2.add_subplot(111)
ax2.plot(range(10,20))
plt.show()
