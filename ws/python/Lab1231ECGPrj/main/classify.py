#!/usr/bin/python

import Lab1231ECGPrj.preprocessor.preprocessor1 as prep
import Lab1231ECGPrj.classifier.classifier1 as cls
import Lab1231ECGPrj.evaluator.evaluator1 as ev

def run():
    # SET THE DATASET
    dataset = ['mitdb/100']

    lead = 'MLII'
    length = 10
    
    # PREPROCESS
    # >> Convert from the long-duration signal to a set of _annotated_ beats 
    # >> TODO @tttor: Convert the original signal to Piecewise Linear Representation
    data = prep.run(dataset[0], lead, length)
    
    # CLASSIFY
    # >> using the Dynamic Time Warping (DTW) as the distance function
    cls.run()
    
    # EVALUATE
    ev.run()

if __name__ == '__main__':
    run()
