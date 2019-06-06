import random
from fractions import Fraction

class Polynomial:
	def __init__(self, coe_list):
		self.coefficients = coe_list[:]
				
	def get_y_for(self, x):
		total = 0
		count = 0
		
		while count < len(self.coefficients):
			result = x ** count
			result = result * self.coefficients[count]
			total = total + result
			count = count + 1

		return total

def split(val, n, k):
	coe_max = abs(val.numerator)
	coefficients = [Fraction(val, 1)]
	k = k - 1
	n = n + 1
	
	for r in range(k):
		coefficients.append(Fraction(random.randint(1, coe_max), random.randint(1, coe_max)))
		
	#print coefficients
	keygen = Polynomial(coefficients)
	keys = []

	for x in range(1, n):
		x_rand = random.randint(1, coe_max)
		y = keygen.get_y_for(x_rand)
		key = x_rand, y
		keys.append(key)
		
	#print keys
	return keys

def interpolate(points, x):
	total = 0
	count = 0
	
	for current_point in points:
		indicator = Fraction(1, 1)
		
		for other_point in points:
			if other_point[0] != current_point[0]:
				#print 'multiplying indicator by {}'.format(x - other_point[0])
				indicator *= (x - other_point[0])
				#print 'dividing indicator by {}'.format(current_point[0] - other_point[0])
				indicator /= (current_point[0] - other_point[0])

		#print 'Multiplying indicator by {}'.format(current_point[1])
		indicator *= current_point[1]
		#print 'Total is {}'.format(indicator)
		total += indicator
	
	#print total
	return total
 
	
#codes = split(200, 8, 4)
#print codes
#print interpolate(codes[2:6], 0)
#while True:
	#pass