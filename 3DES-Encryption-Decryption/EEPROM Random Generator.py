import random
import string

def RNG(size):
    values = string.ascii_lowercase + string.ascii_uppercase + string.digits

    x = ''
    for y in range(size):
        x = x + (random.choice(values))

    return x

t = RNG(4096)
print(t)
print(":".join("{:02x}".format(ord(c)) for c in t))