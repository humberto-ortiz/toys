#!/usr/bin/env python

# Toy RSA implementation
# Alex Roper
# aroper@umich.edu
#
# This is a toy proof of concept for fun. Please do not use for any actual
# encryption, there are a number of practical considerations relating to quality
# of the RNG, hardware security, armoring (padding), block chaining, swapping
# out sensitive pages, etc and no effort is made to address these -- the attacks
# on the below Wikipedia page are all effective.
#
# Some examples:
# * http://en.wikipedia.org/wiki/RSA_(algorithm)#Attacks_against_plain_RSA
# * http://rdist.root.org/2009/10/06/why-rsa-encryption-padding-is-critical/
# * http://www.imc.org/ietf-openpgp/mail-archive/msg14307.html
# * http://rdist.root.org/2008/03/10/advances-in-rsa-fault-attacks/
# * http://web.eecs.umich.edu/~valeria/research/publications/DATE10RSA.pdf
#

import cPickle
import cStringIO
import math
import optparse
import random
import sys

PRIMALITY_ITERATIONS = 100
DEFAULT_RSA_KEY_LENGTH = 512

# Point at which recursive bigint routines fall back to native Python math.
# This must be at least 4, and should ideally be the native word size.
NATIVE_MATH_MAX = 1 << 64

class RSAException(Exception):
  pass

class DecodeRangeError(RSAException):
  pass

