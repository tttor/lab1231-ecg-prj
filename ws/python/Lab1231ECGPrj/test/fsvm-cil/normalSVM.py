#!/usr/bin/python

# @author: vektor dewanto
# @obj: reproduce data on table 6.3, p94 using (normal/standard) SVM
# Some preprocessing steps follow (Batuwitage, phdthesis), namely: the dataset (p92), the proprocessing, the hyperparam tuning, the eval metric

import numpy as np
from sklearn import svm
from sklearn.metrics import confusion_matrix
from sklearn import cross_validation as cv
from sklearn.grid_search import GridSearchCV
from sklearn import preprocessing

import util 

def preprocess(raw_X):
    # Scaled into [-1,+1] interval. (Batuwitage, phdthesis, p92), BUT results in worse gmean than using preprocessing.scale()
    min_max_scaler = preprocessing.MinMaxScaler(feature_range=(-1.0, 1.0))
    X = min_max_scaler.fit_transform(raw_X)

#    # Scaled data has zero mean and unit variance:
#    X = preprocessing.scale(raw_X)
    
    return X

def tune_train_test(X, y):
    # Utilize the so-called outer-inner-cv (Batuwitage, p55)
    outer_kfold = 5
    inner_kfold = 5
    gmeans = np.zeros(shape=(outer_kfold,3))# column: gmean, SE, SP

    outer_iter_idx = 0
    skf = cv.StratifiedKFold(y, n_folds=outer_kfold)# contains n_folds clones datasets, each contain te_ and tr_ data with the ratio of 1:(n_fols-1)
    for tr_idx, te_idx in skf:# the outer loop: StratifiedKFold
        print 'Outer loop iter-th=', outer_iter_idx+1, '--------------------'
        X_tr, X_te = X[tr_idx], X[te_idx]
        y_tr, y_te = y[tr_idx], y[te_idx]
        
        # Inner loop: A _coarse_ grid-search k-fold CV, (Batuwitage, phdthesis, p55)
        coarse_param_space = {'C': [2**i for i in range(-15,0)], \
                              'gamma': [2**i for i in range(1,16)]}
        
        coarse_clf = svm.SVC(kernel='rbf')
        coarse_grid_search = GridSearchCV(coarse_clf, param_grid=coarse_param_space, cv=inner_kfold)
        coarse_grid_search.fit(X_tr,y_tr)# Run fit with all sets of parameters.
        
        primary_C = coarse_grid_search.get_params()['estimator__C']
        primary_gamma = coarse_grid_search.get_params()['estimator__gamma']
        
        # Inner loop: A _narrow_ grid search
        narrow_param_space = {'C': [ 2**(primary_C+i) for i in [float(j)/100 for j in range(-75,100, 25)] ], \
                              'gamma': [ 2**(primary_gamma+i) for i in [float(j)/100 for j in range(-75,100, 25)] ]}
        
        narrow_clf = svm.SVC(kernel='rbf')
        narrow_grid_search = GridSearchCV(coarse_clf, param_grid=coarse_param_space, cv=inner_kfold)
        narrow_grid_search.fit(X_tr,y_tr)# Run fit with all sets of parameters.
        
        secondary_C = narrow_grid_search.get_params()['estimator__C']
        secondary_gamma = narrow_grid_search.get_params()['estimator__gamma']
        
        # a new SVM model was trained by using the complete training dataset on those secondary_C and secondary_gamma
        clf = svm.SVC(kernel='rbf', C=secondary_C, gamma=secondary_gamma)
        clf.fit(X_tr, y_tr)
        
        # the performance of the resulted model was tested on the remaining separate testing partition.
        y_pred = clf.predict(X_te)
        confusion_matrix(y_te, y_pred)
        gmeans[outer_iter_idx,:] = util.get_gmean(confusion_matrix(y_te, y_pred))
        outer_iter_idx = outer_iter_idx + 1    
        
    return (np.mean(gmeans, axis=0), np.std(gmeans, axis=0))

def run(raw_X, y):
    X = preprocess(raw_X)
    perf = tune_train_test(X, y)
    
    return perf
