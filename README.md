# Sphere Set Approximation

**WORK IN PROGRESS**

Approximate a mesh by bounding it with a set of spheres, using method described in

[*Variational Sphere set Approximation for Solid Objects*](http://dx.doi.org/10.1007/s00371-006-0052-0)

with several minor changes:

- bugfixes SOTV, which in this paper is overestimated in some cases
- provides an analytic algorithm to solve swing volume
- voxel size doesn't have to be manually specified
- surface points are generated using best-candidate instead of random sampling 
- point assignment

### Usage

```bash
cd src
make
./main ../armadillo.obj 64
```

The algorithm runs very slow (proportional to number of faces).

### Abstract of the Paper

We approximate a solid object represented as a triangle mesh by a bounding set of spheres having minimal summed volume outside the object. We show how outside volume for a single sphere can be computed using a simple integration over the object’s triangles. We then minimize the total outside volume over all spheresin the set using a variant of iterative Lloyd clustering which splits the mesh points into sets and bounds each with an outside volume-minimizing sphere. The resulting sphere sets are tighter than those of previous methods. In experiments comparing against a state-of-the-art alternative (adaptive medial axis), our method often requires half or fewer as many spheres to obtain the same error, under a variety of error metrics including total outside volume, shadowing fidelity, and proximity measurement.

### Optimization steps

1. Fix the centers of spheres. Greedily assign points to them, minimizing OV (outside volume)
2. Fix the point clusters. Adjust the spheres using Powell's method, minimizing OV
3. Teleportation: Remove the most overlapped sphere. Split the sphere with most OV

Alternate between 1 and 2, triggering 3 whenever failing to reduce loss.

