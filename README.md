# Cloth-Simulation with interacting particles

This is a side project of mine. It's about cloth simulation with connected
particles (it always kind of fascinated me).

This is how it looks in its final form:

![Cloth-simulation](https://github.com/MauriceGit/Cloth_Simulation/blob/master/Screenshots/cloth_sphere_.png "Final cloth simulation")

Or in wireframe-mode so we can see the cloth better:

![Cloth-simulation wireframe](https://github.com/MauriceGit/Cloth_Simulation/blob/master/Screenshots/cloth_sphere.png "Final cloth simulation in wireframe mode")

## **Physics behind it**

The cloth consists of a number of particles which are connected with a virtual spring.

- Each particle is connected directly to the four particles right next to it (to keep the structure).
- Each particle is connected to the four diagonal positioned particles (to keep the cloth from diagonally stretching too much or folding up).
- Each particle is connected to a number of particles further away (like the 3rd or 4th or so) to get some realistic bending.

Each connection consists of a basic length and spring force. If the distance between two connected particles is equal to the
basic length there is no force applied to these two particles.

If two connected particles are further away from each other than the basic length allows, a force will be calculated which pulls both
particles to each other. Likewise there is a repelling force calculated if particles get closer to each other than permitted.

Supplementary there are a couple of external forces applied to all (or some) particles, which in this case are:

- Gravity (to all particles)
- Wind. For each triangle of particles the projecting surface facing the wind-direction is calculated. Dependend on this surface a wind-force is
    applied to the correspondent particles.

After calculating all forces and constraints there is a simple Euler Integration calculated for each particle. So after calculating all forces
the accelleration, velocity and new position is estimated and particles get positioned accordingly.

![Cloth-simulation with wind](https://github.com/MauriceGit/Cloth_Simulation/blob/master/Screenshots/cloth_hanging.png "Cloth simulation with some wind")
![Cloth-simulation with spheres](https://github.com/MauriceGit/Cloth_Simulation/blob/master/Screenshots/bullet_cloth.png "Cloth after fast collision with spheres")

## **Collision**

There are a couple of simple collisions detected and handled.

Collision with the floor is handled:

![Cloth-simulation with wind](https://github.com/MauriceGit/Cloth_Simulation/blob/master/Screenshots/cloth_final.png "Cloth collision with the floor")

Collision with spheres:

![Cloth-simulation with a sphere](https://github.com/MauriceGit/Cloth_Simulation/blob/master/Screenshots/Sphere_Cloth_2.png "Cloth collision with a sphere")

Collision with cubes (sorry, it not obvious, but there is a cube underneath there ;)):

![Cloth-simulation with wind](https://github.com/MauriceGit/Cloth_Simulation/blob/master/Screenshots/cloth_final_wireframe.png "Cloth collision with the floor")

