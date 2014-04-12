#!/usr/bin/env python
# encoding: utf-8

import io
import collections
import sys

from library.sio import *
from collections import defaultdict, deque, Counter
from itertools import *

try:
  import networkx as nx # http://networkx.github.io/
  from networkx.utils.union_find import UnionFind
except ImportError:
  pass

PARALLELISM_LEVEL = 8

try:
  import futures
except ImportError:
  print >> sys.stderr, "futures not available; multithreading disabled."
  PARALLELISM_LEVEL = 0

def boilerplate_main(read, solve, write, parallelism_level=PARALLELISM_LEVEL):
  # Read in all test cases
  cases = [read() for _ in xrange(rint())]

  # Solve test cases
  if parallelism_level:
    pool = futures.ThreadPoolExecutor(max_workers=parallelism_level)
    answers = [pool.submit(solve, *case) for case in cases]
    answers = [ans.result() for ans in answers]
  else:
    answers = [solve(*case) for case in cases]

  # Print results
  for i, answer in enumerate(answers, 1):
    print 'Case #%i:' % i,
    write(*answer);

  if parallelism_level:
    pool.shutdown(True)
