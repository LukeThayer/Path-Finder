#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <math.h>
#include <algorithm>
#include <iostream>

#define MAP_HEIGHT_RESOLUTION 20
#define MAP_WIDTH_RESOLUTION 20
#define PIXEL_WIDTH 35
#define PIXEL_HEIGHT 35
class PathFinder : public olc::PixelGameEngine
{
public:
    enum Type //type of pixel
    {
        defualtPixel,
        visitedPixel,
        obsticalPixel,
        goalPixel,
        startPixel,
        pathPixel
    };
    struct sNode //store pixel location and type
    {
        Type pixelType;
        int x;
        int y;
        double cost;
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
            if (nodes[searchIndex].cost != 0)
            {
                PathUpdate();
            }
        }
        UpdateMap(); //updates the current map
        return true;
    }

private:
    bool startNotExists = true;
    bool goalNotExists = true;
    int startIndex;
    int goalIndex;
    int searchIndex;
    int cheapestIndex;
    sNode *nodes = nullptr;

    void PathUpdate()
    {
        int localIndex = 0;
        int localPixelIndex[4];
        nodes[cheapestIndex].cost = 100;

        for (int y = -1; y <= 1; y++)
        {
            for (int x = -1; x <= 1; x++)
            {
                if ((x != 0 || y != 0) && (y == 0 || x == 0))
                {
                    localPixelIndex[localIndex] = (nodes[searchIndex].x + x) + ((nodes[searchIndex].y + y) * ScreenHeight());

                    if (nodes[localPixelIndex[localIndex]].pixelType == defualtPixel)
                    {
                        CostCalc(&nodes[localPixelIndex[localIndex]]);

                        nodes[localPixelIndex[localIndex]].pixelType = visitedPixel;
                    }

                    if (nodes[localPixelIndex[localIndex]].cost < nodes[cheapestIndex].cost)
                    {
                        cheapestIndex = localPixelIndex[localIndex];
                    }
                    localIndex++;
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
        nodes = new sNode[ScreenHeight() * ScreenWidth()];
        for (int x = 0; x < ScreenWidth(); x++)
        {
            for (int y = 0; y < ScreenHeight(); y++)
            {
                nodes[y * ScreenHeight() + x].cost = 0;
                nodes[y * ScreenHeight() + x].x = x;
                nodes[y * ScreenHeight() + x].y = y;
                nodes[y * ScreenHeight() + x].pixelType = defualtPixel;
            }
        }
    }
    void UpdateMap() // draws the map with most current data
    {
        for (int x = 0; x < ScreenWidth(); x++)
            for (int y = 0; y < ScreenHeight(); y++)
            {
                uint32_t index = y * ScreenHeight() + x;
                switch (nodes[index].pixelType)
                {
                case 0:
                    Draw(x, y, olc::Pixel(olc::BLUE));
                    break;
                case 1:
                    Draw(x, y, olc::Pixel(olc::CYAN));
                    break;
                case 2:
                    Draw(x, y, olc::Pixel(olc::DARK_GREY));
                    break;
                case 3:
                    Draw(x, y, olc::Pixel(olc::RED));
                    break;
                case 4:
                    Draw(x, y, olc::Pixel(olc::GREEN));
                    break;
                case 5:
                    Draw(x, y, olc::Pixel(olc::YELLOW));
                default:
                    break;
                }
            };

        FillRect(GetMouseX(), GetMouseY(), 1, 1, olc::Pixel(olc::BLACK));
        return;
    }

    void Input() // reads in input//REWORK!!
    {
        if (state == drawing)
        {
            if (get_key('Q'))
            {

                nodes[GetMouseY() * ScreenHeight() + GetMouseX()].pixelType = obsticalPixel;
                nodes[GetMouseY() * ScreenHeight() + GetMouseX()].cost = 1000;
            };
            if (get_key('w'))
            {
                if (startNotExists)
                {
                    nodes[GetMouseY() * ScreenHeight() + GetMouseX()].pixelType = startPixel;
                    startNotExists = false;
                    startIndex = GetMouseY() * ScreenHeight() + GetMouseX();
                    searchIndex = startIndex;

                    CostCalc(&nodes[startIndex]);
                }
            };
            if (get_key('e'))
            {
                if (goalNotExists)
                {
                    nodes[GetMouseY() * ScreenHeight() + GetMouseX()].pixelType = goalPixel;
                    goalNotExists = false;
                    goalIndex = GetMouseY() * ScreenHeight() + GetMouseX();
                    nodes[goalIndex].cost = 0;
                }
            };
        }
        if (get_key('s'))
        {
            if (!goalNotExists && !startNotExists)
            {
                state = running;
            }
        };

        if (get_key('d'))
        {
            state = drawing;
        };
        if (get_key('a'))
        {
            Reset();
        };
    }
    void CostCalc(sNode *pixel)
    {
        pixel->cost = (sqrtf(float(powf((pixel->x - nodes[goalIndex].x), 2) + float(powf((pixel->y - nodes[goalIndex].y), 2)))));
    }
};

int main(int argc, char const *argv[])
{
    PathFinder demo;
    if (demo.Construct(MAP_HEIGHT_RESOLUTION, MAP_WIDTH_RESOLUTION, PIXEL_HEIGHT, PIXEL_WIDTH))
        demo.Start();

    return 0;
}
