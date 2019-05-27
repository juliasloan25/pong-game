#include "collision.h"


CollisionInfo find_collision(List *shape1, List *shape2) {
    Vector center1 = polygon_centroid(shape1);
    Vector center2 = polygon_centroid(shape2);
    double rad1 = vec_magnitude(vec_subtract(center1,
                                            *(Vector *)list_get(shape1, 0)));
    double rad2 = vec_magnitude(vec_subtract(center2,
                                            *(Vector *)list_get(shape2, 0)));

    double center_diff = vec_magnitude(vec_subtract(center1, center2));
    List *cont_shape1 = find_contained_verts(shape1, shape2);
    List *cont_shape2 = find_contained_verts(shape2, shape1);

    if (center_diff <= rad1 + rad2 && brute_force(shape1, shape2)) {
        CollisionInfo coll = {
            .collided = true,
            .axis = find_coll_axis(shape1, shape2)
        };
        list_free(cont_shape1);
        list_free(cont_shape2);
        return coll;
    }

    else { // no collision detected
        CollisionInfo coll = {
            .collided = false,
            .axis = VEC_ZERO
        };
        list_free(cont_shape1);
        list_free(cont_shape2);
        return coll;
    }
}

bool brute_force(List *shape1, List *shape2) {
    // return true if any vertices contained by either shape
    List *verts_1 = find_contained_verts(shape1, shape2);
    List *verts_2 = find_contained_verts(shape2, shape1);
    bool is_contained = (list_size(verts_1) != 0) || (list_size(verts_2) !=0);
    list_free(verts_1);
    list_free(verts_2);
    return is_contained;
}

List *find_edges(List *shape) {
    List *edges = list_init(list_size(shape), free, (EqualFunc)vec_equal);

    for (int i = 0; i < list_size(shape) - 1; i++) {
        Vector *edge = malloc(sizeof(Vector));
        *edge = vec_subtract(*(Vector *)list_get(shape, i + 1),
                                *(Vector *)list_get(shape, i));
        list_add(edges, edge);
    }
    // edges of the shape in counterclockwise order
    return edges;
}

List *find_axes(List *shape) {
    List *edges = find_edges(shape);
    List *norm_list = list_init(list_size(edges), free, (EqualFunc)vec_equal);

    for (int i = 0; i < list_size(edges); i++) {
        Vector edge = *(Vector *)list_get(edges, i);
        Vector normal = {
            .x = -1.0 * edge.y,
            .y = edge.x
        };

        // normalize axes vectors before adding them to the list
        Vector *norm = malloc(sizeof(Vector));
        *norm = vec_multiply(1.0 / vec_magnitude(normal), normal);
        list_add(norm_list, norm);
    }
    // list of normalized axes of the shape
    list_free(edges);
    return norm_list;
}

// return vertices of shape 2 contained in shape1
List *find_contained_verts(List *shape1, List *shape2) {
    List *contained_vert = list_init(list_size(shape2), NULL, (EqualFunc)vec_equal);

    List *edges = find_edges(shape1);
    for (int j = 0; j < list_size(shape2); j++) { // loop through vertices of 2
        bool is_contained = true;
        for (int i = 0; i < list_size(edges); i++) { // loop through edges of 1
            Vector point = *(Vector *)list_get(shape2, j);
            Vector vertex = *(Vector *)list_get(shape1, i); //first vert of edge
            Vector point_to_vert = vec_subtract(point, vertex);

            // vertex is to the left of edge (NOT contained)
            if(vec_cross(*(Vector *)list_get(edges, i), point_to_vert) < 0) {
                is_contained = false;
            }
        }
        if (is_contained) {
            list_add(contained_vert, (Vector *)list_get(shape2, j));
        }
    }

    // at least one vertex of shape1 contained in shape2
    list_free(edges);
    return contained_vert;
}

Vector find_coll_axis(List *shape1, List *shape2) {
    Vector axis = VEC_ZERO;
    List *shape1_cont = find_contained_verts(shape2, shape1);
    List *shape2_cont = find_contained_verts(shape1, shape2);
    /*int size1 = (int) list_size(shape1);
    int size2 = (int) list_size(shape2);*/

    for (int i = 0; i < list_size(shape1); i++) {
        // current vertex of shape1 is contained in shape2
        if(list_contains(shape1_cont, (Vector *)list_get(shape1, i))) {
            
            Vector vec1 = vec_subtract(*(Vector *)list_get(shape1, i),
                                *(Vector *)list_get(shape1, (i - 1) % list_size(shape1)));
            Vector vec2 = vec_subtract(*(Vector *)list_get(shape1, i),
                                *(Vector *)list_get(shape1, (i + 1) % list_size(shape1)));

            // normalize vector
            Vector v = vec_add(vec_multiply(1.0 / vec_magnitude(vec1), vec1),
                                vec_multiply(1.0 / vec_magnitude(vec2), vec2));
            axis = vec_add(axis, v);
        }
    }
    for (int i = 0; i < list_size(shape2); i++) {
        // current vertex of shape2 is contained in shape1
        if(list_contains(shape2_cont, (Vector *)list_get(shape2, i))) {
            Vector vec1 = vec_subtract(*(Vector *)list_get(shape2, i),
                                *(Vector *)list_get(shape2, (i - 1) % list_size(shape2)));
            Vector vec2 = vec_subtract(*(Vector *)list_get(shape2, i),
                                *(Vector *)list_get(shape2, (i + 1) % list_size(shape2)));

            // normalize vector
            Vector v = vec_add(vec_multiply(1.0 / vec_magnitude(vec1), vec1),
                                vec_multiply(1.0 / vec_magnitude(vec2), vec2));
            axis = vec_subtract(axis, v);

        }
    }
    list_free(shape1_cont);
    list_free(shape2_cont);
    return vec_multiply(1.0 / vec_magnitude(axis), axis);
}
