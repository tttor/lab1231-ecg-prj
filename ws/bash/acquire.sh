#!/bin/bash

# @author: vektor dewanto
# @obj: acquire reading, event annotation, and beat annotation from mitdb, only for lead: MLII, by which the invalid record list below is hardcoded
# @note: depend on WFDB and ecgpuwave libs that work _well_ only in 32-bit machines

# Init
len=60 # in seconds, from t=0

sig=MLII
sig_num=0
db_name=mitdb

out_root_dir=hot

# rdsamp OPTIONS
#-c
#Produce output in CSV (comma-separated value) format (default: write output in tab-separated columns).
#-f time
#Begin at the specified time. By default, rdsamp starts at the beginning of the record
#-p
#Print times in seconds and milliseconds, and values in physical units. By default, rdsamp prints times in sample intervals and values in A/D units.
#-ps (or -Ps)
#Print the elapsed time in seconds. This is the default format when using -p or -P.
#-s signal-list
#Print only the signals named in the signal-list (one or more input signal numbers or names, separated by spaces; default: print all signals). This option may be used to re-order or duplicate signals.
#-v
#Print column headings (signal names on the first line, units on the second). The names of some signals are too wide to fit in the columns; such names are shortened by omitting the initial characters (since names of related signals often differ only at the end, this helps to make the columns identifiable). Names of units are shortened when necessary by omitting the final characters, since the initial characters are usually most important for distinguishing different units.

# ECGPUWAVE Options include:
#-f time
#Begin at the specified time (default: the beginning of the record).
#-i input-annotator
#Read QRS locations from the specified input-annotator (and copy them to the output annotation file). Default: run the built-in QRS detector.
#-n beat-type
#Specify which beats to process (must be used together with -i): beat_type may be 0 (default: process all beats) or 1 (process only beats labelled as NORMAL ("N") by the input annotator).
#-s n
#Analyze signal n (default: signal 0).
#-t time
#Stop at the specified time (default: the end of the record).

#RDANN Options include:
#-c chan
#Print only those annotations with chan fields that match chan.
#-e
#Print annotation times as elapsed times from the beginning of the record (default: rdann prints absolute times if the absolute time of the beginning of the record is defined, and elapsed times otherwise, unless the -x option has been given).
#-f time
#Begin at the specified time. By default, rdann starts at the beginning of the record; if modification labels are present, they are not printed unless -f 0 is given explicitly, however.
#-h
#Print a usage summary.
#-n num
#Print only those annotations with num fields that match num.
#-p type [ type ... ]
#Print annotations of the specified types only. The type arguments should be annotation mnemonics (e.g., N) as normally printed by rdann in the third column. More than one -p option may be used in a single command, and each -p option may have more than one type argument following it. If type begins with ‘‘-’’, however, it must immediately follow -p (standard annotation mnemonics do not begin with ‘‘-’’, but modification labels in an annotation file may define such mnemonics).
#-s sub
#Print only those annotations with subtyp fields that match sub.
#-t time
#Stop at the specified time.
#-v
#Print column headings.
#-x
#Use an alternate time format for output (the first three columns are the elapsed times in seconds, in minutes, and in hours, replacing the hh:mm:ss and sample number columns in the default output). Note that this format is incompatible with wrann.

read_db() {
    ## Read WFDB signal files and dump it to a csv
    for i in $(seq $1 $2)
    do
        echo reading record_num= $i
        record_num=$i

        reading_out_dir=$out_root_dir/$db_name/$record_num/$sig
        echo creating $reading_out_dir
        mkdir -p $reading_out_dir

        echo writing $reading_out_dir/$reading_csv_name
        rdsamp -r $db_name/$record_num -c -H -f 0 -t $len -v -ps -s $sig > $reading_out_dir/$len.reading.csv
        
        echo 
    done

    # Obtain the event annotations
    for i in $(seq $1 $2)
    do
        echo getting event annotation record_num= $i
        record_num=$i
        annotation=event_ann
        
        # Notice that the input signal is determined with signal number, not the signal(lead) name
        ecgpuwave -r $db_name/$record_num -a $annotation -s $sig_num -f 0 -t $len
        
        # convert to text file
        event_ann_out_dir=$out_root_dir/$db_name/$record_num/$sig
        rdann -r $db_name/$record_num -a $annotation -v -x > $event_ann_out_dir/$len.event_ann
        
        echo
    done
    
    # Obtain beat annotation
    for i in $(seq $1 $2)
    do
        echo getting beat annotation record_num= $i
        record_num=$i
        beat_ann_out_dir=$out_root_dir/$db_name/$record_num
        
        rdann -r $db_name/$record_num -f 0 -t $len -a atr -v -x > $beat_ann_out_dir/$len.beat_ann
        
        echo
    done
    
    # Remove invalid dir
    tmp=$3[@]
    invalid=("${!tmp}")

    for i in "${invalid[@]}"
    do
        invalid_dir=$out_root_dir/$db_name/$i
        echo removing $invalid_dir
        rm -rf $invalid_dir
    done
}

# Read the first chunk
BEGIN=100
END=124

# >> records not-exist: 110, 120
# >> records not having MLII signal: 102 104
# >> record causing segmentation fault in ecgpuwave for the duration of 60s : 106 108 109 111 112
invalid=(102 104 106 108 109 110 111 112 120)

read_db $BEGIN $END invalid

# Read the second chunk
BEGIN=200
END=234

# >> records not-exist: 204 206 211 216 218 224 225 226 227 229
# >> records do not have MLII: 
## >> record causing segmentation fault in ecgpuwave for the duration of 60s : 201 203 207 214 215 221 228 231 232 233
invalid=(201 203 204 206 207 211 214 215 216 218 221 224 225 226 227 228 229 231 232 233)

read_db $BEGIN $END invalid

# Remove tmp files 
rm fort* #from ecgpuwave
