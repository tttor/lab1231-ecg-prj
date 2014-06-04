#!/usr/bin/python

import matplotlib.pyplot as plt

def main():
    y =  [13, 11, 13, 12, 13, 10, 30, 12, 11, 13, 12, 12, 12, 11, 12]
    plt.plot(y)
    plt.ylim(ymax=35)
    plt.annotate('this spot must really\nmean something', xy=(6, 30), xytext=(8, 31.5), arrowprops=dict(facecolor='black',shrink=0.05))
    plt.show()
    
if __name__ == "__main__":
    main()
