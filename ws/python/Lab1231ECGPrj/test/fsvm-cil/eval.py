#!/usr/bin/env python
# a bar plot with errorbars

import sys
import numpy as np
import csv

import util

dir_path = 'out/perf-double-preprocessing/'
dbs = None
with open(dir_path + 'dataset_names.csv', 'rb') as csvfile:
    csv_reader = csv.reader(csvfile)
    for row in csv_reader:
        dbs= row

# 
mean_perfs = np.genfromtxt(dir_path + 'normalSVM_mean_perfs.csv', dtype=float, delimiter=',') 
std_perfs = np.genfromtxt(dir_path + 'normalSVM_std_perfs.csv', dtype=float, delimiter=',') 

# 
util.plot_bar(mean_perfs[:,0], std_perfs[:,0], 'Gmean', dbs, 'Normal SVM')


