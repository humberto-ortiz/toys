#!/usr/bin/env python

# Toy RSA implementation
# Alex Roper
# aroper@umich.edu
#
# This is a toy proof of concept for fun. Please do not use for any actual
# encryption, there are a number of practical considerations relating to quality
# of the RNG, hardware security, armoring (padding), etc and no effort is made
# to address these.
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

# Point at which recursive bigint routines fall back to native Python math.
# This must be at least 4, and should ideally be the native word size.
NATIVE_MATH_MAX = 1 << 64

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

def modmul(x, y, n, base=None):
  # O(n^log_2(3)) algorithm
  #
  # (a + kc)(b + kd) = ab + k(ad + bc) + kkcd
  # (a + c)(b + d) = ab + ad + bc + cd
  #
  # let ab = a * b
  # let cd = c * d
  # let crossover = (a + c)(b + d) - ab - cd = ad + bc
  #
  # (a + kc)(b + kd) = ab + k * crossover + k * k * cd
  if x >= NATIVE_MATH_MAX or y >= NATIVE_MATH_MAX:
    if base is None:
      base = int(0.5 + math.log(max(x, y), 2))
    base /= 2

    # Split x and y into low and high nibbles
    base_mask = (1 << base) - 1
    a = x & base_mask
    c = x >> base

    b = y & base_mask
    d = y >> base

    # Compute intermediates, which each require one multiplication.
    ab = modmul(a, b, n)
    cd = modmul(c, d, n)
    crossover = (modmul((a + c), (b + d), n) - ab - cd) % n

    # Final result can be assembled with just shifts and adds.
    return (ab + (((cd << base) + crossover) << base)) % n
  else:
    return (x * y) % n

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
  """Very simple class to represent a message that can be [de]encoded through
     a key. This class is used to manage the dual forms of bytestring and number
     sequence. Numbers is a sequence of integers, base is the power of 2 the
     numbers are encoded modulo and must be a multiple of 8 (for simplicity).
     Caller is responsible for validity of numbers, length, and base -- numbers
     must be a sequence of integers modulo 2 ** base, and length must be the
     number of *bytes* to decode."""
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
       transformed by fxn. The result is NOT checked against the base for
       legality."""
    return Message(map(fxn, self.numbers), self.length, self.base)

class RSAPrivateKey(object):
  """Represents an RSA private key. Requires nbits > 2."""
  def __init__(self, nbits=DEFAULT_RSA_KEY_LENGTH):
    assert nbits > 2
    p = get_prime(nbits)
    q = p
    # Must halt because nbits > 2, meaning at least 2 primes are available. Thus,
    # the expected number of iterations is 2.
    while q == p:
      q = get_prime(nbits)
    
    # We need an encryption exponent that is relatively prime to
    # (p - 1) * (q - 1), which is true iff it has an inverse.
    secret_modulus = (p - 1) * (q - 1)
    self.d = None
    self.e = 1

    # The vast majority of primes - 1 are not multiples of 3, so this halts quick.
    while self.d is None and self.d != self.e:
      self.e += 1
      self.d = modinv(self.e, secret_modulus)
    
    self.N = p * q
    self.public = RSAPublicKey(self.N, self.e)

  def GetPublicKey(self):
    """Returns the private key's corresponding public key."""
    return self.public

  def Decrypt(self, message):
    """Decrypts a Message."""
    assert all((0 <= n < self.N for n in message.numbers))
    return message.Mapped(self.DecryptInteger)

  def DecryptInteger(self, number):
    """Decrypts a single number."""
    assert 0 <= number < self.N
    return modexp(number, self.d, self.N)

class RSAPublicKey(object):
  """Represents an RSA public key."""
  def __init__(self, N, e):
    self.N = N
    self.e = e

  def Encrypt(self, message):
    """Encrypts a message to the RSA key specified."""
    assert all((0 <= n < self.N for n in message.numbers))
    return message.Mapped(self.EncryptInteger)

  def EncryptInteger(self, number):
    """Encrypts a single number."""
    assert 0 <= number < self.N
    return modexp(number, self.e, self.N)
