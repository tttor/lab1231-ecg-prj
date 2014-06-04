#!/usr/bin/python

import numpy as np
import matplotlib.pyplot as plt

file_path = '/home/tor/ecg/lab1231-ecg-prj/dataset/mit-bih/10s/100_MLII_10s.csv'
    
def main():
    with open(file_path) as fp:
        y = []
        for line in fp:
            print line
            y.append(line)
        print y

def main2():
    y = [line.strip() for line in open(file_path)]
    plt.plot(y,'o')
    plt.show()

def main3():
    line_list = [line.strip() for line in open(file_path)]
    x = []
    y = []
    
    for line in line_list[2:]:# [0],[1] are column titles
        y.append(float(line.split(',')[1]))
        
        x_comp = line.split(',')[0]
        x_mm = int(x_comp.split(':')[0][1]) # [0] is an apostrophe
        x_ss = float(x_comp.split(':')[1][:-1]) # [-1] is an apostrophe 
        x.append(x_mm * 60. + x_ss)
    
    plt.plot(x,y,'-')
    plt.show()
    
if __name__ == '__main__':
    main3()
