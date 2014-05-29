#!/usr/bin/python

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages

def get_gmean(cm):
    '''For now: Only for binary classification. 
    Based on (Batuwitage, phdthesis, p71)'''
    TP = cm[0,0] 
    TN = cm[1,1]
    FP = cm[1,0]
    FN = cm[0,1]
    
    SE = float(TP)/(TP+FN)
    SP = float(TN)/(TN+FP)
    gmean = np.sqrt(SE*SP)
    
    return (gmean, SE, SP)
    
def plot_bar(means, stds, metric, dbs, title):
    # Hardcoded
    target = {}# dataset: (Gmean, SE, SP)
    target['glass'] = (78.11, 70.29, 86.80, 'glass') 
    target['pima-indians-diabetes'] = (68.94, 53.04, 89.60, 'pima-inds')
    target['haberman'] = (42.04, 19.78, 89.33, 'hbman')
    target['page-blocks'] = (76.14, 58.26, 99.52, 'page-blck')
    target['abalone'] = (21.93, 4.86, 99.02, 'abalone')
    target['yeast'] =(58.68, 35.09, 98.12, 'yeast')
    target['transfusion'] = (54.47, 32.03, 92.63, 'trans')
    
    col = None
    if metric == 'Gmean':
        col = 0
    elif metric == 'SE':
        col = 1
    elif metric == 'SP':
        col = 2
    else:
        assert False, 'erronouos metric'
        
    target_means = [ target[db][col] for db in dbs ]
    target_stds = [0.0] * len(target_means)
    
    # 
    N = len(dbs)
    
    ind = np.arange(N)  # the x locations for the groups
    width = 0.35       # the width of the bars

    fig, ax = plt.subplots()
    rects1 = ax.bar(ind, target_means, width, color='r', yerr=target_stds)

    #
    rects2 = ax.bar(ind+width, means*100.0, width, color='y', yerr=stds*100.0)

    ax.set_ylabel('Scores (%)')
    ax.set_title(title)
    ax.set_xticks(ind+width)
    ax.set_xticklabels([target[name][3] for name in dbs])

    ax.legend( (rects1[0], rects2[0]), ('Target', 'Achieved') )

    def autolabel(rects):
        # attach some text labels
        for rect in rects:
            height = rect.get_height()
            ax.text(rect.get_x()+rect.get_width()/2., 1.05*height, '%d'%int(height), ha='center', va='bottom')

    autolabel(rects1)
    autolabel(rects2)

    # Save
    with PdfPages('out/cmp.pdf') as pdf:
        pdf.savefig(fig) 
    
    plt.show()
    
