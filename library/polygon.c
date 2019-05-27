#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "polygon.h"

double polygon_area(List *polygon){
  double area = 0;
  Vector curr_vec;
  Vector next_vec;

  /*Using summation formula to calculate area*/
  for(size_t i = 0; i<list_size(polygon); i++){
    curr_vec = *(Vector *)list_get(polygon, i);
    next_vec = *(Vector *)list_get(polygon, (i + 1) % list_size(polygon));
    area += (curr_vec.x * next_vec.y) - (next_vec.x * curr_vec.y);
  }
  if(area < 0){
    area *= -1;
  }
  return area/2;
}

Vector polygon_centroid(List *polygon){
  double c_x = 0;
  double c_y = 0;
  Vector curr_vec;
  Vector next_vec;
  double area = polygon_area(polygon);

  /*Using summation formula to calculate point of centroid*/
  for(size_t i = 0; i<list_size(polygon); i++){
    curr_vec = *(Vector *)list_get(polygon, i);
    next_vec = *(Vector *)list_get(polygon, (i + 1) % list_size(polygon));
    c_x += (curr_vec.x + next_vec.x)*(curr_vec.x*next_vec.y - next_vec.x * curr_vec.y);
    c_y += (curr_vec.y + next_vec.y)*(curr_vec.x*next_vec.y - next_vec.x * curr_vec.y);
  }

  Vector centroid = (Vector){c_x / (6 * area), c_y / (6 * area)};

  return centroid;
}

void polygon_translate(List *polygon, Vector translation){
  size_t i;
  for (i=0; i<list_size(polygon); i++){
    ((Vector *)list_get(polygon,i))->x += translation.x;
    ((Vector *)list_get(polygon,i))->y += translation.y;
  }
}

void polygon_rotate(List *polygon, double angle, Vector point){
  size_t size = list_size(polygon);
  Vector sub_vec;
  Vector vec_rot;
  Vector vec_final;
  for(size_t i = 0; i < size; i++){
    sub_vec = vec_subtract(*(Vector *)list_get(polygon, i), point);
    vec_rot = vec_rotate(sub_vec, angle);
    vec_final = vec_add(point, vec_rot);
    ((Vector *)list_get(polygon, i))->x = vec_final.x;
    ((Vector *)list_get(polygon, i))->y = vec_final.y;
  }
}
