#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <variant>

#pragma once

#define PI 3.14159265359
#define WHITE sf::Color(237, 237, 223, 255)

enum collision_type {
    NORMAL,
    END,
    NO_COLLISION
};

using namespace std;

// Vector and functions

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

// Classes

class Ball {
    private:
    sf::CircleShape back;
    sf::ConvexShape stripe;
    sf::CircleShape small_circle;
    sf::Text number_label;
    sf::CircleShape outline;

    public:
    Vector2<float>    position, velocity;
    float             radius, mass;
    float             friction;
    bool              is_striped;
    sf::Color         color;
    int               number;
    bool              is_moving;

    //Constructors
    Ball(float x, float y, float r, float m, float f, bool striped, sf::Color col, int num, sf::Font *font) {
        position = Vector2<float>{x, y};
        velocity = Vector2<float>{0, 0};
        radius = r;
        mass = m;
        friction = f;
        is_striped = striped;
        color = col;
        number = num;
        is_moving = false;

        back.setRadius(radius);
        back.setOrigin(radius, radius);
        back.setPointCount(120);
        back.setFillColor(color);

        if(is_striped) {
            back.setFillColor(WHITE);

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
        small_circle.setFillColor(WHITE);

        number_label.setString(to_string(number));
        number_label.setCharacterSize(radius/2);
        number_label.setStyle(sf::Text::Bold);
        number_label.setFillColor(sf::Color::Black);
        number_label.setFont(*font);
        auto center = number_label.getGlobalBounds().getSize() / 2.f;
        auto localBounds = center + number_label.getLocalBounds().getPosition();
        number_label.setOrigin(localBounds);

        outline.setRadius(radius-1);
        outline.setOrigin(radius-1, radius-1);
        outline.setPointCount(120);
        outline.setFillColor(sf::Color(255, 255, 255, 0));
        outline.setOutlineThickness(2);
        outline.setOutlineColor(sf::Color::Black);
    }

    // Methods
    bool moving() {
        return magnitude(velocity) >= 10.f;
    }

    void update(float dt) {        
        Vector2<float> acceleration = velocity*friction*-1;
        position = position + velocity*dt + acceleration*dt*dt/2.f;
        velocity = velocity + acceleration*dt;

        if(is_moving) {
            if(!moving()) {
                velocity = Vector2<float>{0.f, 0.f};
                is_moving = false;
            }
        }
    }

    void draw(sf::RenderWindow *window) {
        back.setPosition(position.x, position.y);
        window->draw(back);

        if(number != 0) {
            if(is_striped) {
                stripe.setPosition(position.x, position.y);
                window->draw(stripe);
            }

            small_circle.setPosition(position.x, position.y);
            window->draw(small_circle);

            number_label.setPosition((int)position.x, (int)position.y);
            window->draw(number_label);

            outline.setPosition(position.x, position.y);
            window->draw(outline);
        }
    }
};

class Cue {
    public:

    Vector2<float> p1, p2;
    float width, length;

    Cue(float w, float l) {
        width = w;
        length = l;
    }

    void draw(sf::RenderWindow *window) {
        
    }
};

class Table {
    private:
    sf::Texture texture;
    sf::Sprite sprite;

    public:
    vector<Vector2<float>> hole_position;
    vector<sf::CircleShape> holes;
    float hole_radius;

    Table(Vector2<float> pos, float scale, Vector2<float> corner_hole, Vector2<float> side_hole, float hole_r, sf::Image *image) {
        texture.loadFromImage(*image);
        texture.setSmooth(true);

        sprite.setTexture(texture);
        sprite.setOrigin(sprite.getLocalBounds().width/2, sprite.getLocalBounds().height/2);
        sprite.setPosition(pos.x, pos.y);
        sprite.setScale(scale, scale);
        // sprite.setRotation(90);

        hole_radius = hole_r;

        hole_position.push_back(corner_hole);
        hole_position.push_back(corner_hole*Vector2<float>{-1.f, 1.f});
        hole_position.push_back(corner_hole*Vector2<float>{1.f, -1.f});
        hole_position.push_back(corner_hole*Vector2<float>{-1.f, -1.f});

        hole_position.push_back(side_hole);
        hole_position.push_back(side_hole*Vector2<float>{-1.f, 1.f});

        for(auto pos : hole_position) {
            sf::CircleShape hole;
            hole.setRadius(hole_radius);
            hole.setOrigin(hole_radius, hole_radius);
            hole.setPosition(pos.x, pos.y);
            hole.setFillColor(sf::Color::Black);
            holes.push_back(hole);
        }
    }

    void draw(sf::RenderWindow *window) {
        window->draw(sprite);

        for(auto hole : holes) {
            window->draw(hole);
        }
    }
};

class Line {
    private:
    sf::RectangleShape line;

    public:
    Vector2<float> p1, p2, normal;
    float length;

    Line(Vector2<float> point1, Vector2<float> point2) {
        p1 = point1;
        p2 = point2;
        normal = unit(Vector2<float>{-(p2.y - p1.y), p2.x - p1.x});
        length = distance(p1, p2);

        line.setSize(sf::Vector2f(distance(p1, p2), 8));
        line.setOrigin(0, 4);
        line.setPosition(p1.x, p1.y);
        line.setRotation(angle(p1, p2)*180.f/PI+180);
        line.setFillColor(sf::Color(255, 0, 0, 75));
    }
    
    // https://www.youtube.com/watch?v=h1WXaa2UwLQ
    bool collision(Vector2<float> pos, float radius) {
        float dot_product = dot(pos - p1, p2 - p1) / (length*length);
        Vector2<float> closest = p1 + (p2-p1)*dot_product;

        if(distance(pos, p1) <= radius || distance(pos, p2) <= radius) return true;
        if(distance(closest, p1) + distance(closest, p2) - 1 > length) return false;
        if(distance(pos, closest) <= radius) return true;
        return false;
    }

    void draw(sf::RenderWindow *window) {
        window->draw(line);
    }
};

