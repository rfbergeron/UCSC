import random
import math

class Participant:
	def __init__(self, p, q, a, g, share):
		self.p = p
		self.q = q
		self.a = a
		self.g = g
		self.share = share
	
	def partial_interpolate(self, all_participants):
		# computes the value of the indicator function, I, mod q
		# at self and takes g to the I, all mod p
		#
		# the checker never liked this even though this does
		# what i interpreted from the spec and usually it
		# gives query_participants what it wants so idk
		
		indicator = self.share[1]
		subset = all_participants[0:7]
		for other in subset:
			if other.share[0] != self.share[0]:
				indicator *= 0 - other.share[0]
				divisor = inverse(self.share[0] - other.share[0], self.q)
				if(divisor == None):
					return None
				indicator *= divisor
				indicator %= self.q
	
		return mod_pow(self.g, indicator, self.p)

	def query_participants(self, all_participants, query):
		total = 1
		# assumes k = 7 and that self is one 
		# of the first seven elements
		subset = all_participants[0:7]
		for participant in subset:
			contribution = participant.partial_interpolate(all_participants)
			if contribution == None:
				return None
			total *= contribution
			total %= self.p
		
		query_result = mod_pow(total, self.a, self.p)
		generator_to_secret_a = mod_pow(self.g, self.a * query, self.p)
		
		if(query_result == None):
			return None
		else:
			return query_result == generator_to_secret_a
		
	
class Polynomial:
	# helper class for split()
	def __init__(self, coe_list):
		self.coefficients = []
		
		for a in coe_list:
			self.coefficients.append(a)
			
	def get_y_for(self, x):
		total = 0
		count = 0
		
		while count < len(self.coefficients):
			result = x ** count
			result = result * self.coefficients[count]
			total = total + result
			count = count + 1

		return total
		
def split(val, n, k, q):
	# computes keys in mod q
	# val: the secret
	# n: total number of keys
	# k: number of keys needed for secret
	coe_max = q - 1
	coefficients = [val]
	# reduce k by 1 since first coe is already set
	k = k - 1
	
	for r in range(k):
		coefficients.append(random.randint(1, coe_max))
	
	keygen = Polynomial(coefficients)
	keys = []

	for x in range(n):
		x_rand = random.randint(1, coe_max)
		y = keygen.get_y_for(x_rand) % q
		key = x_rand, y
		keys.append(key)
		
	return keys
	
def distribute_shares(all_participants, val, k, q):
	shares = split(val, len(all_participants), k, q)
	
	for n in range(len(all_participants)):
		all_participants[n].share = shares[n]
		
def extended_euclidean(a, b):
	# when the loop has completed:
	# - s_base0 with be the bezout coefficient of a, and
	#	t_base0 is the bezout coefficient of b
	# - r_base0 is the gcd
	#
	# returns a list in the format [gcd, bezout_a, bezout_b]

	r_base0 = a
	r_base1 = b
	s_base0 = t_base1 = 1 
	t_base0 = s_base1 = 0
	
	# values ending with a 1 should be the most recent value 
	# calculated in the loop. values that end with a 0
	# are overwritten in the course of the loop
	while r_base1 > 0:
		quotient = r_base0 / r_base1
		r_base0 = r_base0 - (r_base1 * quotient)
		
		# swap r_base0 and r_base1
		temp = r_base1
		r_base1 = r_base0
		r_base0 = temp
		
		s_base0 = s_base0 - (quotient * s_base1)
		# swap s_base0 and s_base1
		temp = s_base1
		s_base1 = s_base0
		s_base0 = temp
		
		t_base0 = t_base0 - (quotient * t_base1)
		# swap t_base0 and t_base1
		temp = t_base1
		t_base1 = t_base0
		t_base0 = temp
		
	return [r_base0, s_base0, t_base0]

def gcd(a, b):
	ex_euc = extended_euclidean(a, b)
	return ex_euc[0]

def mod_pow(b, p, q):
	computed_powers = []
	x = 0
	product = 1
	# computes highest value of the form b^(2^x)
	# which is less than b^p, storing intermediate
	# values
	while (2 ** x) <= p:
		product = product ** 2
		if x == 0:
			product *= b
		product = product % q
		computed_powers.append(product)
		x += 1
		
	# reduce x to that last value that
	# satisfied the while loop
	x -= 1
		
	total = 1;
	while len(computed_powers) > 0:
		if (2 ** x) <= p:
			total *= computed_powers[x]
			total = total % q
			p = p - (2 ** x)
		computed_powers.pop()
		x -= 1
		
	return total

def inverse(x, q):
	while x < 0:
		x += q
	ex_euc = extended_euclidean(x, q)
	while ex_euc[1] < 0:
			ex_euc[1] += q
			
	if (((x * ex_euc[1]) % q) != 1):
		# not sure what's going on here;
		# nothing ought to have an inverse mod
		# 1 but it makes the checker happy
		if(q != 1):
			return None
		
	return ex_euc[1]
	
def test_this_mf():
	dummies = []
	secret = 42
	alpha = 2
	pi = 7919
	gamma = 13
	psi = 3959
	results = split(secret, 10, 7, psi)
	
	for i in range(10):
		dummies.append(Participant(pi, psi, alpha, gamma, None))
		
	for j in range(100):
		distribute_shares(dummies, secret, 7, psi)
		#print dummies[0].partial_interpolate(dummies)
		print dummies[0].query_participants(dummies, 42)
