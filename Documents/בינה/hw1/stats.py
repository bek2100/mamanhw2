'''
This file should be runnable to print map_statistics using 
$ python stats.py
'''

from collections import namedtuple
from collections import Counter
from ways import load_map_from_csv


def map_statistics(roads):
    '''return a dictionary containing the desired information
    You can edit this function as you wish'''
    Stat = namedtuple('Stat', ['max', 'min', 'avg'])
    rdlnks = [len(roads[i].links)
             for i in roads]
    rddis = [roads[i].links[j].distance
                for i in roads if not len(roads[i].links)==0
                 for j in range(0, len(roads[i].links))]
    rdhigh = [roads[i].links[j].highway_type
                for i in roads if not len(roads[i].links)==0
                    for j in  range(0, len(roads[i].links))]
    return {
        'Number of junctions' : len(roads),
        'Number of links' : sum(rdlnks),
        'Outgoing branching factor' : Stat(max= max(rdlnks), min=min(rdlnks), avg = float(sum(rdlnks)) / float(len(roads))),
        'Link distance' : Stat(max=max(rddis), min=min(rddis), avg=float(sum(rddis)) / float(len(rddis))),
        # value should be a dictionary
        # mapping each road_info.TYPE to the no' of links of this type
        'Link type histogram' : Counter(rdhigh),  # tip: use collections.Counter
    }


def print_stats():
    for k, v in map_statistics(load_map_from_csv()).items():
        print('{}: {}'.format(k, v))

        
if __name__ == '__main__':
    from sys import argv
    assert len(argv) == 1
    print_stats()
