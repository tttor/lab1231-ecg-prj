#!/usr/bin/python

import matplotlib.pyplot as plt
import collections

def read_raw(raw_path):
    content = [line.strip() for line in open(raw_path)]
    
    raw = [ [float(line.split(',')[0]),float(line.split(',')[1])] for line in content[2:] ]# content[0:2] contains column titles
    return raw

def read_seg(seg_path):
    content = [line.strip() for line in open(seg_path)]
    
    seg = [ [float(line.split(',')[0]),line.split(',')[1]] for line in content[1:] ]# content[0] contains column titles
    return seg

def read_ann(ann_path):
    content = [line.strip() for line in open(ann_path)]
    
    ann = [ [float(line.split(',')[0]),line.split(',')[1]] for line in content[1:] ]# content[0] contains column titles
    return ann
    
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
    
def main():
    # Read
    raw_path = '/home/tor/ecg/lab1231-ecg-prj/dataset/hot/100.MLII.10s.csv'
    raw = read_raw(raw_path)
    
    seg_path = '/home/tor/ecg/lab1231-ecg-prj/dataset/hot/100.MLII.seg10s.csv'
    seg = read_seg(seg_path)
    
    ann_path = '/home/tor/ecg/lab1231-ecg-prj/dataset/hot/100.ann10s.csv'
    ann = read_ann(ann_path)
    
    # Combine raw + seg data: 'Elapsed (s)','Reading (mV)', 'Event', where the last column is from the ECGPUWAVE output
    # FACT: The beginning of a beat has a type  of '(' _followed_ by a type of 'p'
    tot = raw
    seg_t = [e[0] for e in seg]# time data only
    
    for e in raw:
        if e[0] in seg_t:
            idx = seg_t.index(e[0])
            event = seg[idx][1]
            
            if event == '(':
                # Check the following event
                next_event = seg[idx+1][1]
                event += next_event
            elif event == ')':
                # Check the preceeding event
                past_event = seg[idx-1][1]
                event += past_event
            
            e.append(event)
        else:
            e.append('none')

    csv = open('tot.csv', "w")
    csv.write("'Elapsed time (s)','Reading (mV)','Event'\n")
    for e in tot:
        csv.write(str(e[0]) + "," + str(e[1]) + "," + e[2])
        csv.write("\n")
    csv.close()
    
#    # Plot
#    x = [xy[0] for xy in tot]
#    y = [xy[1] for xy in tot]
#    
##    my_dpi = 117
##    plt.figure(figsize=(1360/my_dpi, 768/my_dpi), dpi=my_dpi)
#    plt.plot(x,y,'-')
#    plt.ylim(ymax=2.0)
#    
#    for i in tot:
#        if i[2] == '(p':
#            print i[0], i[1]
#            plt.annotate('', xy=(i[0],i[1]), xytext=(i[0] - 0.3, i[1] + 0.5), arrowprops=dict(arrowstyle="->"))
#    plt.show()
    
    # Construct data (set of instances (beats))
    x_data = []
    
    found = False# whether the event of '(p' been ever found yet
    current_idx = -1
    for e in tot:
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
        label = get_label([e[0] for e in datum], ann)
        
        data.append((feature, label))
    
    # Plot each datum
    for i, datum in enumerate(data):
        x = [e[0] for e in datum[0]]
        y = [e[1] for e in datum[0]]
        
        plt.plot(x, y)
        plt.xlabel('Time (s)')
        plt.ylabel('Reading (mV)')
        plt.title('Datum th= '+ str(i+1) +' of ' + str(len(data)) + ' LABEL= ' + datum[1])
        print 'showing', i+1,'-th datum of ', len(data), 'LABEL=', datum[1]
        plt.show()
        
if __name__ == "__main__":
    main()

