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
int tk = 100;//задержка 

float VCam = 1.;//скорость движения камеры
float k = 0.9;//коэффициент стремления

int rd = 10;//радиус кисити
int mx;
int my;

bool dr = false;

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
            sr_zn = 2*abs(sr(grid, x0, y0) - grid[x0][y0]);
            if (copy[x0][y0] > 0.5)
            {
                copy[x0][y0] -= sr_zn * k;
            }
            else
            {
                copy[x0][y0] += sr_zn * k;
            }
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

    while (window.isOpen())
    {
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
            }
            else if (evnt.type == sf::Event::MouseButtonPressed)
            {
                if (evnt.mouseButton.button == 0)
                {
                    dr = true;
                    //левая кнопка мыши
                }
            }
            else if (evnt.type == sf::Event::MouseButtonReleased)
            {
                if (evnt.mouseButton.button == 0)
                {
                    dr = false;
                    //левая кнопка мыши
                }
            }
        }
        if (dr)
        {
            mx = evnt.mouseButton.x / float(w / float(x));
            my = evnt.mouseButton.y / float(h / float(y));

            for (int i = 0; i < x; i++)
            {
                for (int j = 0; j < y; j++)
                {
                    if (sqrt(pow(i - mx, 2) + pow(j - my, 2)) < rd)
                    {
                        grid[i][j] = 1;
                    }
                }
            }
        }
        grid = itt(grid);

        draw(grid);
    }
    
    return 0;
}