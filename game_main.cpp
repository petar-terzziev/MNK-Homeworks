#include <iostream>
#include <vector>
#include "game.h"
using namespace std;
int main() {

    LevelDescription desc;
    desc.playableArea = {
            { {80, 112}, {197, 388} }, // left (start) green area
            { {197, 350}, {276, 388} }, // bottom connector
            { {240, 150}, {556, 350} }, // main area with enemies
            { {520, 112}, {600, 150} }, // top connector
            { {600, 112}, {717, 388} }, // right (end) green area
    };
    desc.start = { {80, 112}, {197, 388} }; // same as first item in playableArea
    desc.end = { {600, 112}, {717, 388} }; // same as last item in playableArea
    desc.enemyPaths = {
            { {258, 170}, {538, 170} }, // start at left
            { {538, 210}, {258, 210} }, // start as right but offset by 40 in Y
            { {258, 250}, {538, 250} }, // start at left
            { {538, 290}, {258, 290} }, // starts at right
    };
    generateBestPath(desc);

    return 0;
}






