#!/usr/bin/python

import Lab1231ECGPrj.preprocessor.preprocessor1 as prep
import Lab1231ECGPrj.classification.classification1 as clf_engine
import Lab1231ECGPrj.evaluator.evaluator1 as ev

from sklearn import datasets
from sklearn.cross_validation import train_test_split

import numpy as np

##########################################################################################
# SET THE DATASET
#dataset = ['mitdb/100']

#lead = 'MLII'
#length = 10

iris = datasets.load_iris()

##########################################################################################
# PREPROCESS
# >> Convert from the long-duration signal to a set of _annotated_ beats 
# >> TODO @tttor: Convert the original signal to Piecewise Linear Representation
# >> TODO @tttor: Scale the feature, e.g to [0,1] or [-1,+1], or standardize it to have mean 0 and variance 1; may use sklearn.preprocessing.StandardScaler
#data = prep.run(dataset[0], lead, length)

X = iris.data# X is an 2-d array of size of 150 x 4
y = iris.target

# Split the data into a training set and a test set
X_tr, X_te, y_tr, y_te = train_test_split(X, y, test_size=0.3, random_state=0)

# Shuffle n_sh times
# NOTE: a single dataset is a list of [X_tr, X_te, y_tr, y_te]
n_sh = 7
datasets = [train_test_split(X, y, test_size=0.3, random_state=i) for i in range(n_sh)]

##########################################################################################
# MODEL SELECTION, TRAINING, TESTING
# >> using the Dynamic Time Warping (DTW) as the distance function

clfs = ['SGD', 'nuSVC', 'NearestNeigbours', 'XRandomizedTrees', 'GradientBoosting']
n_clf = len(clfs)

clf_perf_set = [clf_engine.evaluate(clf, dataset) for clf in clfs for dataset in datasets]

clf_stars = [e[0] for e in clf_perf_set]
perf_metrics = [e[1] for e in clf_perf_set]

# Convert to 2D array of element e_{i,j} from classifier c_i and dataset D_j
clf_stars = np.asarray(clf_stars)
clf_stars = clf_stars.reshape(n_clf, n_sh)

perf_metrics = np.asarray(perf_metrics)
perf_metrics = perf_metrics.reshape(n_clf, n_sh)

##########################################################################################
# PLOTTING
ev.plot_1(perf_metrics, clfs)
ev.plot_2(perf_metrics, clfs)
ev.plot_cm(perf_metrics, clfs)

##########################################################################################
print 'finish :)'


