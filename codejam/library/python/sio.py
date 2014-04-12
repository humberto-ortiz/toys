# Simple I/O library for reading files designed to be read with C++ iostream.
# Focus is on simplicity and ease of use, not performance. The use of binary
# mode for stdin is regrettable but necessary for peeking.

import io

stdin_stream = io.BufferedReader(io.open(0, 'rb'))

def skipspace(stream):
  while stream.peek(1)[0].isspace():
    stream.read(1)

def rint(stream=stdin_stream):
  skipspace(stream)
  buf = ""
  c = "0"
  while c in "0123456789":
    buf += stream.read(1)
    c = stream.peek(1)[0]
  return int(buf)

def rfloat(stream=stdin_stream):
  skipspace(stream)
  buf = ""
  c = "0"
  while c in ".0123456789":
    buf += stream.read(1)
    c = stream.peek(1)[0]
  return float(buf)

def rword(stream=stdin_stream):
  skipspace(stream)
  buf = ""
  c = "0"
  while not c.isspace():
    buf += stream.read(1)
    c = stream.peek(1)[0]
  return float(buf)

def rline(stream=stdin_stream):
  return stream.readline()[:-1]

def rints(num=None, stream=stdin_stream):
  return rsized(rint, num, stream=stream)

def rfloats(num=None, stream=stdin_stream):
  return rsized(rfloat, num, stream=stream)

def rwords(num=None, stream=stdin_stream):
  return rsized(rword, num, stream=stream)

def rlines(num=None, stream=stdin_stream):
  return rsized(rline, num, stream=stream)

def rsized(elem, num=None, stream=stdin_stream):
  if num is None:
    num = rint()
  return [elem(stream=stream) for _ in xrange(num)]

def rsquare(elem, num=None, stream=stdin_stream):
  if num is None:
    num = rint(stream=stream)
  return [rsized(elem, num, stream=stream) for _ in xrange(num)]

def rrect(elem, num=None, stream=stdin_stream):
  if num is None:
    num = (rint(stream=stream), rint(stream=stream))
  return [rsized(elem, num[1], stream=stream) for _ in xrange(num[0])]

__all__ = (
  "rint",
  "rfloat",
  "rword",
  "rline",
  "rints",
  "rfloats",
  "rwords",
  "rlines",
  "rsized",
  'rsquare',
  'rrect',
)
