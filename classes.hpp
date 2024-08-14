#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <variant>

#pragma once

#define PI 3.14159265359

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
    Vector2<float>    position, velocity;
    float             radius, mass;
    bool              is_striped;
    sf::Color         color;
    int               number;

    sf::CircleShape back;
    sf::ConvexShape stripe;
    sf::CircleShape small_circle;
    sf::Text number_label;


    //Constructors
    Ball(float x, float y, float r, float m, bool striped, sf::Color col, int num, sf::Font *font) {
        position = Vector2<float>{x, y};
        velocity = Vector2<float>{0, 0};
        radius = r;
        mass = m;
        is_striped = striped;
        color = col;
        number = num;

        back.setRadius(radius);
        back.setOrigin(radius, radius);
        back.setPointCount(120);
        back.setFillColor(color);

        if(is_striped) {
            back.setFillColor(sf::Color::White);

            int stripe_resolution = 30;
            float stripe_arc = 90.f;
            float stripe_angle = stripe_arc/stripe_resolution;
            float angle_adjust = (stripe_resolution % 2 == 0) ? stripe_angle/2 : 0;
            stripe.setPointCount(stripe_resolution*2);
            for(int i = 0; i < stripe_resolution; i++) {
                float angle = (i-stripe_resolution/2)*stripe_angle + angle_adjust;
                float radians = angle * PI / 180.f;
                stripe.setPoint(i, sf::Vector2f(cosf(radians)*radius, -sinf(radians)*radius));
                radians += PI;
                stripe.setPoint(i+stripe_resolution, sf::Vector2f(cosf(radians)*radius, -sinf(radians)*radius));
            }
            stripe.setFillColor(color);
        }

        small_circle.setRadius(radius/2.5);
        small_circle.setOrigin(radius/2.5, radius/2.5);
        small_circle.setPointCount(60);
        small_circle.setFillColor(sf::Color::White);

        number_label.setString(to_string(number));
        number_label.setCharacterSize(radius/2);
        number_label.setStyle(sf::Text::Bold);
        number_label.setFillColor(sf::Color::Black);
        number_label.setFont(*font);
        auto center = number_label.getGlobalBounds().getSize() / 2.f;
        auto localBounds = center + number_label.getLocalBounds().getPosition();
        number_label.setOrigin(localBounds);
    }

    Ball(int x, int y, float r, float m, bool striped, sf::Color col, int num, sf::Font *font) {
        Ball((float)x, (float)y, r, m, striped, col, num, font);
    }

    Ball(Vector2<float> pos, float r, float m, bool striped, sf::Color col, int num, sf::Font *font) {
        Ball(pos.x, pos.y, r, m, striped, col, num, font);
    }

    // Methods
    void drawBall(sf::RenderWindow *window) {
        back.setPosition(position.x, position.y);
        window->draw(back);

        if(is_striped) {
            stripe.setPosition(position.x, position.y);
            window->draw(stripe);
        }

        small_circle.setPosition(position.x, position.y);
        window->draw(small_circle);

        number_label.setPosition((int)position.x, (int)position.y);
        window->draw(number_label);
    }

    void update(float dt) {

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

