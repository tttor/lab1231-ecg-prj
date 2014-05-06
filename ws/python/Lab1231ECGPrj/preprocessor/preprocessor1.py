#!/usr/bin/python

import matplotlib.pyplot as plt
import collections
import Lab1231ECGPrj.util.csv_reader as reader

dataset_dir_path = '/home/tor/ecg/lab1231-ecg-prj/dataset/'

def is_valid(datum):
    ''' A datum is valid if it contains the following events: '(p', ')p', '(N', ')N', '(t', ')t '''
    valid = False
    datum_event = [subdatum[2] for subdatum in datum]
    
    if len(datum_event) == len(set(datum_event)):
        valid = True
    
    compulsory_event = ['(p', ')p', '(N', ')N', '(t', ')t']
    for e in compulsory_event:
        if e in datum_event:
            valid = True
        else:
            valid = False
            
    return valid

def get_label(data, ann):
    labels = [e[1] for e in ann if e[0] in data]
    counter = collections.Counter(labels)

    return counter.most_common(1)[0][0]

def run(dataset, lead, length):
    ''' @brief
    @input: dataset name, e.g 'mitdb/100'
    @output: D = {x, y}, where x = beat and y = beat type '''
    print 'preprocessor1.run(): BEGIN'
    
    # Read
    path = dataset_dir_path + dataset + '/' + lead + '/' + str(length) + '.csv'
    reading = reader.read(path, 2)# content[0] and [1] contain column titles
    
    path = dataset_dir_path + dataset + '/' + lead + '/' + 'event_ann.' + str(length) + '.csv'
    event_ann = reader.read_2(path, 1)# content[0] contains column titles
    
    path = dataset_dir_path + dataset + '/' + 'beat_ann.' + str(length) + '.csv'
    beat_type_ann = reader.read_2(path, 1)# content[0] contains column titles
    
    # Embed event_ann into reading (signals are still in long length, not yet segmented into beats)
    event_ann_t = [e[0] for e in event_ann]# time data only
    
    for e in reading:
        if e[0] in event_ann_t:
            idx = event_ann_t.index(e[0])
            event = event_ann[idx][1]
            
            if event == '(':
                # Check the following event
                next_event = event_ann[idx+1][1]
                event += next_event
            elif event == ')':
                # Check the preceeding event
                past_event = event_ann[idx-1][1]
                event += past_event
            
            e.append(event)
        else:
            e.append('none')
            
#    # Plot the event_ann-embedded readings
#    x = [xy[0] for xy in reading]
#    y = [xy[1] for xy in reading]
#    plt.plot(x,y,'-')
#    
#    for i in reading:
#        if i[2] == '(p':
#            plt.annotate('', xy=(i[0],i[1]), xytext=(i[0] - 0.3, i[1] + 0.5), arrowprops=dict(arrowstyle="->"))
#    plt.show()

    # Construct the data data (set of instances (beats))
    x_data = []
    
    found = False# whether the event of '(p' been ever found yet
    current_idx = -1
    for e in reading:
        event = e[2]
        if event == '(p':
            found = True
            x_data.append([]);
            current_idx += 1
        
        if found:
            x_data[current_idx].append(e)
    
    # Validate the constructed data
    x_data[:] = [datum for datum in x_data if is_valid(datum)]
    
    # Add the annotation for each datum in data
    data = []
    for datum in x_data:
        feature = datum;
        label = get_label([e[0] for e in datum], beat_type_ann)
        
        data.append((feature, label))
    
#    # Plot each datum
#    for i, datum in enumerate(data):
#        x = [e[0] for e in datum[0]]
#        y = [e[1] for e in datum[0]]
#        
#        plt.plot(x, y)
#        plt.xlabel('Time (s)')
#        plt.ylabel('Reading (mV)')
#        plt.title('Datum th= '+ str(i+1) +' of ' + str(len(data)) + ' LABEL= ' + datum[1])
#        print 'showing', i+1,'-th datum of ', len(data), 'LABEL=', datum[1]
#        plt.show()
        
    print 'preprocessor1.run(): END'
    return data
    
if __name__ == '__main__':
    run()
