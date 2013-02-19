import os, itertools
from scipy.stats import norm
from numpy import mean, std

def parse(x):
    data = filter(None, [y.split() for y in open(x).read().split("\n")])
    rv = {}
    for targ, feat in itertools.groupby(sorted(data, key=lambda x: x[0]), lambda x: x[0]):
        rv[float(targ)] = tuple((map(float, x[1:]) for x in feat))
    return rv

train = parse("train.data")
test = list(itertools.chain(*parse("test.data").itervalues()))
val = list(itertools.chain(*parse("train.data").itervalues()))

class Reverand:
    def __init__(s, data):
        s.data = data
        s.n = len(list(itertools.chain(*data.itervalues())))
        s.prior = {}
        s.cond = {}
        s.nfeat = len(data[0]) - 1
        for targ, points in data.iteritems():
            s.prior[targ] = len(points) / float(s.n)
            for i, feat in enumerate(zip(*points)):
                s.cond[i, targ] = mean(feat), std(feat)
                
    def __call__(s, feat):
        posterior = {}
        for targ, prior in s.prior.iteritems():
            posterior[targ] = prior
            for i in range(s.nfeat):
                posterior[targ] *= norm(*s.cond[i, targ]).pdf(feat[i])
        return max(posterior.iteritems(), key=lambda (x, y): y)[0]
            
r = Reverand(train)
r(test[0])

print map(r, val)
