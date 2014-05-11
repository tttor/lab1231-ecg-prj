#!/usr/bin/python

from sklearn.metrics import accuracy_score
from sklearn.metrics import precision_score
from sklearn.metrics import recall_score
from sklearn.metrics import fbeta_score
from sklearn.metrics import hamming_loss
from sklearn.metrics import confusion_matrix

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages

g_plot_dir_path = '/home/tor/Dropbox/robotics/prj/010/wrt/draft/fig/'

def run(y_true, y_pred):
    perf = {}
    
    perf['accuracy'] = accuracy_score(y_true, y_pred)
    perf['precision'] = precision_score(y_true, y_pred, average='micro')
    perf['recall'] = recall_score(y_true, y_pred, average='micro')
    perf['fbeta_score'] = fbeta_score(y_true, y_pred, average='macro', beta=1.0)
    perf['hamming_loss'] = hamming_loss(y_true, y_pred)
    perf['cm'] = confusion_matrix(y_true, y_pred)
    
    return perf

def plot_cm(perf_metrics, clfs):
    n_clf = perf_metrics.shape[0]
    n_sh = perf_metrics.shape[1]
    n_class = np.shape(perf_metrics[0,0]['cm'])[0]
    
    # Arrange the metrics
    cm_means = []# the values is averaged over datasets from all class types
    for i in range(n_clf):
        cm_metrics_1D = []# from_one_class from all datasets
        for j in range(n_sh):
            cm_metrics_1D.append(perf_metrics[i,j]['cm'])
        
        cm_mean = np.zeros((n_class, n_class)) # mean over datasets from _one_ classfier type
        for m in range(n_class):
            for n in range(n_class):
                mn_value_list = [cm[m,n] for cm in cm_metrics_1D]
                cm_mean[m,n] = np.mean(mn_value_list)
        cm_means.append(cm_mean)
    
    # Set fig properties
    fig = plt.figure()
    for i, cm in enumerate(cm_means):
        ax = fig.add_subplot(2, 3, i+1)
        ax.set_aspect(1)
        res = ax.imshow(np.array(cm), cmap=plt.cm.jet, interpolation='nearest')
        
        width = len(cm)
        height = len(cm[0])
        
        alphabet = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'
        plt.xticks(range(width), alphabet[:width])
        plt.yticks(range(height), alphabet[:height])
        plt.xlabel(clfs[i])
#        plt.ylabel('GROUND TRUTH')
    
    # Save
    with PdfPages(g_plot_dir_path + 'cm.pdf') as pdf:
        pdf.savefig(fig) 
        
def plot_1(perf_metrics, clfs):
    '''Plot accuracy, precision, and recall in a bargraph.
    "mean" and "std" here refer to the one that is over datasets.
    '''
    n_clf = perf_metrics.shape[0]
    n_sh = perf_metrics.shape[1]
    
    # Arrange the metrics
    accuracy_metrics = np.zeros((n_clf, n_sh))
    for i in range(n_clf):
        for j in range(n_sh):
            accuracy_metrics[i,j] = perf_metrics[i,j]['accuracy']
            
    accuracy_means = np.mean(accuracy_metrics, axis=1)
    accuracy_stds = np.std(accuracy_metrics, axis=1)
    
    precision_metrics = np.zeros((n_clf, n_sh))
    for i in range(n_clf):
        for j in range(n_sh):
            precision_metrics[i,j] = perf_metrics[i,j]['precision']
            
    precision_means = np.mean(precision_metrics, axis=1)
    precision_stds = np.std(precision_metrics, axis=1)
    
    recall_metrics = np.zeros((n_clf, n_sh))
    for i in range(n_clf):
        for j in range(n_sh):
            recall_metrics[i,j] = perf_metrics[i,j]['recall']
            
    recall_means = np.mean(recall_metrics, axis=1)
    recall_stds = np.std(recall_metrics, axis=1)

    # Set bar graph properties
    ind = np.arange(n_clf)  # the x locations for the groups
    width = 0.25       # the width of the bars

    fig, ax = plt.subplots()
    rects1 = ax.bar(ind, accuracy_means, width, color='r', yerr=accuracy_stds)
    rects2 = ax.bar(ind+width, precision_means, width, color='y', yerr=precision_stds)
    rects3 = ax.bar(ind+width*2, recall_means, width, color='g', yerr=recall_stds)

    ax.set_ylabel('Scores')
    ax.set_title('Performance comparison among classfiers')
    ax.set_xticks(ind+(width*3/2))
    ax.set_xticklabels(clfs)

    ax.legend( (rects1[0], rects2[0], rects3[0]), ('accuracy', 'precision', 'recall'), loc='upper left' )
    
    # Save
    with PdfPages(g_plot_dir_path + 'accuracy_precision_recall.pdf') as pdf:
        pdf.savefig(fig) 

def plot_2(perf_metrics, clfs):
    '''Plot fbeta_score and hamming_loss in a bargraph.
    "mean" and "std" here refer to the one that is over datasets.
    '''
    n_clf = perf_metrics.shape[0]
    n_sh = perf_metrics.shape[1]
    
    # Arrange the metrics
    fbeta_score_metrics = np.zeros((n_clf, n_sh))
    for i in range(n_clf):
        for j in range(n_sh):
            fbeta_score_metrics[i,j] = perf_metrics[i,j]['fbeta_score']
            
    fbeta_score_means = np.mean(fbeta_score_metrics, axis=1)
    fbeta_score_stds = np.std(fbeta_score_metrics, axis=1)
    
    hamming_loss_metrics = np.zeros((n_clf, n_sh))
    for i in range(n_clf):
        for j in range(n_sh):
            hamming_loss_metrics[i,j] = perf_metrics[i,j]['hamming_loss']
            
    hamming_loss_means = np.mean(hamming_loss_metrics, axis=1)
    hamming_loss_stds = np.std(hamming_loss_metrics, axis=1)
    
    # Set bar graph properties
    ind = np.arange(n_clf)  # the x locations for the groups
    width = 0.25       # the width of the bars

    fig, ax = plt.subplots()
    rects1 = ax.bar(ind, fbeta_score_means, width, color='r', yerr=fbeta_score_stds)
    rects2 = ax.bar(ind+width, hamming_loss_means, width, color='y', yerr=hamming_loss_stds)
    
    ax.set_ylabel('Scores, Loss')
    ax.set_title('Performance comparison among classfiers')
    ax.set_xticks(ind+width)
    ax.set_xticklabels(clfs)

    ax.legend( (rects1[0], rects2[0]), ('fbeta score', 'hamming loss'), loc='upper left' )
    
    # Save
    with PdfPages(g_plot_dir_path + 'fbeta_score_hamming_loss.pdf') as pdf:
        pdf.savefig(fig) 
    
def test():
    print 'this is a test'
    
if __name__ == '__main__':
    test()
