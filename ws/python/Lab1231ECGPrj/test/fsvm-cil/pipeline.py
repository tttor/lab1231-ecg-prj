import sys
import os
import numpy as np
import matplotlib.pyplot as plt
import csv

import loader
import normalSVM 

# Load
dataset_names = ['glass', 'pima-indians-diabetes', 'haberman', 'page-blocks', 'abalone', 'yeast', 'transfusion']
#dataset_names = ['abalone', 'yeast']
datasets = [loader.load(e) for e in dataset_names]

# Init perf, column => gmean, SE, SP
mean_perfs = np.zeros((len(datasets),3))
std_perfs = np.zeros((len(datasets),3))

# Run ML
for i, dataset in enumerate(datasets):
    print 'running ML on', dataset_names[i]
    
    X, y = dataset
    mean_perfs[i,:], std_perfs[i,:] = normalSVM.run(X, y)
    
# Save perf
dir_path = 'out/perf-double-preprocess/'

if not os.path.exists(dir_path):
    os.makedirs(dir_path)
            
np.savetxt(dir_path+"normalSVM_mean_perfs.csv", mean_perfs, delimiter=",")
np.savetxt(dir_path+"normalSVM_std_perfs.csv", std_perfs, delimiter=",")
with open(dir_path+'dataset_names.csv', 'wb') as csvfile:
    csv_writer = csv.writer(csvfile, quoting=csv.QUOTE_MINIMAL)
    csv_writer.writerow(dataset_names)