class FailedToLoadKeyfile(RSAException):
  pass

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
     sequence. Numbers is a sequence of integers, modulo is the mod we work in.
     Caller is responsible for validity of numbers. The encoding used here is
     extremely simple and inefficient in space use."""
  #TODO: Consider lazy implementation for better large-file performance
  def __init__(self, numbers, modulo, overflow):
    self.numbers = numbers
    self.modulo = modulo
    self.overflow = overflow

  @classmethod
  def Encode(klass, data, modulo):
    """Converts the data to a byte sequence in multiples of 8 (could be more
       efficient but code complexity not worth it)."""
    assert modulo > 2 ** 8
    base = klass._base_from_modulo(modulo)
    bytes_per_item = base / 8 - 1
    assert bytes_per_item >= 1
    num_items = int(math.ceil(float(len(data)) / bytes_per_item))
    items = []
    overflow = len(data) % bytes_per_item
    if overflow == 0:
      overflow = bytes_per_item
    for i in range(num_items):
      num = 0
      for byte in data[i * bytes_per_item:(i + 1) * bytes_per_item]:
        num = ((num << 8) + ord(byte))
      if (i + 1) * bytes_per_item > len(data):
        num <<= 8 * ((i + 1) * bytes_per_item - len(data))
      assert 0 <= num < 256 ** bytes_per_item
      items.append(num)
    return Message(items, modulo, overflow)

  def Decode(self):
    """Converts the message contents to bytes. Requires that all numbers are
       modulo the base (IE, you can't decode arbitrary messages, such as the
       output of encryption. Throws DecodeRangeError in such cases. This is
       necessary because inputs may be arbitrary data, but encrypted data will
       be modulo some number N, which is represented by ceil(log(N, 2)) bits."""
    base = self._base_from_modulo(self.modulo)
    bytes_per_item = base / 8 - 1
    assert bytes_per_item >= 1
    my_data = []

    for item in self.numbers:
      if not 0 <= item < 256 ** bytes_per_item:
        raise DecodeRangeError()
      for i in reversed(xrange(bytes_per_item)):
        my_data.append(chr((((0xFF << (i * 8)) & item) >> (i * 8))))
    if self.overflow < bytes_per_item:
      return "".join(my_data[:-(bytes_per_item - self.overflow)])
    else:
      return "".join(my_data)

  def Mapped(self, fxn):
    """Returns a new message that is the result of the old with numbers
       transformed by fxn. The result is NOT checked against the base for
       legality."""
    return Message(map(fxn, self.numbers), self.modulo, self.overflow)

  @staticmethod
  def _base_from_modulo(modulo):
    return int(math.ceil(math.log(modulo, 2) / 8.)) * 8

  def __eq__(self, other):
    return (self.numbers == other.numbers and self.overflow == other.overflow and
            self.modulo == other.modulo)

  def __ne__(self, other):
    return not (self == other)

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

    # HOZ - Store secret stuff for working out examples.
    self.p = p
    self.q = q
    self.phi = secret_modulus

  def GetPublicKey(self):
    """Returns the private key's corresponding public key."""
    return self.public

  def Decrypt(self, message):
    """Decrypts a Message."""
    return message.Mapped(self.DecryptInteger)

  def DecryptInteger(self, number):
    """Decrypts a single number."""
    assert 0 <= number < self.N
    return modexp(number, self.d, self.N)

  def __eq__(self, other):
    return (self.N == other.N and self.e == other.e and self.d == other.d)

  def __ne__(self, other):
    return not (self == other)

class RSAPublicKey(object):
  """Represents an RSA public key."""
  def __init__(self, N, e):
    self.N = N
    self.e = e

  def Encrypt(self, message):
    """Encrypts a message to the RSA key specified."""
    return message.Mapped(self.EncryptInteger)

  def EncryptInteger(self, number):
    """Encrypts a single number."""
    assert 0 <= number < self.N
    return modexp(number, self.e, self.N)

  def __eq__(self, other):
    return (self.N == other.N and self.e == other.e)
 
  def __ne__(self, other):
    return not (self == other)

def _load_key(fileio, cast=False):
  """Attempt to load a key from a file. Will convert private keys to public if
     cast is True."""
  try:
    key = cPickle.load(fileio)
    if cast and not hasattr(key, "Encrypt"):
      key = key.GetPublicKey()
  except Exception:
    raise FailedToLoadKeyfile("Failed to load key file.")

  return key

def encrypt(args):
  """Encrypt command line option."""
  key = _load_key(open(args[1], "rb"), True)

  infile = open(args[2], "rb") if args[2] != "-" else sys.stdin
  outfile = open(args[3], "wb") if args[3] != "-" else sys.stdout

  msg = key.Encrypt(Message.Encode(infile.read(), key.N))

  cPickle.dump((key, msg), outfile, -1)
  if infile is not sys.stdin:
    infile.close()
  if outfile is not sys.stdout:
    outfile.close()

def decrypt(args):
  """Decrypt command line option"""
  key = _load_key(open(args[1], "rb"))
  if not hasattr(key, "Decrypt"):
    print >> sys.stderr, ("This key is not capable of decryption."
                          " You must provide a private key.")
    return

  infile = open(args[2], "rb") if args[2] != "-" else sys.stdin
  outfile = open(args[3], "wb") if args[3] != "-" else sys.stdout

  enc_key, msg = cPickle.load(infile)
  if enc_key != key:
    print >> sys.stderr, ("This key does not match the key this message was "
                          "encrypted to.")
    return

  outfile.write(key.Decrypt(msg).Decode())
  if infile is not sys.stdin:
    infile.close()
  if outfile is not sys.stdout:
    outfile.close()

def keygen(args):
  """Keygen command line option"""
  nbits = int(args[1])
  if nbits < 8:
    print >> sys.stderr, "Private key must be at least 8 bits long!"
    return
  key = RSAPrivateKey(nbits)
  if args[2] == "-":
    cPickle.dump(key, sys.stdout, -1)
  else:
    cPickle.dump(key, open(args[2], "wb"), -1)
    cPickle.dump(key.GetPublicKey(),
                 open(args[2] + ".pub", "wb"), -1)

def publicextract(args):
  """Public key extract command line option"""
  infile = open(args[1], "rb") if args[1] != "-" else sys.stdin
  outfile = open(args[2], "wb") if args[2] != "-" else sys.stdout
  key = _load_key(infile)
  if not hasattr(key, "GetPublicKey"):
    print >> sys.stderr, ("The key you provided is not capable of providing a "
                          "public key. Is it already a public key?")
    return
  cPickle.dump(key.GetPublicKey(), outfile, -1)

MODES = {"encrypt":encrypt,
         "decrypt":decrypt,
         "keygen":keygen,
         "publicextract":publicextract}

def main():
  parser = optparse.OptionParser()
  opts, args = parser.parse_args()

  # Dispatch the command.
  if len(args) >= 1 and args[0] in MODES:
    MODES[args[0]](args)
  else:
    print >> sys.stderr, \
             "Usage: %s [encrypt|decrypt] key infile outfile" % sys.argv[0] + \
             "\n       %s keygen nbits outfile" % sys.argv[0] + \
             "\n       %s publicextract in_privatekeyfile out_publickeyfile" \
               % sys.argv[0]
    return

if __name__ == "__main__":
  main()

