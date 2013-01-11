import contextlib
import cPickle
import mock
import random
import StringIO
import unittest

import rsa

class test__extended_euclidean(unittest.TestCase):
  def test_coprime(self):
    for a, b in [(25, 11), (9, 13), (8, 15)]:
      x, y, gcd = rsa._extended_euclidean(a, b)
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
  def setUp(self):
    self.native_val = rsa.NATIVE_MATH_MAX
    rsa.NATIVE_MATH_MAX = 4

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
    # -1 * -1 == 1 (unless you're Gene Ray)
    self.assertEquals(rsa.modmul(4, 4, 5), 1)

  def test_stress(self):
    R = random.Random(34)
    for i in range(50):
      x = R.randint(0, 100)
      y = R.randint(0, 100)
      n = R.randint(1, 100)
      self.assertEquals(rsa.modmul(x, y, n), (x * y) % n)

  def tearDown(self):
    rsa.NATIVE_MATH_MAX = self.native_val

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
       luck of choosing a relatively composite to n. Thus, we mock out random's
       sample to ensure no factors of n are tried as values as a."""
    numbers = dict(((561, (3, 11, 17)),
                    (62745, (3, 5, 47, 89)),
                    (162401, (17, 41, 233)),
                    (314821, (13, 61, 397)),
                    (1024651, (19, 199, 271)),
                    (31691713801, (11, 47, 1033, 59341)),
                    (384486837505, (5, 67, 113, 463, 21937)),
                    (989017417441, (139, 691, 829, 12421))))

    def pessimal_sample_gen(bad_choices):
      """Note -- not a general solution; badle behaved when count greater than
         number of distinct items or duplicates are desired. Works perfectly
         here though. Las Vegas performance."""
      unpatched_sample = random.Random(34).sample
      def sampler(items, count):
        gulp = min(len(items), 2 * count)
        sample = set(unpatched_sample(items, gulp)).difference(bad_choices)
        while len(sample) < count:
          sample.update(unpatched_sample(items, gulp))
          sample.difference_update(bad_choices)
        return unpatched_sample(sample, count)
      return sampler

    for n, factors in numbers.iteritems():
      sampler = pessimal_sample_gen(factors)
      with mock.patch("random.sample") as mocked_sample:
        mocked_sample.side_effect=sampler
        self.assertFalse(rsa.isprime(n))

class test_rsa_get_prime(unittest.TestCase):
  def test_edge(self):
    with mock.patch("random.randint") as random_mock:
      random_mock.side_effect=random.Random(34).randint
      primes = [rsa.get_prime(3) for _ in range(10)]
      self.assertTrue(all((int(p) in (2, 3, 5, 7) for p in primes)))

      for b in (3, 4, 5, 10, 20):
        for i in range(3):
          prime = rsa.get_prime(b)
          self.assertTrue(rsa.isprime(prime))
          self.assertTrue(prime < 2 ** b)

class test_Message(unittest.TestCase):
  def test_message_edge(self):
    self.assertEquals(rsa.Message([72, 69, 76, 76, 79], 2 ** 16, 1).Decode(),
                                  "HELLO")

    self.assertEquals(rsa.Message([18501, 19532, 20257], 2 ** 24, 2).Decode(),
                                  "HELLO!")

    self.assertEquals(rsa.Message([310400253952], 2 ** 48, 3).Decode(),
                                  "HEL")

    self.assertEquals(rsa.Message.Encode("Hey", 2 ** 32).Decode(), "Hey")
  
    with self.assertRaises(rsa.DecodeRangeError):
      rsa.Message([65537], 2 ** 24, 2).Decode()

  def test_savelength_exhaustive(self):
    """Regression test against even division cases."""
    data = "This was a triumph!!abcd"
    for i in range(len(data)):
      for base in (16, 24, 32):
        self.assertEquals(
            data[:i], rsa.Message.Encode(data[:i], 2 ** base).Decode())

  def test_Encode(self):
    for data in ("This was a triumph.", "\xFF\xFF"):
      for base in (9, 16, 24, 32, 40):
        # Operations must be inverses.
        self.assertEquals(rsa.Message.Encode(data, 2 ** base).Decode(),
                          data)

  def test_Mapped(self):
    msg = rsa.Message([72, 69, 76, 76, 79], 2 ** 16, 1)
    self.assertEquals(msg.Mapped(lambda x: x + 2).Decode(), "JGNNQ")

  def test_high_bits_encoding_regression(self):
    """Must be able to encode messages larger than n in any base."""
    value = "\xFF\xFF\xFF\xFF\xFF"

    prime = 58579
    msg = rsa.Message.Encode(value, prime).Mapped(lambda n: n % prime)
    self.assertEquals(msg.Decode(), value)

  def test_eq_neq(self):
    msg1a = rsa.Message([18501, 19532, 20257], 2 ** 24, 2)
    msg1b = rsa.Message([18501, 19532, 20257], 2 ** 24, 2)
    msg2 = rsa.Message.Encode("Hello!", 2 ** 24)

    self.assertEquals(msg1a, msg1b)
    self.assertNotEquals(msg1a, msg2)
    self.assertEquals(msg2, msg2)

class test_RSA(unittest.TestCase):
  def test_creation_edge(self):
    with mock.patch("random.randint") as random_mock:
      random_mock.side_effect=random.Random(34).randint
      for nbits in (3, 4, 5, 10, 20):
        K = rsa.RSAPrivateKey(nbits)
        k = K.GetPublicKey()

        # Verify that small edge case keys can decrypt successfully all possible
        # values.
        for data in range(nbits):
          self.assertEquals(K.DecryptInteger(k.EncryptInteger(data)), data)

  def test_message_crypt(self):
    with mock.patch("random.randint") as random_mock:
      random_mock.side_effect=random.Random(34).randint
      value = "Hello world! How are you this fine day? I'm doing just great!"
      for nbits in (32, 72, 64, 136):
        msg = rsa.Message.Encode(value, 2 ** nbits)
        K = rsa.RSAPrivateKey(nbits)
        k = K.GetPublicKey()
        self.assertEquals(K.Decrypt(k.Encrypt(msg)).Decode(), value)

  def test_high_bits_encryption_regression(self):
    """Must be able to crypt messages larger than n in any base."""
    value = "\xFF\xFF\xFF\xFF\xFF"
    with mock.patch("random.randint") as random_mock:
      random_mock.side_effect=random.Random(34).randint

      for nbits in (10, 16, 20):
        K = rsa.RSAPrivateKey(10)
        k = K.GetPublicKey()

        msg = rsa.Message.Encode(value, K.N)

        self.assertEquals(K.Decrypt(k.Encrypt(msg)).Decode(), value)
  
  def test_eq_neq(self):
    with mock.patch("random.randint") as random_mock:
      random_mock.side_effect=random.Random(34).randint
      key1a = rsa.RSAPrivateKey(10)
      random_mock.side_effect=random.Random(34).randint
      key1b = rsa.RSAPrivateKey(10)
      key2 = rsa.RSAPrivateKey(10)

      self.assertTrue(key1a == key1b)
      self.assertTrue(key1a is not key1b)
      self.assertFalse(key1a != key1b)
      self.assertTrue(key1a != key2)
      self.assertFalse(key1b == key2)

      self.assertTrue(key1a.GetPublicKey() == key1b.GetPublicKey())
      self.assertTrue(key1a.GetPublicKey() is not key1b.GetPublicKey())
      self.assertFalse(key1a.GetPublicKey() != key1b.GetPublicKey())
      self.assertTrue(key1a.GetPublicKey() != key2.GetPublicKey())
      self.assertFalse(key1b.GetPublicKey() == key2.GetPublicKey())

# Begin CLI tests

def make_fs_side_effect(tester, files):
  """Helper to create a mock filesystem from a dict of file data."""
  def open_file(fn, mode, buf=-1):
    tester.assertIn(fn, files)
    fd, good_mode = files[fn]
    tester.assertEquals(mode, good_mode)
    fd.close = lambda: None
    return fd
  return open_file

class test_load_key(unittest.TestCase):
  def test_load_key(self):
    with mock.patch("random.randint") as random_mock:
      random_mock.side_effect=random.Random(34).randint
      key = rsa.RSAPrivateKey(12)

      priv_pck = StringIO.StringIO(cPickle.dumps(key, -1))
      pub_pck = StringIO.StringIO(cPickle.dumps(key.GetPublicKey(), -1))

      self.assertEquals(rsa._load_key(pub_pck), key.GetPublicKey())
      self.assertEquals(rsa._load_key(priv_pck), key)

      priv_pck.seek(0)

      self.assertEquals(rsa._load_key(priv_pck, cast=True), key.GetPublicKey())

      self.assertRaises(rsa.FailedToLoadKeyfile, rsa._load_key, pub_pck)

class test_encrypt_decrypt(unittest.TestCase):
  def setUp(self):
    with mock.patch("random.randint") as rmock:
      rmock.side_effect=random.Random(34).randint
      key = rsa.RSAPrivateKey(12)
      badkey = rsa.RSAPrivateKey(10)
      good = (key.GetPublicKey(),
              rsa.Message([1385493, 12416809, 4364045], 13021759, 1))
      files = {"pub":(StringIO.StringIO(cPickle.dumps(key.GetPublicKey(),
                                                        -1)), "rb"),
               "priv":(StringIO.StringIO(cPickle.dumps(key, -1)), "rb"),
               "badkey":(StringIO.StringIO(cPickle.dumps(badkey, -1)), "rb"),
               "in":(StringIO.StringIO("Hello"), "rb"),
               "good":(StringIO.StringIO(cPickle.dumps(good)), "rb"),
               "enc1":(StringIO.StringIO(), "wb"),
               "enc2":(StringIO.StringIO(), "wb"),
               "dec":(StringIO.StringIO(), "wb")}
      self.key = key
      self.opener_fxn = make_fs_side_effect(self, files)
      self.files = files

  def test_encrypt(self):
    with mock.patch("__builtin__.open") as opener:
      opener.side_effect = self.opener_fxn
      rsa.encrypt(["", "pub", "in", "enc1"])
      self.files["in"][0].seek(0)
      rsa.encrypt(["", "priv", "in", "enc2"])

      enc1 = cPickle.loads(self.files["enc1"][0].getvalue())
      enc2 = cPickle.loads(self.files["enc2"][0].getvalue())
      good = cPickle.loads(self.files["good"][0].getvalue())
      self.assertEquals(enc1, good)
      self.assertEquals(enc2, good)

  def test_decrypt(self):
    with mock.patch("__builtin__.open") as opener:
      opener.side_effect = self.opener_fxn
      rsa.decrypt(["", "priv", "good", "dec"])

      dec = self.files["dec"][0].getvalue()
      good = self.files["in"][0].getvalue()
      self.assertEquals(dec, good)

  def test_wrong_decrypt(self):
    with contextlib.nested(mock.patch("__builtin__.open"),
                           mock.patch("sys.stderr")) as (opener, stderr):
      opener.side_effect = self.opener_fxn
      stderr_backing = StringIO.StringIO()
      stderr.write = stderr_backing.write
      rsa.decrypt(["", "badkey", "good", "dec"])
      self.assertRegexpMatches(stderr_backing.getvalue(), "does not match the key")
      self.assertEquals(self.files["dec"][0].getvalue(), "")

  def test_pub_decrypt(self):
    with contextlib.nested(mock.patch("__builtin__.open"),
                           mock.patch("sys.stderr")) as (opener, stderr):
      opener.side_effect = self.opener_fxn
      stderr_backing = StringIO.StringIO()
      stderr.write = stderr_backing.write
      rsa.decrypt(["", "pub", "good", "dec"])
      self.assertRegexpMatches(stderr_backing.getvalue(), "not capable of decryption")
      self.assertEquals(self.files["dec"][0].getvalue(), "")

class test_keygen(unittest.TestCase):
  def setUp(self):
    with mock.patch("random.randint") as rmock:
      rmock.side_effect=random.Random(34).randint
      self.key = rsa.RSAPrivateKey(12)
      files = {"key":(StringIO.StringIO(), "wb"),
               "key.pub":(StringIO.StringIO(), "wb")}
      self.opener_fxn = make_fs_side_effect(self, files)
      self.files = files

  def test_keygen_stdout(self):
    with contextlib.nested(mock.patch("sys.stdout"),
                           mock.patch("random.randint")) as (cout, rmock):
      rmock.side_effect=random.Random(34).randint
      stdout = StringIO.StringIO()
      cout.write = stdout.write
      rsa.keygen(["", "12", "-"])
      self.assertEquals(cPickle.loads(stdout.getvalue()).GetPublicKey(), 
                        self.key.GetPublicKey())
      self.assertEquals(cPickle.loads(stdout.getvalue()), self.key)

  def test_keygen(self):
    with contextlib.nested(mock.patch("__builtin__.open"),
                           mock.patch("random.randint")) as (opener, rmock):
      rmock.side_effect=random.Random(34).randint
      opener.side_effect = self.opener_fxn
      rsa.keygen(["", "12", "key"])
      self.assertEquals(cPickle.loads(self.files["key"][0].getvalue()).GetPublicKey(), 
                        self.key.GetPublicKey())
      self.assertEquals(cPickle.loads(self.files["key"][0].getvalue()), 
                        self.key)

  def test_too_short(self):
    with contextlib.nested(mock.patch("__builtin__.open"),
                           mock.patch("sys.stderr"),
                           mock.patch("random.randint")) as (opener, cerr, rmock):
      stderr = StringIO.StringIO()
      rmock.side_effect=random.Random(34).randint
      opener.side_effect = self.opener_fxn
      cerr.write = stderr.write

      rsa.keygen(["", "7", "key"])
      self.assertEquals(self.files["key"][0].getvalue(), "")

      rmock.side_effect=random.Random(34).randint
      rsa.keygen(["", "8", "key"])
      rmock.side_effect=random.Random(34).randint
      shortkey = rsa.RSAPrivateKey(8)
      self.assertEquals(cPickle.loads(self.files["key"][0].getvalue()).GetPublicKey(), 
                        shortkey.GetPublicKey())
      self.assertEquals(cPickle.loads(self.files["key"][0].getvalue()), shortkey)
      self.assertEquals(cPickle.loads(self.files["key.pub"][0].getvalue()),
                        shortkey.GetPublicKey())
                        

class test_publicextract(unittest.TestCase):
  def setUp(self):
    with mock.patch("random.randint") as rmock:
      rmock.side_effect=random.Random(34).randint
      key = rsa.RSAPrivateKey(12)
      files = {"pub":(StringIO.StringIO(cPickle.dumps(key.GetPublicKey(),
                                                        -1)), "rb"),
               "priv":(StringIO.StringIO(cPickle.dumps(key, -1)), "rb"),
               "out":(StringIO.StringIO(), "wb")}
      self.key = key
      self.opener_fxn = make_fs_side_effect(self, files)
      self.files = files

  def test_extract_pub(self):
    with mock.patch("__builtin__.open") as opener:
      opener.side_effect = self.opener_fxn
      rsa.publicextract(["", "priv", "out"])
      self.assertEquals(self.files["pub"][0].getvalue(),
                        self.files["out"][0].getvalue())

  def test_extract_pub_fail(self):
    with contextlib.nested(mock.patch("__builtin__.open"),
                           mock.patch("sys.stderr")) as (opener, stderr_mock):
      stderr = StringIO.StringIO()
      stderr_mock.write = stderr.write
      opener.side_effect = self.opener_fxn
      rsa.publicextract(["", "pub", "out"])
      self.assertEquals(self.files["out"][0].getvalue(), "")
      self.assertRegexpMatches(stderr.getvalue(), "not capable of")
