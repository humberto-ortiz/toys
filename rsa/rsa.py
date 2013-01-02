#!/usr/bin/env python

# Toy RSA implementation
# Alex Roper
# aroper@umich.edu
#
# This is a toy proof of concept for fun. Please do not use for any actual
# encryption, there are a number of practical considerations relating to quality
# of the RNG, hardware security, armoring, etc and no effort is made to address
# these.
#
# Some examples:
# * http://rdist.root.org/2009/10/06/why-rsa-encryption-padding-is-critical/
# * http://www.imc.org/ietf-openpgp/mail-archive/msg14307.html
# * http://rdist.root.org/2008/03/10/advances-in-rsa-fault-attacks/
# * http://web.eecs.umich.edu/~valeria/research/publications/DATE10RSA.pdf
#

import math
import random

PRIMALITY_ITERATIONS = 100

def _extended_euclidean(a, b):
  """Helper function that runs the extended Euclidean algorithm, which
     finds x and y st a * x + b * y = gcd(a, b). Used for gcd and modular
     division. Returns x, y, gcd(a, b)"""
  flip = b > a
  if flip:
    a, b = b, a
  if b == 0:
    return 1, 0, a
  x, y, gcd = _extended_euclidean(b, a % b)
  ny = y
  nx = x - (a / b) * y
  assert a * ny + b * nx == gcd
  if flip:
    ny, nx = nx, ny
  return ny, nx, gcd

def gcd(a, b):
  """Returns the greatest common denominator of a, b."""
  assert a >= 0 and b >= 0
  _, _, d = _extended_euclidean(a, b)
  return d

def modmul(x, y, n):
  """Computes x * y mod n"""
  total = 0
  while y > 0:
    if y % 2 == 1:
      total = (total + x) % n
    y /= 2
    x *= 2
  return total

def modinv(x, n):
  """Returns the inverse of x mod n if it exists, or None if not."""
  a, b, gcd = _extended_euclidean(x, n)
  if gcd != 1:
    return None
  else:
    return a % n

def modexp(x, y, n):
  """Computes (x ** y) mod n"""
  z = 1
  for i in reversed(range(int(math.ceil(math.log(y + 1, 2))))):
    z_squared = modmul(z, z, n)
    if (1 << i) & y:
      z = modmul(x, z_squared, n)
    else:
      z = z_squared
  return z % n

def isprime(x, N=PRIMALITY_ITERATIONS):
  """Uses the primality test of Rabin and Miller (based on Fermat's little
     theorem with extensions for Carmichael numbers). Probability of false
     positive is (1/4) ** PRIMALITY_ITERATIONS."""
  assert x >= 0
  # Special cases
  if x == 0 or x == 1:
    return False

  # Find t and u such that u * (2 ** t) == x - 1
  t = 0
  u = x - 1
  while u % 2 == 0:
    t += 1
    u /= 2
  assert (2 ** t) * u == x - 1

  def prime_test_one_base(a, t, u, x):
    """Tests one 0 < a < x by repeated squaring. Returns False
       if a contradiction (nontrivial square root or Fermat test), True
       otherwise."""
    assert 0 < a < x
    # Repeatedly square mod x to compute a^N-1 mod x so we can check for
    # non-trivial square roots, which is necessary to detect Carmichael numbers.
    val = modexp(a, u, x)
    found_first_one = (val == 1)
    for i in range(t):
      new_val = modexp(val, 2, x)
      # If there exists a non-trivial square root, x must be composite provided
      # it is the first time 1 appears in the sequence.
      if new_val == 1 and not found_first_one:
        found_first_one = True
        if val != x - 1:
          return False
      val = new_val
    return val == 1
    
  for a in random.sample(xrange(1, x), min(x - 1, N)):
    if not prime_test_one_base(a, t, u, x):
      return False
  else:
    return True
