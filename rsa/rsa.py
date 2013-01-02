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
DEFAULT_RSA_KEY_LENGTH = 512
ENCRYPTION_EXPONENT = 3

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
  # Although elegant algorithmically, the n^2 iterative procedure given below
  # is no match for Python's built in library, which likely uses the FFT nlogn.
  return (x * y) % n
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
    
  for i in range(N):
    a = random.randint(1, x - 1)
    if not prime_test_one_base(a, t, u, x):
      return False
  else:
    return True

def get_prime(nbits):
  """Gets a prime that is n bits in length. Requires nbits > 2."""
  assert nbits > 2
  number = 4
  while not isprime(number):
    number = random.randint(2 ** (nbits - 1), 2 ** nbits - 1)
  return number

class Message(object):
  """Represents a message that can be [de]encoded through a key. This class is
     used to manage the dual forms of bytestring and number sequence."""
  def __init__(self, numbers, length, base):
    self.numbers = numbers
    self.length = length
    self.base = base

  @classmethod
  def FromBytes(klass, data, base):
    """Converts the data to a byte sequence in multiples of 8 (could be more
       efficient but code complexity not worth it)."""
    bytes_per_item = base / 8
    assert bytes_per_item >= 1
    num_items = int(math.ceil(float(len(data)) / bytes_per_item))
    items = []
    for i in range(num_items):
      num = 0
      for byte in data[i * bytes_per_item:(i + 1) * bytes_per_item]:
        num = ((num << 8) + ord(byte))
      if (i + 1) * bytes_per_item > len(data):
        num <<= 8 * ((i + 1) * bytes_per_item - len(data))
      items.append(num)
    return Message(items, len(data), base)

  def AsBytes(self):
    """Converts the message contents to bytes.
       than 2 ** base, where base is >= 8."""
    bytes_per_item = self.base / 8
    assert bytes_per_item >= 1
    my_data = []
    for item in self.numbers:
      for i in reversed(xrange(bytes_per_item)):
        my_data.append(chr((((0xFF << (i * 8)) & item) >> (i * 8))))
    return ''.join(my_data[:self.length])

  def Mapped(self, fxn):
    """Returns a new message that is the result of the old with numbers
       transformed by fxn."""
    return Message(map(fxn, self.numbers), self.length, self.base)

class RSAPrivateKey(object):
  """Represents an RSA private key."""
  def __init__(self, nbits=DEFAULT_RSA_KEY_LENGTH):
    p = get_prime(nbits)
    q = get_prime(nbits)
    self.e = ENCRYPTION_EXPONENT
    self.d = modinv(self.e, (p - 1) * (q - 1))
    self.N = p * q
    self.public = RSAPublicKey(self.N, self.e)

  def GetPublicKey(self):
    """Returns the private key's corresponding public key."""
    return self.public

  def Decrypt(self, message):
    """Decrypts a Message."""
    print message.numbers
    assert all((0 <= n < self.N for n in message.numbers))
    return message.Mapped(lambda n: modexp(n, self.d, self.N))

class RSAPublicKey(object):
  """Represents an RSA public key."""
  def __init__(self, N, e):
    self.N = N
    self.e = e

  def Encrypt(self, message):
    """Encrypts a message to the RSA key specified."""
    assert all((0 <= n < self.N for n in message.numbers))
    return message.Mapped(lambda n: modexp(n, self.e, self.N))
