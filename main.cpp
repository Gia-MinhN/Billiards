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




const int ball_size = 25;
const int ball_mass = 100;
const sf::Color color_order[7] = {
    sf::Color(227, 211, 36, 255), // yellow 
    sf::Color::Blue, 
    sf::Color::Red, 
    sf::Color(76, 17, 171, 255), // purple 
    sf::Color(227, 154, 36, 255), // orange
    sf::Color(44, 148, 67, 255), // green
    sf::Color(148, 30, 30, 255) // dark red
};

const float ball_spacing_x = ball_size + 1; //Add for spacing
const float ball_spacing_y = ball_size*2 - 5; //Subtract for tight fit
float ball_placement_x = 0.f;
float ball_placement_y = 0.f;

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

    // All solid/striped balls
    for(int i = 1; i <= 7; i++) {
        all_balls.push_back(Ball((float)i*ball_size*2, 0.f, ball_size, ball_mass, false, color_order[i-1], i, font));
        all_balls.push_back(Ball((float)i*ball_size*2, (float)ball_size*2, ball_size, ball_mass, true, color_order[i-1], i+8, font));
    }
    
    // Black 8 ball
    all_balls.push_back(Ball(0.f, (float)ball_size*2, ball_size, ball_mass, false, sf::Color::Black, 8, font));
    return all_balls;

}

Vector2<float> window_position_transform(Vector2<float> position, Vector2<float> translate, float zoom) {
    return ((position - Vector2<float>{(float)window_width/2, (float)window_height/2})/zoom + translate);
}

//Creation of Ball Triangle Positioning
int triangle(float ball_placement_x, float ball_placement_y, float ball_spacing_x, float ball_spacing_y, vector<Ball>* all_balls) {
    int l = 0;
    srand(static_cast<unsigned>(time(nullptr))); //Randomize based on time
    vector<int> shuffledNumbers = generateShuffledNumbers();
    for(int i=1; i<=5; i++) {
        for(int k=5-i; k>0; k--) 
            ball_placement_x = ball_placement_x + ball_spacing_x; //Create spacing
        for(int j=1; j<=i; j++) {
            ball_placement_x = ball_placement_x + ball_spacing_x;
            int nums = shuffledNumbers[l] - 1; //Radomization  Shuffle
            (*all_balls)[nums+1].position = {ball_placement_x, ball_placement_y}; //Based on spacing position ball into location, use +1 to disregard white ball
            ball_placement_x = ball_placement_x + ball_spacing_x;
            l++;
            //cout << l;
            
        }

        ball_placement_y = ball_placement_y + ball_spacing_y; //Tight fit
        ball_placement_x = 0;
       
    }
    return 0;
}

int main()
{
    

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

    // Image
    sf::Image image;
    image.loadFromFile("pool_table.jpg");
    

    // Testing
    Table table = Table(0.f, 0.f, 2.f, &image);
    vector<Ball> all_balls = generate_all_balls(&font);

    triangle(ball_placement_x,ball_placement_y,ball_spacing_x,ball_spacing_y, &all_balls);
    

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
                            zoom = .5f;
                            translate = {0, 0};
                            view.setSize(window_width/zoom, window_height/zoom);
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
        window.clear();
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