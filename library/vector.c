#include "vector.h"

const Vector VEC_ZERO = {.x = 0, .y = 0};

Vector vec_add(Vector v1, Vector v2){
  Vector v3 = {
    .x = v1.x + v2.x,
    .y = v1.y + v2.y
  };
  return v3;
}

Vector vec_subtract(Vector v1, Vector v2){
  Vector v3 = {
    .x = v1.x - v2.x,
    .y = v1.y - v2.y
  };
  return v3;
}

Vector vec_negate(Vector v){
  Vector v1 = {
    .x = -1 * v.x,
    .y = -1 * v.y
  };
  return v1;
}

Vector vec_multiply(double scalar, Vector v){
  Vector v3 = {
    .x = scalar * v.x,
    .y = scalar * v.y
  };
  return v3;
}

double vec_dot(Vector v1, Vector v2){
  double dot_pro = (v1.x * v2.x) + (v1.y * v2.y);
  return dot_pro;
}

double vec_cross(Vector v1, Vector v2){
  double cross_pro = (v1.x * v2.y) - (v1.y * v2.x);
  return cross_pro;
}

Vector vec_rotate(Vector v, double angle){
  Vector v3 = {
    .x = cos(angle) * v.x - sin(angle) * v.y,
    .y = sin(angle) * v.x + cos(angle) * v.y
  };
  return v3;
}

double vec_dist(Vector v1, Vector v2){
    return vec_magnitude(vec_subtract(v1, v2));
}

double vec_magnitude(Vector v){
  double mag = sqrt(vec_dot(v, v));
  return mag;
}

bool vec_equal(Vector *v1, Vector *v2){
    return (v1->x == v2->x && v1->y == v2->y);
}
