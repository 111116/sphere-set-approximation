import scipy
import sys
import numpy as np
from random import randrange

def select(v):
	a = randrange(0, len(v))
	b = randrange(0, len(v))
	c = randrange(0, len(v))
	d = randrange(0, len(v))
	for iter in range(100):
		a1 = randrange(0, len(v))
		b1 = randrange(0, len(v))
		c1 = randrange(0, len(v))
		d1 = randrange(0, len(v))
		det = np.linalg.det(np.array([v[a]-v[d], v[b]-v[d], v[c]-v[d]]))
		det1 = np.linalg.det(np.array([v[a1]-v[d1], v[b1]-v[d1], v[c1]-v[d1]]))
		# print("det1", det1, a1,b1,c1,d1)
		if abs(det1) > abs(det):
			a,b,c,d = a1,b1,c1,d1
	return a,b,c,d

# help
if len(sys.argv) != 5:
	print("Usage: ...py original.obj transformed.obj original.sph output.sph")
	exit(-1)

vertices = [[],[]]
for id in range(2):
	with open(sys.argv[id+1], "r") as f:
		for line in f:
			tokens = line.split()
			if len(tokens)>0 and tokens[0] == "v":
				vertices[id].append(np.array(list(map(float, tokens[1:]))))
# select representative tetrahedron
assert len(vertices[0]) == len(vertices[1])
id1, id2, id3, id4 = select(vertices[0])
A = np.vstack((
	np.concatenate((vertices[0][id1], [1])),
	np.concatenate((vertices[0][id2], [1])),
	np.concatenate((vertices[0][id3], [1])),
	np.concatenate((vertices[0][id4], [1]))
	)).T
B = np.vstack((
	np.concatenate((vertices[1][id1], [1])),
	np.concatenate((vertices[1][id2], [1])),
	np.concatenate((vertices[1][id3], [1])),
	np.concatenate((vertices[1][id4], [1]))
	)).T
# solve transform
transform = np.matmul(A, np.linalg.inv(B))
print("DET:", np.linalg.det(transform))
if abs(np.linalg.det(transform) - 1) > 0.01:
	print("scaling not supported")

def transformed(x):
	x = np.concatenate((x, [1]))
	return np.dot(transform,x)[0:3]
# check error
maxerr = 0.
for i in range(len(vertices[0])):
	maxerr = max(maxerr, abs(np.amax(transformed(vertices[0][i]) - vertices[1][i])))
print("maxerr:", maxerr)
if maxerr > 0.01:
	print("halting...")
	exit(-1)
# transform spheres
with open(sys.argv[3], "r") as f:
	with open(sys.argv[4], "w") as fw:
		print("converting...")
		for line in f:
			x,y,z,r = map(float, line.split())
			x1,y1,z1 = np.around(transformed(np.array([x,y,z])), decimals=8)
			print(x1,y1,z1,r, file=fw)
		print("done.")

