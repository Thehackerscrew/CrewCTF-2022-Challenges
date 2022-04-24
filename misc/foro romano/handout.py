randseed = '66a48631d401c5e6b5e18'
randpos = 7

dictionarium = ['gravida', 'tristique', 'nunc', 'ornare', 'luctus', 'velit', 'ullamcorper', 'quam', 'mi', 'aliquam', 'ac', 'eleifend', 'porttitor', 'cursus', 'nisl', 'vivamus', 'faucibus', 'nibh', 'blandit', 'venenatis', 'tortor', 'egestas', 'enim', 'orci', 'sit', 'dignissim', 'ipsum', 'urna', 'id', 'semper', 'quisque', 'maecenas', 'in', 'morbi', 'suspendisse', 'posuere', 'nam', 'nec', 'eget', 'sagittis', 'est', 'auctor', 'dictum', 'nullam', 'amet', 'arcu', 'consequat', 'pulvinar', 'ligula', 'lacus', 'justo', 'elementum', 'pharetra', 'viverra', 'neque', 'sed']
hexed_dict = ["".join("{:02x}".format(ord(c)) for c in word) for word in dictionarium]
hexed_dict[randpos] = randseed
int_dict = [int(c, 16) for c in hexed_dict]

passwd = input("Welcome to Foro Romano. You must enter the password to enter: ")
bined_passwd = "".join("{0:b}".format(ord(c)).zfill(7) for c in passwd)

key_str = 's3cr3t_k3y'
hexed_key = "".join("{:02x}".format(ord(c)) for c in key_str)
int_key = int(hexed_key,16)

xor = 0

assert(len(bined_passwd) == len(int_dict))

for i in range(len(bined_passwd)):
	if bined_passwd[i] == '1':
		xor = xor ^ int_dict[i]

if xor == int_key:
	FLAG = 'crew{{{}}}'.format(passwd)
	print("Flag:", FLAG)
else:
	print("Wrong password!")
