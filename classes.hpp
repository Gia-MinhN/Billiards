#include <SFML/Graphics.hpp>
#include <vector>

#pragma once

using namespace std;

template<class T>
struct Vector2 {
    T x, y;

    // Vector-Scalar
    inline Vector2 operator+(T val) {
        return {x + val, y + val};
    }

    inline Vector2 operator-(T val) {
        return {x - val, y - val};
    }

    inline Vector2 operator*(T val) {
        return {x*val, y*val};
    }

    inline Vector2 operator/(T val) {
        return {x/val, y/val};
    }

    // Vector-Vector
    inline Vector2 operator+(Vector2 other) {
        return {x + other.x, y + other.y};
    }

    inline Vector2 operator-(Vector2 other) {
        return {x - other.x, y - other.y};
    }

    inline Vector2 operator*(Vector2 other) {
        return {x*other.x, y*other.y};
    }

    inline Vector2 operator/(Vector2 other) {
        return {x/other.x, y/other.y};
    }

    // Equals

    inline bool operator==(Vector2 other) {
        return x == other.x && y == other.y;
    }
};

class Ball {
    public:

    // Variables
    Vector2<float> position, velocity;
    bool is_striped;
    sf::Color color;
    int number;
    vector<sf::Shape> design;

    //Constructors
    Ball(float x, float y, bool stripe, sf::Color col, int num) {
        position = Vector2<float>{x, y};
        velocity = Vector2<float>{0, 0};
        is_striped = stripe;
        color = color;
        number = num;
    }

    Ball(int x, int y, bool stripe, sf::Color col, int num) {
        Ball((float)x, (float)y, stripe, col, num);
    }

    Ball(Vector2<float> pos, bool stripe, sf::Color col, int num) {
        Ball(pos.x, pos.y, stripe, col, num);
    }

    // Methods
    void draw(sf::RenderWindow *window) {
        for(auto shape : design) {
            shape.setPosition(position.x, position.y);
            (*window).draw(shape);
        }
    }
};

class Line {
    public:
    Vector2<float> p1, p2, normal;

    Line(Vector2<float> point1, Vector2<float> point2) {
        p1 = point1;
        p2 = point2;
        setupNormal();
    }

    Vector2<float> setupNormal() {
        normal = Vector2<float>{-(p2.y - p1.y), p2.x - p1.x};
        return normal;
    }
};

