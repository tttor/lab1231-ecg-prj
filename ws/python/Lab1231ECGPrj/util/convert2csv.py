#!/usr/bin/python

import argparse

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("type", help="either beat_ann or event_ann")
    parser.add_argument("dir", help="the dir where the src file is located")
    args = parser.parse_args()
    
    src_path = None
    csv_path = None
    if args.type=='beat_ann':
        src_path = args.dir + '/60.' + args.type
        csv_path = args.dir + '/60.' + args.type + '.csv'
    elif args.type=='event_ann':
        src_path = args.dir + '/MLII/60.' + args.type
        csv_path = args.dir + '/MLII/60.' + args.type + '.csv'
    else:
        print 'unknown type, bye!'
        return
        
    content = [line.strip() for line in open(src_path)]
    
    csv = open(csv_path, "w")

    if args.type=='beat_ann':
        csv.write("'Elapsed time','Beat Ann'\n")
    elif args.type=='event_ann':
        csv.write("'Elapsed time','Event Ann'\n")

    for line in content[1:]:# [0] is the column title
        clean_line = " ".join(line.split())# remove any whitespace duplicates
        line_comp = clean_line.split(" ")
        csv.write(line_comp[0] + "," + line_comp[3] +"\n")
    
    csv.close()
    
if __name__ == '__main__':
    main()
