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

const int window_width = 1000;
const int window_height = 1000;

const int ball_size = 50;
const int ball_mass = 100;
const sf::Color color_order[7] = {
    sf::Color(227, 211, 36, 255), // yellow 
    sf::Color::Blue, 
    sf::Color::Red, 
    sf::Color(76, 17, 171, 255), // purple 
    sf::Color(227, 154, 36, 255), // orange
    sf::Color(48, 160, 67, 255), // green
    sf::Color(148, 30, 30, 255) // dark red
};

//Randomize values from 0-14
vector<int> generateShuffledNumbers() {
    vector<int> numbers(15);
    for (int i = 0; i < 15; ++i) {
        numbers[i] = i + 1;
    }
    random_shuffle(numbers.begin(), numbers.end()); // Shuffles the numbers
    return numbers;
}

// https://www.tutorialspoint.com/check-if-a-line-touches-or-intersects-a-circle-in-cplusplus
// https://www.jeffreythompson.org/collision-detection/circle-rect.php

bool within_ball(Vector2<float> position, Ball ball) {
    return distance(position, ball.position) <= ball_size;
}

vector<Ball> generate_all_balls(sf::Font *font) {
    vector<Ball> all_balls;
    // White ball
    all_balls.push_back(Ball(0.f, 0.f, ball_size, ball_mass, false, WHITE, 0, font));

    // Solid balls
    for(int i = 1; i <= 7; i++) {
        all_balls.push_back(Ball(0, 0, ball_size, ball_mass, false, color_order[i-1], i, font));
    }
    
    // Black 8 ball
    all_balls.push_back(Ball(0.f, (float)ball_size*2, ball_size, ball_mass, false, sf::Color::Black, 8, font));

    // Striped balls
    for(int i = 1; i <= 7; i++) {
        all_balls.push_back(Ball(0, 0, ball_size, ball_mass, true, color_order[i-1], i+8, font));
    }

    return all_balls;
}

Vector2<float> window_position_transform(Vector2<float> position, Vector2<float> translate, float zoom) {
    return ((position - Vector2<float>{(float)window_width/2, (float)window_height/2})*zoom + translate);
}

//Creation of Ball Triangle Positioning
void triangle(float x, float y, vector<Ball>* all_balls) {
    int index = 0;
    float ball_spacing_x = ball_size;
    float ball_spacing_y = -sinf(PI*2/3)*ball_size*2;

    srand(static_cast<unsigned>(time(nullptr))); //Randomize based on time
    vector<int> shuffledNumbers = generateShuffledNumbers();

    // Swap 8-ball to center
    int eight_ball_index;
    for(eight_ball_index = 0; eight_ball_index < shuffledNumbers.size(); eight_ball_index++) {
        if(shuffledNumbers[eight_ball_index] == 8) 
            break;
    }
    int tmp = shuffledNumbers[4];
    shuffledNumbers[4] = shuffledNumbers[eight_ball_index];
    shuffledNumbers[eight_ball_index] = tmp;

    float x_cur = x - 5*ball_size;
    float y_cur = y;

    for(int i = 1; i <= 5; i++) {
        for(int k = 5 - i; k > 0; k--) {
            x_cur += ball_spacing_x; //Create spacing
        }
        for(int j = 1; j <= i; j++) {
            x_cur += ball_spacing_x;
            (*all_balls)[shuffledNumbers[index]].position = {x_cur, y_cur}; //Based on spacing position ball into location
            x_cur += ball_spacing_x;
            index++;
        }

        y_cur += ball_spacing_y;
        x_cur = x - 5*ball_size;
    }
}

int main()
{
    Vector2<float> drag_start_position;
    Vector2<float> mouse_position;

    Vector2<float> translate  = {0.f, 0.f};
    float          zoom       = 4.f;
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
    view.setSize(window_width*zoom, window_height*zoom);
    window.setView(view);

    // Clock
    sf::Clock clock;

    // Font
    sf::Font font;
    font.loadFromFile("arial.ttf");

    // Image
    sf::Image image;
    image.loadFromFile("pool_table.png");
    

    // Testing
    Table table = Table(0.f, 0.f, 2.f, 0.f, 0.f, 0.f, &image);
    vector<Ball> all_balls = generate_all_balls(&font);
    triangle(0, -300, &all_balls);
    

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
                    zoom -= event.mouseWheel.delta/10.f;
                    zoom = max(.5f, zoom);
                    zoom = min(5.f, zoom);
                    view.setSize(window_width*zoom, window_height*zoom);
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
                            zoom = 4.f;
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
        // Reset wimndow
        window.clear(sf::Color(50, 100, 100, 255));
        window.setView(view);

        // Drawing
        table.draw(&window);

       


        for(Ball ball : all_balls) {
            ball.draw(&window);
        }

        // Display
        window.display();   
    }

    return 0;
}