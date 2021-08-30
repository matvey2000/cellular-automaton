#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>
#include <experimental/filesystem>
#include "windows.h"

int w = 1000;
int h = 1000;
//экран
int x = 100;
int y = 100;
//сетка
int tk = 10;//задержка 

float VCam = 1.;//скорость движения камеры

sf::RenderWindow window(sf::VideoMode(w, h), "cellular automaton", sf::Style::None);

void draw(float** grid)
{
    window.clear();
    sf::RectangleShape cell;
    cell.setSize(sf::Vector2f(w / x, h / y));
    int r;

    for (int i = 0; i < x; i++)
    {
        for (int j = 0; j < y; j++)
        {
            cell.setFillColor(sf::Color::Color(255 * grid[i][j], 255 * grid[i][j], 255 * grid[i][j]));
            cell.setPosition(sf::Vector2f(i * w / x, j * h / y));

            window.draw(cell);
        }
    }

    window.display();
}
float sr(float** grid, int x0, int y0)
{
    float sr = 0.;

    sr += grid[(x0 - 1 + x) % x][y0];
    sr += grid[(x0 + 1 + x) % x][y0];
    sr += grid[x0][(y0 - 1 + y) % y];
    sr += grid[x0][(y0 + 1 + y) % y];

    sr = sr / 4;
    return sr;
}
float** itt(float** grid)
{
    Sleep(tk);
    auto copy = grid;
    float sr_zn;
    for (int x0 = 0; x0 < x; x0++)
    {
        for (int y0 = 0; y0 < y; y0++)
        {
            sr_zn = 0.5-abs(sr(grid, x0, y0) - grid[x0][y0]);
            copy[x0][y0] += grid[x0][y0] - sr_zn;
            if (copy[x0][y0] < 0)
            {
                copy[x0][y0] = 0;
            }
            if (copy[x0][y0] > 1)
            {
                copy[x0][y0] = 1;
            }
        }
    }
    return copy;
}
int main()
{
    /*
    * esc - закончить сейчас же
    * p - фото
    * колёсико мыши - масштаб
    * wasd - движение
    */

    std::experimental::filesystem::remove_all("photos");
    std::experimental::filesystem::create_directories("photos");

    window.clear();

    sf::Texture photo;
    photo.create(w, h);

    int kadr = 0;

    float** grid = new float* [x];
    for (int i = 0; i < x; i++)
    {
        grid[i] = new float[y];
        for (int j = 0; j < x; j++)
        {
            grid[i][j] = rand() / float(RAND_MAX);
        }
    }
    sf::View view;

    int w = 0;
    int d = 0;
    float vx = 0;
    float vy = 0;

    double dt;
    double t = clock();
    while (window.isOpen())
    {
        dt = clock() - t;
        t = clock();
        view.move(vx * dt, vy * dt);

        sf::Event evnt;

        while (window.pollEvent(evnt))
        {
            if (evnt.type == sf::Event::KeyPressed)
            {
                if (evnt.key.code == sf::Keyboard::Escape)
                {
                   
                    window.close();
                }
                else if (evnt.key.code == sf::Keyboard::P)
                {
                    //фото
                    photo.update(window);
                    photo.copyToImage().saveToFile("photos/photo_"+std::to_string(kadr)+".png");
                    kadr++;
                }
                else if (evnt.key.code == sf::Keyboard::W)
                {
                    w = 1;
                }
                else if (evnt.key.code == sf::Keyboard::S)
                {
                    w = -1;
                }
                else if (evnt.key.code == sf::Keyboard::D)
                {
                    d = 1;
                }
                else if (evnt.key.code == sf::Keyboard::A)
                {
                    d = -1;
                }
            }
            else if (evnt.type == sf::Event::KeyReleased)
            {
                if (evnt.key.code == sf::Keyboard::W)
                {
                    w = 0;
                }
                else if (evnt.key.code == sf::Keyboard::S)
                {
                    w = 0;
                }
                else if (evnt.key.code == sf::Keyboard::D)
                {
                    d = 0;
                }
                else if (evnt.key.code == sf::Keyboard::A)
                {
                    d = 0;
                }
            }
            else if (evnt.type == sf::Event::MouseWheelMoved)
            {
                if (evnt.mouseWheel.delta != 0)
                {
                    view.zoom(pow(1.1, -evnt.mouseWheel.delta));
                }
            }
        }
        vx += d * dt;
        vy += -w * dt;

        vx = vx / pow(2, dt);
        vy = vy / pow(2, dt);

        grid = itt(grid);

        window.setView(view);
        draw(grid);
    }
    
    return 0;
}