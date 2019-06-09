#include "body.h"

struct body {
	List *polygon;
	Vector vel;
	RGBColor body_color;
	double mass;
	Vector centroid;
	double angle;
	Vector force;
	Vector impulse;
	void *info;
	FreeFunc info_freer;
	bool removed;
	double collisions;
	bool has_coll_limit;
	double coll_limit;
};

Body *body_init(List *shape, double mass, RGBColor color){
	Body *body = malloc(sizeof(Body));
	assert(body != NULL);
	body->polygon = shape;
	body->vel = VEC_ZERO;
	body->mass = mass;
	body->body_color = color;
	body->angle = 0;
	body->centroid = polygon_centroid(shape);
	body->force = VEC_ZERO;
	body->impulse = VEC_ZERO;
	body->removed = false;
	body->collisions = 0;
	body->has_coll_limit = false;
	return body;
}

Body *body_init_with_info(List *shape, double mass, RGBColor color,
			void *info, FreeFunc info_freer){
	Body *body = body_init(shape, mass, color);
	body->info = info;
	body->info_freer = info_freer;
	return body;
}

Body *body_init_with_collision_limit(List *shape, double mass, RGBColor color,
			void *info, FreeFunc info_freer, double coll_limit){
	Body *body = body_init_with_info(shape, mass, color, info, info_freer);
	body->has_coll_limit = true;
	body->coll_limit = coll_limit;
	return body;
}

void body_free(Body *body){
	list_free(body->polygon);
	if(body->info_freer != NULL || body->info != NULL){
		body->info_freer(body->info);
	}
	free(body);
}

List *body_get_shape(Body *body){
	List *current_shape = list_init(list_size(body->polygon), free,
										(EqualFunc) vec_equal_v);
	Vector* curr_vec;
	Vector* curr_vec_b;
	for(int i = 0; i < list_size(body->polygon); i++){
		curr_vec = malloc(sizeof(Vector));
		curr_vec_b = (Vector *)list_get(body->polygon, i);
		curr_vec->x = curr_vec_b->x;
		curr_vec->y = curr_vec_b->y;
		list_add(current_shape, curr_vec);
	}
	return current_shape;
}

Vector body_get_centroid(Body *body){
	return body->centroid;
}

Vector body_get_velocity(Body *body){
	return body->vel;
}

RGBColor body_get_color(Body *body){
	return body->body_color;
}

void body_set_centroid(Body *body, Vector x){
	Vector curr_centroid = body_get_centroid(body);
	polygon_translate(body->polygon, vec_subtract(x, curr_centroid));
	body->centroid = polygon_centroid(body->polygon);
}

double body_get_mass(Body *body){
	return body->mass;
}

Vector body_get_force(Body *body) {
	return body->force;
}

void body_set_velocity(Body *body, Vector v){
	body->vel = v;
}

void body_set_rotation(Body *body, double angle){
	polygon_rotate(body->polygon, angle - body->angle, body->centroid);
	body->angle = angle;
}

void body_add_force(Body *body, Vector force){
	body->force = vec_add(force, body->force);
}

void body_add_impulse(Body *body, Vector impulse){
	body->impulse = vec_add(impulse, body->impulse);
}

void body_tick(Body *body, double dt){
	Vector accel = vec_multiply(1 / body_get_mass(body), body_get_force(body));
	body_set_velocity(body, vec_add(vec_multiply(dt, accel), body_get_velocity(body)));
	body_set_velocity(body, vec_add(vec_multiply(1 / body_get_mass(body),
									body->impulse), body_get_velocity(body)));

	Vector translate_vector = vec_multiply(dt, body_get_velocity(body));
	body->impulse = VEC_ZERO;
	body->force = VEC_ZERO;
	Vector new_centroid = vec_add(body_get_centroid(body), translate_vector);
	body_set_centroid(body, new_centroid);
}

void *body_get_info(Body *body){
	return body->info;
}

void body_remove(Body *body){
	body->removed = true;
}

bool body_is_removed(Body *body){
	return body->removed;
}

void body_add_collision(Body *body){
	body->collisions++;
	if(body->has_coll_limit){
		if(body->collisions >= body->coll_limit){
			body->removed = true;
		}
	}
}

void body_set_color(Body *body, RGBColor color){
	body->body_color = color;
}
