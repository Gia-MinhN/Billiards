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

const int ball_size = 50;
const int ball_mass = 100;

// https://www.tutorialspoint.com/check-if-a-line-touches-or-intersects-a-circle-in-cplusplus
// https://www.jeffreythompson.org/collision-detection/circle-rect.php

bool within_ball(Vector2<float> position, Ball ball) {
    return distance(position, ball.position) <= ball_size;
}

vector<Ball> generate_all_balls() {
    
}

Vector2<float> window_position_transform(Vector2<float> position, Vector2<float> translate, float zoom) {
    return ((position - Vector2<float>{(float)window_width/2, (float)window_height/2})/zoom + translate);
}

int main()
{
    // Variables
    Vector2<float> drag_start_position;
    Vector2<float> mouse_position;

    Vector2<float> translate  = {0.f, 0.f};
    float          zoom       = .5f;
    bool           lmb_toggle = false;
    bool           rmb_toggle = false;

    // Settings
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    // Window
    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "billiards", sf::Style::Default, settings);

    // View
    sf::View view;
    view.setSize(window_width, window_height);
    view.setCenter(0, 0);
    view.setSize(window_width/zoom, window_height/zoom);
    window.setView(view);

    // Clock
    sf::Clock clock;

    // Font
    sf::Font font;
    font.loadFromFile("arial.ttf");

    // Testing
    Ball b1 = Ball(0.f, 0.f, ball_size, ball_mass, false, sf::Color::Black, 8, &font);
    Ball b2 = Ball(-150.f, 0.f, ball_size, ball_mass, true, sf::Color::Blue, 10, &font);

    // Loop to run the game
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
                    zoom += event.mouseWheel.delta/10.f;
                    zoom = max(.5f, zoom);
                    zoom = min(3.f, zoom);
                    printf("%f\n", zoom);
                    view.setSize(window_width/zoom, window_height/zoom);
                    break;
                }
                case sf::Event::MouseButtonPressed: {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        if(rmb_toggle) break;
                        sf::Vector2i tmp = sf::Mouse::getPosition(window);
                        drag_start_position = window_position_transform({(float)tmp.x, (float)tmp.y}, translate, zoom);
                        lmb_toggle = true;

                        printf("%f, %f\n", drag_start_position.x, drag_start_position.y);
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

        window.clear(sf::Color(100, 125, 175, 255));
        window.setView(view);

        b1.drawBall(&window);
        b2.drawBall(&window);

        window.display();
    }

    return 0;
}