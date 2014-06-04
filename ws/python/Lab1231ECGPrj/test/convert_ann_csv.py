#!/usr/bin/python

file_path = '/home/tor/ecg/lab1231-ecg-prj/dataset/hot/100.ann10s'
csv_path = '/home/tor/ecg/lab1231-ecg-prj/dataset/hot/100.ann10s.csv'

def main():
    content = [line.strip() for line in open(file_path)]
    
    csv = open(csv_path, "w")

    csv.write("'Elapsed time','Beat Type'\n")
    for line in content[1:]:# [0] is the column title
        clean_line = " ".join(line.split())# remove any whitespace duplicates
        line_comp = clean_line.split(" ")
        csv.write(line_comp[0] + "," + line_comp[3] +"\n")
    
    csv.close()
    
if __name__ == '__main__':
    main()
