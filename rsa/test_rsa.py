import random
import unittest

import rsa

class test__extended_euclidean(unittest.TestCase):
  def test_coprime(self):
    for a, b in [(25, 11), (9, 13), (8, 15)]:
      x, y, gcd = rsa._extended_euclidean(a, b)
      print a, b, x, y, gcd
      self.assertEquals(a * x + b * y, gcd)

  def test_not_coprime(self):
    x, y, gcd = rsa._extended_euclidean(3, 9)
    self.assertEquals(gcd, 3)
    x, y, gcd = rsa._extended_euclidean(33, 22)
    self.assertEquals(gcd, 11)

  def test_flip(self):
    for a, b in [(15, 8), (7, 13), (10, 15), (10, 21)]:
      x1, y1, gcd1 = rsa._extended_euclidean(a, b)
      y2, x2, gcd2 = rsa._extended_euclidean(b, a)
      self.assertTupleEqual((x1, y1, gcd1), (x2, y2, gcd2))

class test_gcd(unittest.TestCase):
  """Mostly tested in tests for extended euclidean."""
  def test_trivial(self):
    self.assertEquals(rsa.gcd(3, 4), 1)
    self.assertEquals(rsa.gcd(4, 3), 1)
    self.assertEquals(rsa.gcd(10, 25), 5)
    self.assertEquals(rsa.gcd(11, 17), 1)
    self.assertEquals(rsa.gcd(561, 253), 11)

class test_modmul(unittest.TestCase):
  def test_edge_nomod(self):
    self.assertEquals(rsa.modmul(0, 0, 100), 0)
    self.assertEquals(rsa.modmul(0, 1, 100), 0)
    self.assertEquals(rsa.modmul(1, 0, 100), 0)
    self.assertEquals(rsa.modmul(1, 1, 100), 1)

    self.assertEquals(rsa.modmul(1, 50, 100), 50)
    self.assertEquals(rsa.modmul(50, 1, 100), 50)

  def test_small_exhaustive_nomod(self):
    for a in range(10):
      for b in range(10):
        self.assertEquals(rsa.modmul(a, b, 100), a * b)
  
  def test_mod_edge(self):
    self.assertEquals(rsa.modmul(3, 3, 1), 0)
    self.assertEquals(rsa.modmul(3, 3, 2), 1)
    self.assertEquals(rsa.modmul(3, 3, 3), 0)
    self.assertEquals(rsa.modmul(3, 3, 4), 1)

    # 1 * -1 == -1
    self.assertEquals(rsa.modmul(1, 4, 5), 4)
    # -1 * -1 == 1
    self.assertEquals(rsa.modmul(4, 4, 5), 1)

  def test_stress(self):
    R = random.Random(34)
    for i in range(50):
      x = R.randint(0, 100)
      y = R.randint(0, 100)
      n = R.randint(1, 100)
      self.assertEquals(rsa.modmul(x, y, n), (x * y) % n)

class test_rsa_modinv(unittest.TestCase):
  def test_simple(self):
    self.assertEquals(rsa.modinv(3, 4), 3)
    self.assertIsNone(rsa.modinv(2, 4))
    self.assertEquals(rsa.modinv(1, 2), 1)
    self.assertEquals(rsa.modinv(1, 3), 1)
    self.assertEquals(rsa.modinv(1, 4), 1)
  
  def test_stress_prime_identity(self):
    for i in range(20):
      prime = rsa.isprime(i)
      for j in range(1, i):
        inv = rsa.modinv(j, i)
        if prime:
          self.assertIsNotNone(inv)
        if inv is not None:
          self.assertEquals(rsa.modmul(inv, j, i), 1)

class test_rsa_modexp(unittest.TestCase):
  def test_edge_nomod(self):
    self.assertEquals(rsa.modexp(0, 0, 100), 1)
    self.assertEquals(rsa.modexp(0, 1, 100), 0)
    self.assertEquals(rsa.modexp(0, 10, 100), 0)

    self.assertEquals(rsa.modexp(1, 0, 100), 1)
    self.assertEquals(rsa.modexp(1, 1, 100), 1)
    self.assertEquals(rsa.modexp(1, 13, 100), 1)

    self.assertEquals(rsa.modexp(5, 0, 100), 1)
    self.assertEquals(rsa.modexp(5, 1, 100), 5)
    self.assertEquals(rsa.modexp(5, 2, 100), 25)

  def test_small_powers_nomod(self):
    for b in (2, 5):
      for i in range(10):
        self.assertEquals(rsa.modexp(b, i, 10000000), b ** i)

  def test_mod_edge(self):
    self.assertEquals(rsa.modexp(5, 2, 1), 0)
    self.assertEquals(rsa.modexp(0, 0, 1), 0)
    self.assertEquals(rsa.modexp(0, 1, 1), 0)
    self.assertEquals(rsa.modexp(1, 0, 1), 0)
    self.assertEquals(rsa.modexp(1, 1, 1), 0)
    self.assertEquals(rsa.modexp(5, 2, 1), 0)

  def test_mod_parity(self):
    for i in range(1, 20):
      self.assertEquals(rsa.modexp(2, i, 2), 0)
      self.assertEquals(rsa.modexp(3, i, 2), 1)

  def test_stress(self):
    R = random.Random(34)
    for i in range(50):
      x = R.randint(0, 100)
      y = R.randint(0, 100)
      n = R.randint(1, 100)
      self.assertEquals(rsa.modexp(x, y, n), (x ** y) % n)

class test_rsa_isprime(unittest.TestCase):
  def test_first_few_prime(self):
    for i in (2, 3, 5, 7, 11):
      self.assertTrue(rsa.isprime(i))

  def test_first_few_notprime(self):
    for i in (0, 1, 4, 6, 8, 9, 10):
      self.assertFalse(rsa.isprime(i))

  def test_carmichael(self):
    """Carmichael numbers are composite numbers for which a ** (n - 1) = 1 mod n
       for all a relatively prime to n. For all other composite numbers, at
       least 1/2 of 0 < a < n will violate Fermat's little theorem, but for
       Carmichaels we need to test a number not relatively prime to n. Thus, to
       test Rabin and Miller's repeated squaring extension, we need to choose
       few enough a (for small numbers) that we don't pass the test due to the
       luck of choosing a relatively composite to n. Hence the seed value and
       N override in this test."""
    random.seed(5)
    for i in (561, 62745, 162401, 314821, 1024651, 31691713801, 384486837505,
              989017417441):
      self.assertFalse(rsa.isprime(i, 5))
