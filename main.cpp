#include <iostream>
#include <cmath>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <thread>
#include <SFML/Graphics.hpp>

#include "vector_functions.hpp"
#include "classes.hpp"

using namespace std;

// Globals

const int window_width = sf::VideoMode::getDesktopMode().width/2;
const int window_height = sf::VideoMode::getDesktopMode().height/2;

const int ball_size = 20;
const int ball_mass = 100;
const float drag = 0.9f;

// https://www.tutorialspoint.com/check-if-a-line-touches-or-intersects-a-circle-in-cplusplus
// https://www.jeffreythompson.org/collision-detection/circle-rect.php

bool within_ball(Vector2<float> position, Ball ball) {
    return distance(position, ball.position) <= ball_size;
}

Vector2<float> window_position_transform(Vector2<float> position, Vector2<float> translate, float zoom) {
    return ((position - Vector2<float>{(float)window_width/2, (float)window_height/2})*zoom + translate);
}

int main()
{
    // Window
    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "billiards");

    // View
    sf::View view;
    view.setSize(window_width, window_height);
    view.setCenter(0, 0);
    window.setView(view);

    // Clock
    sf::Clock clock;

    // Variables
    Vector2<float> drag_start_position;
    Vector2<float> mouse_position;
    Vector2<float> translate = {0.f, 0.f};
    float zoom = 1.f;
    bool  lmb_toggle       = false;
    bool  rmb_toggle       = false;

    sf::CircleShape ball(ball_size);
    ball.setOrigin(ball.getRadius(), ball.getRadius());
    ball.setPosition(0, 0);
    ball.setFillColor(sf::Color::Red);

    sf::CircleShape ball2(ball_size);
    ball2.setOrigin(ball.getRadius(), ball.getRadius());
    ball2.setPosition(-100, 0);

    translate = {-100, -100};
    view.setCenter(translate.x, translate.y);

    while (window.isOpen())
    {
        sf::Event event;

        // Checking events
        while (window.pollEvent(event))
        {
            switch(event.type) {
                case sf::Event::Closed: {
                    window.close();
                    break;
                }
                case sf::Event::MouseWheelMoved: {
                    zoom -= event.mouseWheel.delta/10.f;
                    zoom = max(.5f, zoom);
                    zoom = min(3.f, zoom);
                    view.setSize(window_width*zoom, window_height*zoom);
                    break;
                }
                case sf::Event::MouseButtonPressed: {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        if(rmb_toggle) break;
                        sf::Vector2i tmp = sf::Mouse::getPosition(window);
                        drag_start_position = window_position_transform({(float)tmp.x, (float)tmp.y}, translate, zoom);
                        lmb_toggle = true;
                    }
                    if (event.mouseButton.button == sf::Mouse::Right) {
                        if(lmb_toggle) break;
                        sf::Vector2i tmp = sf::Mouse::getPosition(window);
                        drag_start_position = window_position_transform({(float)tmp.x, (float)tmp.y}, translate, zoom);
                        rmb_toggle = true;
                    }
                    break;
                }
                case sf::Event::MouseButtonReleased: {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        lmb_toggle = false;
                    }
                    if (event.mouseButton.button == sf::Mouse::Right) {
                        rmb_toggle = false;
                    }
                    break;
                }
                case sf::Event::KeyPressed: {
                    switch(event.key.code) {
                        case sf::Keyboard::Escape: {
                            window.close();
                            break;
                        }
                        case sf::Keyboard::R: {
                            zoom = 1.f;
                            translate = {0, 0};
                            view.setSize(window_width*zoom, window_height*zoom);
                            view.setCenter(0, 0);
                            break;
                        }
                        default: {
                            break;
                        }
                    }
                    break;
                }
                default: {
                    break;
                }
            }
        }

        if(rmb_toggle) {
            sf::Vector2i tmp = sf::Mouse::getPosition(window);
            mouse_position = window_position_transform({(float)tmp.x, (float)tmp.y}, {0, 0}, zoom);
            translate = drag_start_position - mouse_position;
            view.setCenter(translate.x, translate.y);
        }

        window.clear();
        window.setView(view);

        window.draw(ball);

        window.draw(ball2);

        window.display();
    }

    return 0;
}