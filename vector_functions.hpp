#include <cmath>
#include "classes.hpp"

#pragma once

float distance(Vector2<float> a, Vector2<float> b) {
    return sqrt(pow((b.x - a.x), 2) + pow((b.y - a.y), 2));
}

float magnitude(Vector2<float> vect) {
    return sqrt(pow(vect.x, 2) + pow(vect.y, 2));
}

Vector2<float> unit(Vector2<float> vect){
    if(vect.x == 0 && vect.y == 0) {
        return {0, 0};
    }
    return vect/magnitude(vect);
}

float angle(Vector2<float> a, Vector2<float> b) {
    return atan2(a.y - b.y, a.x - b.x);
}

float dot(Vector2<float> a, Vector2<float> b) {
    return a.x*b.x + a.y*b.y;
}

Vector2<float> reflect(Vector2<float> vect, Vector2<float> norm) {
    return (vect - norm*2*dot(norm, vect));
}
