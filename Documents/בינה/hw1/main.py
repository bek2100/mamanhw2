'''
Parse input and run appropriate code./Users/rebeccaBar/Documents/בינה/HW1/buildpath.py
Don't use this file for the actual work; only minimal code should be here.
We just parse input and call methods from other modules.
'''

#do NOT import ways. This should be done from other files
#simply import your modules and call the appropriate functions


def base(source, target):
    'call function to find path using uniform cost, and return list of indices'
    raise NotImplementedError

    
def betterWaze(source, target,abstractMap=None):
    'call function to find path using better ways algorithm, and return list of indices'
    if not abstractMap:
        raise NotImplementedError # You should load the map you were asked to pickle
        # Note: pickle might give you an error for the namedtuples, even if they
        # are imported indirectly from ways.graph. You might need to declare, for
        # example: Link = ways.graph.Link
    raise NotImplementedError
    

def dispatch(argv):
    from sys import argv
    source, target = int(argv[2]), int(argv[3])
    if argv[1] == 'base':
        path = base(source, target)
    elif argv[1] == 'bw':
        abstractMap = None
        if len(argv)>4:
             import pickle as pkl
             abstractMap = pkl.load(open(argv[4],'rb'))
        path = betterWaze(source, target,abstractMap)
    print(' '.join(str(j) for j in path))


if __name__ == '__main__':
    from sys import argv
    dispatch(argv)
