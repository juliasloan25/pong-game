#ifndef __COLLISION_H__
#define __COLLISION_H__

#include <stdbool.h>
#include <math.h>
#include "list.h"
#include "vector.h"
#include "body.h"
#include "polygon.h"

/**
 * Represents the status of a collision between two shapes.
 * The shapes are either not colliding, or they are colliding along some axis.
 */
typedef struct {
    /** Whether the two shapes are colliding */
    bool collided;
    /**
     * If the shapes are colliding, the axis they are colliding on.
     * This is a unit vector pointing from the first shape towards the second.
     * Normal impulses are applied along this axis.
     * If collided is false, this value is undefined.
     */
    Vector axis;
} CollisionInfo;


/**
 * Computes the status of the collision between two convex polygons.
 * The shapes are given as lists of vertices in counterclockwise order.
 * There is an edge between each pair of consecutive vertices,
 * and one between the first vertex and the last vertex.
 *
 * @param shape1 the first shape
 * @param shape2 the second shape
 * @return whether the shapes are colliding, and if so, the collision axis.
 * The axis should be a unit vector pointing from shape1 towards shape2.
 */
CollisionInfo find_collision(List *shape1, List *shape2);

/*
* Goes through the brute force test in the case where bounding boxes do overlap
* to see if any vertices of shape2 are contained within shape1.
*
* @param shape1 the first shape
* @param shape2 the second shape
* @return whether the shapes are colliding
*/
bool brute_force(List *shape1, List *shape2);

/*
* Determine all edges of a shape
*
* @param shape the shape to find edges of
* @return a list of vectors representing the edges of a shape
*/
List *find_edges(List *shape);


/*
* Determine all normalized axes of a shape
*
* @param shape the shape to find axes of
* @return a list of vectors representing all unit normals to the edges (i.e. axes)
*/
List *find_axes(List *shape);

/*
* Find all vertices of shape2 that are contained in shape1
* @param shape1 the shape to check if vertices are contained in
* @param shape2 the shape to check if any of its vertices are contained
* @return a list of the vertices of shape 2 that are contained in shape1
*/
List *find_contained_verts(List *shape1, List *shape2);

/*
* Find the collision axis of shape1 and shape2. Returns VEC_ZERO if no collision
* @param shape1 the first shape
* @param shape2 the second shape
* @return a normalized vector representing the collision axis
*/
Vector find_coll_axis(List *shape1, List *shape2);


#endif // #ifndef __COLLISION_H__
