#include <iostream>
#include <cmath>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <thread>
#include <SFML/Graphics.hpp>
#include "classes.hpp"

using namespace std;

// Globals

const int window_width = 1000;
const int window_height = 1000;

const int sub_updates = 4;

const int ball_size = 25;
const int ball_mass = 100;
const float friction = 500.f;
const float line_distance = 422;
const sf::Color color_order[7] = {
    sf::Color(227, 211, 36, 255), // yellow 
    sf::Color::Blue, 
    sf::Color::Red, 
    sf::Color(76, 17, 171, 255), // purple 
    sf::Color(227, 154, 36, 255), // orange
    sf::Color(48, 160, 67, 255), // green
    sf::Color(148, 30, 30, 255) // dark red
};
Vector2<float> line_points[] = {
    // Top
    {-417.f, -906.f}, {-347.f, -838.f},
    {-347.f, -838.f}, {347.f, -838.f},
    {347.f, -838.f}, {417.f, -906.f},

    // Bottom
    {417.f, 906.f}, {347.f, 838.f},
    {347.f, 838.f}, {-347.f, 838.f},
    {-347.f, 838.f}, {-417.f, 906.f},

    // Lower Left
    {-495.f, 828.f}, {-425.f, 760.f},
    {-425.f, 760.f}, {-425.f, 60.f},
    {-425.f, 60.f}, {-460.f, 47.f},

    // Upper Left
    {-460.f, -47.f}, {-425.f, -60.f},
    {-425.f, -60.f}, {-425.f, -760.f},
    {-425.f, -760.f}, {-495.f, -828.f},

    // Upper Right
    {495.f, -828.f}, {425.f, -760.f},
    {425.f, -760.f}, {425.f, -60.f}, 
    {425.f, -60.f}, {460.f, -47.f}, 

    // Lower Right
    {460.f, 47.f}, {425.f, 60.f}, 
    {425.f, 60.f}, {425.f, 760.f}, 
    {425.f, 760.f}, {495.f, 828.f}
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

bool within_ball(Vector2<float> position, Ball ball) {
    return distance(position, ball.position) <= ball_size;
}

vector<Ball*> generate_all_balls(sf::Font *font) {
    vector<Ball*> all_balls;
    // White ball
    all_balls.push_back(new Ball(0.f, 0.f, ball_size, ball_mass, friction, false, WHITE, 0, font));

    // Solid balls
    for(int i = 1; i <= 7; i++) {
        all_balls.push_back(new Ball(0.f, 0.f, ball_size, ball_mass, friction, false, color_order[i-1], i, font));
    }
    
    // Black 8 ball
    all_balls.push_back(new Ball(0.f, 0.f, ball_size, ball_mass, friction, false, sf::Color::Black, 8, font));

    // Striped balls
    for(int i = 1; i <= 7; i++) {
        all_balls.push_back(new Ball(0.f, 0.f, ball_size, ball_mass, friction, true, color_order[i-1], i+8, font));
    }

    return all_balls;
}

Vector2<float> window_position_transform(Vector2<float> position, Vector2<float> translate, float zoom) {
    return ((position - Vector2<float>{(float)window_width/2, (float)window_height/2})*zoom + translate);
}

//Creation of Ball Triangle Positioning
void triangle(float x, float y, vector<Ball*> *all_balls) {
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
            (*all_balls)[shuffledNumbers[index]]->position = {x_cur, y_cur}; //Based on spacing position ball into location
            x_cur += ball_spacing_x;
            index++;
        }

        y_cur += ball_spacing_y;
        x_cur = x - 5*ball_size;
    }
}

bool none_moving(vector<Ball*> *all_balls) {
    for(auto ball : *all_balls) {
        if(ball->is_moving()) return false;
    }
    return true;
}

void ball_line_collision(Ball *ball, Line *line) {
    ball->velocity = reflect(ball->velocity, line->normal);
}

void check_ball_line_collision(vector<Ball*> *all_balls, vector<Line*> *all_lines) {
    for(auto ball : (*all_balls)) {
        for(auto line : (*all_lines)) {
            if(line->collision(ball->position, ball->radius)) {
                ball_line_collision(ball, line);
            }
        }
    }
}

void sub_update(vector<Ball*> *all_balls, float dt) {
    for(auto ball : *all_balls) {
        ball->update(dt);
    }
}

