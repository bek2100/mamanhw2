# -*- coding: utf-8 -*-

from time import clock
import zlib
from math import acos, radians, pi
from numpy import ones, cos,array, sin
'General tools'

DB_DIRNAME = 'db/'

'This is arbitrary, and will change in the tests'
SEED = 0x23587643

def dhash(*data):
    'Generates a random-looking deterministic hash'
    return abs(zlib.adler32(bytes(str(data),'UTF-8'))*111) * SEED % 0xffffffff
## The move from python 2 to 3 caused some problems.

def dbopen(fname, *args, **kwargs):
    'make sure we are in the correct directory'
    if not fname.startswith(DB_DIRNAME):
        fname = DB_DIRNAME + fname
    return open(fname, *args, **kwargs)


'DMS := Degrees, Minutes, Seconds'
def float2dms(decimal_degrees):
    degrees = int(decimal_degrees)
    minutes = int(60 * (decimal_degrees - degrees))
    seconds = int(3600 * (decimal_degrees - degrees - minutes / 60))
    return (degrees, minutes, seconds)


def dms2float(degrees, minutes, seconds=0):
    return degrees + minutes / 60 + seconds / 3600


def compute_distance(lat1, lon1, lat2, lon2):
    '''computes distance in KM'''
    '''
    This code was borrowed from 
    http://www.johndcook.com/python_longitude_latitude.html
    '''
    if (lat1, lon1) == (lat2, lon2):
        return 0.0
    if max(abs(lat1 - lat2), abs(lon1 - lon2)) < 0.00001:
        return 0.001

    phi1 = radians(90 - lat1)
    phi2 = radians(90 - lat2)
    
    meter_units_factor = 40000 / (2 * pi)
    arc = acos(sin(phi1) * sin(phi2) * cos(radians(lon1) - radians(lon2))
             + cos(phi1) * cos(phi2))
    return arc * meter_units_factor


class Everything(object):
    '(Lousy) complement for the empty set'
    def __contains__(self, val):
        return True 

def timed(f):
    '''decorator for printing the timing of functions
    usage: 
    @timed
    def some_funcion(args...):'''
    
    def wrap(*x, **d):
        start = clock()
        res = f(*x, **d)
        print(f.__name__, ':', clock() - start)
        return res
    return wrap

if __name__ == '__main__':
    for i in range(10):
        print(dhash(i))
