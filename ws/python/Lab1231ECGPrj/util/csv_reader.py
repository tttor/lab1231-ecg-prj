#!/usr/bin/python

def read(path, begin= 0):
    content = [line.strip() for line in open(path)]
    
    reading = [ [float(line.split(',')[0]),float(line.split(',')[1])] for line in content[begin:] ]
    return reading

def read_2(path, begin= 0):
    content = [line.strip() for line in open(path)]
    
    reading = [ [float(line.split(',')[0]),line.split(',')[1]] for line in content[begin:] ]
    return reading
    
def test():
    print 'this should test all readers'
    
if __name__ == '__main__':
    test()
