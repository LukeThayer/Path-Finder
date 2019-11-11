#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <math.h>
#include <algorithm>
#include <iostream>

#define MAP_HEIGHT_RESOLUTION 20
#define MAP_WIDTH_RESOLUTION 20
#define PIXEL_WIDTH 100
#define PIXEL_HEIGHT 100
class PathFinder : public olc::PixelGameEngine
{
public:
    enum Type //type of pixel
    {
        defualtPixel,
        searchedPixel,
        obsticlePixel,
        goalPixel,
        startPixel,
        pathPixel
    };
    struct sNode //store pixel location and type
    {
        Type pixelType;
        int x;
        int y;
        float cost;
    };
    enum State //what the program is doing currently
    {
        running,
        drawing
    };
    State state;

    PathFinder() // names the window
    {
        sAppName = "PathFinder";
        SetPixelMode(olc::Pixel::Mode::ALPHA);
    }

    bool OnUserCreate() override
    {
        Reset(); //creates an empty map
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {

        Input();              //looks for user input
        if (state == running) //begins program once prompted
        {
            if (searchIndex != goalIndex)
            {
                // printf("LOL SEARCHING");
                PathUpdate();
            }
            else
            {
                printf("GOAL FOUND");
            }
        }
        UpdateMap(); //updates the current map
        return true;
    }

    void setPixelType(int x, int y, Type type)
    {
        nodes[indexFromXY(x, y)].pixelType = type;
    }

private:
    bool drawTestMap = true;
    bool startNotExists = true;
    bool goalNotExists = true;
    int startIndex;
    int goalIndex;
    int searchIndex;
    int cheapestIndex;
    sNode *nodes = nullptr;

    int indexFromXY(int x, int y)
    {
        auto _x = std::clamp(x, 0, ScreenWidth() - 1);
        auto _y = std::clamp(y, 0, ScreenHeight() - 1);
        return _x + _y * ScreenWidth();
    }

    float costCalc(sNode *pixel)
    {
        if (pixel->pixelType == Type::obsticlePixel)
        {
            return std::numeric_limits<float>::max();
        }

        auto xd = pixel->x - nodes[goalIndex].x;
        auto xsq = powf(xd, 2);

        auto yd = pixel->y - nodes[goalIndex].y;
        auto ysq = powf(yd, 2);

        return sqrtf(xsq + ysq);
    }

    void PathUpdate()
    {        
        for (int y = -1; y <= 1; y++)
        {
            for (int x = -1; x <= 1; x++)
            {
                auto searchNode = &nodes[searchIndex];
                auto _x = searchNode->x + x;
                auto _y = searchNode->y + y;
                auto i = indexFromXY(_x, _y);
                auto pixel = &nodes[i];
                auto cost = costCalc(pixel);
                
                //SEARCH FILTER
                if ((x != 0 || y != 0) && (y == 0 || x == 0))
                {
                    if (pixel->cost == std::numeric_limits<float>::max())
                    {
                        pixel->cost = 0;
                    }
                    pixel->cost += cost;

                    if (pixel->pixelType == defualtPixel)
                    {
                        pixel->pixelType = searchedPixel;
                        auto s = "Cost{x:" + std::to_string(_x) + ", y:" + std::to_string(_y) + "}:" + std::to_string(cost);
                    }

                    if (cost <= nodes[cheapestIndex].cost)
                    {
                        cheapestIndex = i;
                    }
                    // else
                    // {
                        searchNode->cost += costCalc(searchNode);
                    // }
                }
            }
        }

        searchIndex = cheapestIndex;

        nodes[searchIndex].pixelType = pathPixel;

        return;
    }

    void Reset() // returns all pixels to defualt type
    {
        startNotExists = true;
        goalNotExists = true;
        state = drawing;
        if (nodes != nullptr)
        {
            delete nodes;
        }
        nodes = new sNode[ScreenHeight() * ScreenWidth()];
        for (int x = 0; x < ScreenWidth(); x++)
        {
            for (int y = 0; y < ScreenHeight(); y++)
            {
                nodes[indexFromXY(x, y)].cost = std::numeric_limits<float>::max();
                nodes[indexFromXY(x, y)].x = x;
                nodes[indexFromXY(x, y)].y = y;
                nodes[indexFromXY(x, y)].pixelType = defualtPixel;
            }
        }

        if (drawTestMap)
        {
            startNotExists = false;
            goalNotExists = false;

            setPixelType(2, 2, PathFinder::Type::startPixel);
            startIndex = indexFromXY(2, 2);

            for (int x = 0; x < 19; x++)
            {
                setPixelType(x, 10, PathFinder::Type::obsticlePixel);
            }

            setPixelType(18, 18, PathFinder::Type::goalPixel);
            goalIndex = indexFromXY(18, 18);
        }
    }

    olc::Pixel pixelFromNode(sNode* pixel) {
        uint8_t color = std::clamp<float>(pixel->cost / 10.0, 0, 255);

        if (indexFromXY(pixel->x, pixel->y) == searchIndex)
        {
            return olc::Pixel(255,0,255, color + 25);
        }
        return olc::Pixel(color, color, color);
    }

    void UpdateMap() // draws the map with most current data
    {
        for (int x = 0; x < ScreenWidth(); x++)
            for (int y = 0; y < ScreenHeight(); y++)
            {
                uint32_t index = indexFromXY(x,y);
                switch (nodes[index].pixelType)
                {
                case Type::defualtPixel:
                    Draw(x, y, olc::Pixel(olc::BLUE));
                    break;
                case Type::obsticlePixel:
                    Draw(x, y, olc::Pixel(olc::VERY_DARK_MAGENTA));
                    break;
                case Type::goalPixel:
                    Draw(x, y, olc::Pixel(olc::RED));
                    break;
                case Type::startPixel:
                    Draw(x, y, olc::Pixel(olc::GREEN));
                    break;
                case Type::pathPixel:
                case Type::searchedPixel:
                    Draw(x, y, pixelFromNode(&nodes[index]));
                    break;
                default:
                    break;
                }
        };

        FillRect(GetMouseX(), GetMouseY(), 1, 1, olc::Pixel(0, 0, 0, 64));
        return;
    }

    void Input() // reads in input//REWORK!!
    {
        if (state == drawing)
        {
            if (get_key('Q'))
            {
                nodes[GetMouseY() * ScreenHeight() + GetMouseX()].pixelType = obsticlePixel;
                nodes[GetMouseY() * ScreenHeight() + GetMouseX()].cost = 1000;
            }

            if (get_key('w'))
            {
                if (startNotExists)
                {
                    nodes[GetMouseY() * ScreenHeight() + GetMouseX()].pixelType = startPixel;
                    startNotExists = false;
                    startIndex = GetMouseY() * ScreenHeight() + GetMouseX();
                }
            }

            if (get_key('e'))
            {
                if (goalNotExists)
                {
                    nodes[GetMouseY() * ScreenHeight() + GetMouseX()].pixelType = goalPixel;
                    goalNotExists = false;
                    goalIndex = GetMouseY() * ScreenHeight() + GetMouseX();
                    nodes[goalIndex].cost = 0;
                }
            }
        }

        if (get_key('s'))
        {
            if (!goalNotExists && !startNotExists)
            {
                state = running;
                cheapestIndex = startIndex;
                searchIndex = startIndex;
            }
        }

        if (get_key('d'))
        {
            state = drawing;
        }

        if (get_key('a'))
        {
            Reset();
        }
    }
};

int main(int argc, char const *argv[])
{
    PathFinder demo;
    if (demo.Construct(MAP_HEIGHT_RESOLUTION, MAP_WIDTH_RESOLUTION, PIXEL_HEIGHT, PIXEL_WIDTH))
    {
        demo.Start();
    }

    return 0;
}