void update(vector<Ball*> *all_balls, vector<Line*> *all_lines, float dt) {
    float sub_dt = dt / sub_updates;
    for(int i = 0; i < sub_updates; i++) {
        sub_update(all_balls, sub_dt);
        check_ball_line_collision(all_balls, all_lines);
    }
}

void ball_to_ball_collision(vector<Ball*> *all_balls){

    int ball_mass_i = ball_mass;
    int ball_mass_j = ball_mass;

    
    for(int i = 0; i <= 15; i++){
        for(int j = 0; j <= 15; j++){
            if (i != j){
                float distance_val = distance((*all_balls)[i]->position, (*all_balls)[j]->position);
                //float distance = sqrtf((difference_vector.x * difference_vector.x) + (difference_vector.y * difference_vector.y));
                float sum_radius = ball_size*2; //Since radius is equal for all balls
                if (distance_val < sum_radius){
                    
                    //Normalize
                    Vector2<float> normalize = unit((*all_balls)[j]->position - (*all_balls)[i]->position);
                    
                    //i Ball Velocity Vector
                    float dot_i = dot((*all_balls)[i]->velocity, normalize);
                    Vector2<float> velocity_xi = normalize * dot_i;
                    Vector2<float> velocity_yi = (*all_balls)[i]->velocity - velocity_xi;
                    

                    //j Ball Velocity Vector
                    float dot_j = dot((*all_balls)[j]->velocity, normalize);
                    Vector2<float> velocity_xj = normalize * dot_j;
                    Vector2<float> velocity_yj = (*all_balls)[j]->velocity - velocity_xj;

                    //Velocities of both balls
                    Vector2<float> velocity_i = (velocity_xi * ((ball_mass_i - ball_mass_j)/(ball_mass_i + ball_mass_j))) + (velocity_xj * ((2 * ball_mass_j)/(ball_mass_i + ball_mass_j))) + velocity_yi;
                    Vector2<float> velocity_j = (velocity_xi * ((2 * ball_mass_i)/(ball_mass_i + ball_mass_j))) + (velocity_xj * ((ball_mass_j - ball_mass_i)/(ball_mass_i + ball_mass_j))) + velocity_yj;
                    cout << "Velocity_x: (" << velocity_i.x << ", " << velocity_i.y << ")" << endl;
                    cout << "Velocity_y: (" << velocity_j.x << ", " << velocity_j.y << ")" << endl;

                    (*all_balls)[i]->velocity = velocity_i;
                    (*all_balls)[j]->velocity = velocity_j;

                    Vector2<float> correction_overlap = normalize * (sum_radius/2);
                    
                    (*all_balls)[i]->position = (*all_balls)[i]->position - correction_overlap;
                    (*all_balls)[j]->position = (*all_balls)[j]->position + correction_overlap;


                    
                }
            }
            
        }

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
    image.loadFromFile("pool_table_nobg.png");

    // Ball setup
    Table table = Table({0.f, 0.f}, 1.f, {423.5f, -834.5f}, {475.f, 0.f}, ball_size*2, &image);
    vector<Ball*> all_balls = generate_all_balls(&font);
    triangle(0, -422, &all_balls);
    all_balls[0]->position = {0, line_distance};
    
    // Line setup
    vector<Line*> all_lines;
    for(int i = 0; i < size(line_points); i += 2) {
        all_lines.push_back(new Line(line_points[i], line_points[i+1]));
    }

    // Testing
    // Ball test_ball = Ball(0.f, 0.f, ball_size, ball_mass, false, sf::Color(255, 255, 255, 150), 999, &font);
    // all_balls.push_back(test_ball);
    all_balls[0]->velocity = Vector2<float>{0.f, -2500.f};

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

        //Ball to Ball Collision Position Change
        ball_to_ball_collision(&all_balls);

        // Updates
        float dt = clock.restart().asSeconds();
        update(&all_balls, &all_lines, dt);

        // Reset window
        window.clear(sf::Color(50, 150, 150, 255));
        window.setView(view);
        
        // Drawing
        table.draw(&window);
        for(Ball* ball : all_balls) {
            ball->draw(&window);
        }
        // test_ball.draw(&window);
        for(Line* line : all_lines) {
            line->draw(&window);
        }

        // Display
        window.display();   
    }


    // Clean up
    for(auto ball : all_balls) {
        delete ball;
    }
    for(auto line : all_lines) {
        delete line;
    }
    return 0;
}