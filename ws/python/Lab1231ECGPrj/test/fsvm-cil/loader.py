import numpy as np
import re

uci_db_dir_path = '../../../../../dataset/uci/'

def load_glass():
    # Load the dataset: glass
    db_path = uci_db_dir_path + 'glass/glass.data'
    n_sample_per_class = {'positive_class': 70, 'negative_class': 144}
    n_sample = sum([n_sample_per_class['positive_class'], n_sample_per_class['negative_class']])
    n_class = len(n_sample_per_class)
    n_fea = 9

    content = [line.strip() for line in open(db_path)]
    data_str = [ line.split(',')[1:] for line in content ] # 0th element is instance index

    data = np.zeros((n_sample, n_fea+1))# plus one for the true label
    for i,datum in enumerate(data_str):
        for j, e in enumerate(datum):
            data[i,j] = float(e)

    X = data[:,0:-1]
    y = data[:,-1]
    y = np.asarray([1 if l==1 else 2 for l in y])# Into 2 classes: positive (label: 1) and negative (label:2), (Batuwitage, phdthesis, p92)
    
    return (X, y)

def load_pima_indians_diabetes():
    # Load the dataset: pima-indians
    db_path = uci_db_dir_path + 'pima-indians-diabetes/pima-indians-diabetes.data'
    n_sample_per_class = {'positive_class': 268, 'negative_class': 500}
    n_sample = sum([n_sample_per_class['positive_class'], n_sample_per_class['negative_class']])
    n_class = len(n_sample_per_class)
    n_fea = 8

    content = [line.strip() for line in open(db_path)]
    data_str = [ line.split(',')[:] for line in content ]

    data = np.zeros((n_sample, n_fea+1))# plus one for the true label
    for i,datum in enumerate(data_str):
        for j, e in enumerate(datum):
            data[i,j] = float(e)

    X = data[:,0:-1]
    y = data[:,-1]
    y = np.asarray([1 if l==1 else 2 for l in y])# Into 2 classes: positive (label: 1) and negative (label:2), (Batuwitage, phdthesis, p92)

    return (X, y)

def load_haberman():
    # Load the dataset: haberman
    db_path = uci_db_dir_path + 'haberman/haberman.data'
    n_sample_per_class = {'positive_class': 81, 'negative_class': 225}
    n_sample = sum([n_sample_per_class['positive_class'], n_sample_per_class['negative_class']])
    n_class = len(n_sample_per_class)
    n_fea = 3

    content = [line.strip() for line in open(db_path)]
    data_str = [ line.split(',')[:] for line in content ]

    data = np.zeros((n_sample, n_fea+1))# plus one for the true label
    for i,datum in enumerate(data_str):
        for j, e in enumerate(datum):
            data[i,j] = float(e)

    X = data[:,0:-1]
    y = data[:,-1]
    y = np.asarray([1 if l==2 else 2 for l in y])# reverse the class label: positive (label: 1) and negative (label:2), (Batuwitage, phdthesis, p92)
    
    return (X, y)

def load_page_blocks():
    db_path = uci_db_dir_path + 'page-blocks/page-blocks.data'
    n_sample_per_class = {'positive_class': 115, 'negative_class': 5358}
    n_sample = sum([n_sample_per_class['positive_class'], n_sample_per_class['negative_class']])
    n_class = len(n_sample_per_class)
    n_fea = 10

    content = [line.strip() for line in open(db_path)]
    data_str = [ re.sub(' +',' ',line).split(' ')[:] for line in content ]

    data = np.zeros((n_sample, n_fea+1))# plus one for the true label
    for i,datum in enumerate(data_str):
        for j, e in enumerate(datum):
            data[i,j] = float(e)

    X = data[:,0:-1]
    y = data[:,-1]
    y = np.asarray([1 if l==5 else 2 for l in y])# group into two class labels: positive (label: 1) and negative (label:2), (Batuwitage, phdthesis, p92)
    
    return (X, y)

def load_abalone():
    db_path = uci_db_dir_path + 'abalone/abalone.data'
    n_sample_per_class = {'positive_class': 103, 'negative_class': 4074}
    n_sample = sum([n_sample_per_class['positive_class'], n_sample_per_class['negative_class']])
    n_class = len(n_sample_per_class)
    n_fea = 7# exclude one non-numeric feature: Sex

    content = [line.strip() for line in open(db_path)]
    data_str = [ line.split(',')[1:] for line in content ]# exclude one non-numeric feature: Sex

    data = np.zeros((n_sample, n_fea+1))# plus one for the true label
    for i,datum in enumerate(data_str):
        for j, e in enumerate(datum):
            data[i,j] = float(e)

    X = data[:,0:-1]
    y = data[:,-1]
    y = np.asarray([1 if l==15 else 2 for l in y])# group into two class labels: positive (label: 1) and negative (label:2), (Batuwitage, phdthesis, p92)
    
    return (X, y)

def load_yeast():
    db_path = uci_db_dir_path + 'yeast/yeast.data'
    n_sample_per_class = {'positive_class': 51, 'negative_class': 1433}
    n_sample = sum([n_sample_per_class['positive_class'], n_sample_per_class['negative_class']])
    n_class = len(n_sample_per_class)
    n_fea = 8# exclude one non-numeric feature: Name

    content = [line.strip() for line in open(db_path)]
    data_str = [ re.sub(' +',' ',line).split(' ')[1:] for line in content ]# exclude one non-numeric feature: Name

    data = np.zeros((n_sample, n_fea+1))# plus one for the true label
    for i,datum in enumerate(data_str):
        for j, e in enumerate(datum):
            val = None
            if j == len(datum)-1:# group into two class labels: positive (label: 1) and negative (label:2), (Batuwitage, phdthesis, p92)
                if e=='ME2':
                    val=1
                else:
                    val=2
            else:
                val = float(e)
                
            data[i,j] = val

    X = data[:,0:-1]
    y = data[:,-1]
    
    return (X, y)

def load_transfusion():
    db_path = uci_db_dir_path + 'blood-transfusion/transfusion.data'
    n_sample_per_class = {'positive_class': 178, 'negative_class': 570}
    n_sample = sum([n_sample_per_class['positive_class'], n_sample_per_class['negative_class']])
    n_class = len(n_sample_per_class)
    n_fea = 4

    content = [line.strip() for line in open(db_path)]
    data_str = [ line.split(',')[:] for line in content[1:] ]# first line contains column labels

    data = np.zeros((n_sample, n_fea+1))# plus one for the true label
    for i,datum in enumerate(data_str):
        for j, e in enumerate(datum):
            data[i,j] = float(e)

    X = data[:,0:-1]
    y = data[:,-1]
    y = np.asarray([1 if l==1 else 2 for l in y])# group into two class labels: positive (label: 1) and negative (label:2), (Batuwitage, phdthesis, p92)
    
    return (X, y)

def load(name):
    print 'loading', name
    if name=='glass':
        return load_glass()
    elif name=='pima-indians-diabetes':
        return load_pima_indians_diabetes()
    elif name=='haberman':
        return load_haberman()
    elif name=='page-blocks':
        return load_page_blocks()
    elif name=='abalone':
        return load_abalone()
    elif name=='yeast':
        return load_yeast()
    elif name=='transfusion':
        return load_transfusion()
    
    else:
        assert False, 'erronouos name'
        
if __name__ == '__main__':
    load('yeast')
