#!/usr/bin/python

import Lab1231ECGPrj.evaluator.evaluator1 as ev

from sklearn.cross_validation import train_test_split
from sklearn.grid_search import GridSearchCV

from sklearn.linear_model import SGDClassifier
from sklearn.svm import NuSVC
from sklearn.neighbors import KNeighborsClassifier #from sklearn.neighbors import RadiusNeighborsClassifier
from sklearn.ensemble import ExtraTreesClassifier
from sklearn.ensemble import GradientBoostingClassifier
    
def tune_SGD(X_tr, y_tr):
    print 'tune_SGD(X_tr, y_tr):'
    clf = SGDClassifier()
    
    param_space = {'loss': ['hinge', 'log', 'modified_huber'], \
                   'penalty': ['l2', 'l1'], \
                   'alpha' : [0.0001, 0.001]}
    
    grid_search = GridSearchCV(clf, param_grid=param_space, cv=10)
    grid_search.fit(X_tr,y_tr)# Run fit with all sets of parameters.
    
    return grid_search.get_params()['estimator']

def tune_nuSVC(X_tr, y_tr):
    print 'tune_nuSVC(X_tr, y_tr):'
    clf = NuSVC()
    
    param_space = {'nu': [0.3, 0.5, 0.8], \
                   'kernel': ['poly', 'rbf', 'sigmoid'], \
                   'degree': [3, 5], \
                   'gamma': [0.0, 0.5]}
    
    grid_search = GridSearchCV(clf, param_grid=param_space, cv=10)
    grid_search.fit(X_tr,y_tr)# Run fit with all sets of parameters.
    
    return grid_search.get_params()['estimator']

def tune_NearestNeigbours(X_tr, y_tr):
    print 'tune_NearestNeigbours(X_tr, y_tr):'
    clf = KNeighborsClassifier(algorithm='ball_tree')
    
    param_space = {'n_neighbors': [3, 5, 10], \
                   'leaf_size': [20, 30, 50], \
                   'metric': ['minkowski']}# 'euclidean' is unapplicable to the ball_tree algorithm, but for brute

    grid_search = GridSearchCV(clf, param_grid=param_space, cv=10)
    grid_search.fit(X_tr,y_tr)# Run fit with all sets of parameters.
    
    return grid_search.get_params()['estimator']

def tune_XRandomizedTrees(X_tr, y_tr):
    print 'tune_XRandomizedTrees(X_tr, y_tr):'
    clf = ExtraTreesClassifier()
    
    param_space = {'n_estimators': [10, 30, 70], \
                   'criterion': ['gini', 'entropy'], \
                   'max_features': [0.3, 0.5]}
    
    grid_search = GridSearchCV(clf, param_grid=param_space, cv=10)
    grid_search.fit(X_tr,y_tr)# Run fit with all sets of parameters.
    
    return grid_search.get_params()['estimator']

def tune_GradientBoosting(X_tr, y_tr):
    print 'tune_GradientBoosting(X_tr, y_tr):'
    clf = GradientBoostingClassifier()
    
    param_space = {'n_estimators': [50, 100, 200], \
                   'learning_rate': [0.1, 0.3], \
                   'max_features': [0.3, 0.5]}
    
    grid_search = GridSearchCV(clf, param_grid=param_space, cv=10)
    grid_search.fit(X_tr,y_tr)# Run fit with all sets of parameters.
    
    return grid_search.get_params()['estimator']
    
def tune(clf_type, X_tr, y_tr):
    ''' Return the best classifier of type of clf given a training data tr_data, its hyperparams are tuned using k-fold CV'''
    meta_clf = None
    if clf_type == 'SGD':
        meta_clf = tune_SGD(X_tr, y_tr)
    elif clf_type == 'nuSVC':
        meta_clf = tune_nuSVC(X_tr, y_tr)
    elif clf_type == 'NearestNeigbours':
        meta_clf = tune_NearestNeigbours(X_tr, y_tr)
    elif clf_type == 'XRandomizedTrees':
        meta_clf = tune_XRandomizedTrees(X_tr, y_tr)
    elif clf_type == 'GradientBoosting':
        meta_clf = tune_GradientBoosting(X_tr, y_tr)
    
    return meta_clf
    
def train(clf, X_tr, y_tr):
    clf.fit(X_tr, y_tr)
    return clf

def test(clf, X_te, y_te):
    y_pred = clf.predict(X_te)
    perf = ev.run(y_te, y_pred)
    
    return perf
    
def evaluate(clf_type, dataset):
    X_tr = dataset[0]
    X_te = dataset[1]
    y_tr = dataset[2]
    y_te = dataset[3]
    
    meta_clf = tune(clf_type, X_tr, y_tr)
    clf_star = train(meta_clf, X_tr, y_tr)
    perf = test(clf_star, X_te, y_te)
    
    return (clf_star, perf)
