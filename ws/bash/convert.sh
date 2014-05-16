#/bin/bash

db_dir=../../dataset/mitdb.acq.20140516.1826

for entry in $db_dir/*
do
    echo converting beat_ann on $entry
    python ../python/Lab1231ECGPrj/util/convert2csv.py  beat_ann $entry
    
    echo converting event_ann on $entry
    python ../python/Lab1231ECGPrj/util/convert2csv.py  event_ann $entry
done
