
#include <vector>
#include <cmath>
#include <ctime>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <unordered_map>
#include <iomanip>

const int PLAYER_SPEED = 10;
const int ENEMY_SPEED = 15;

const int POPULATION_SIZE = 1000;
const int BUDGET = 300;
const int GENERATIONS = 103;
const double MUTATION_RATE = 0.5;

enum Direction {
	directionNone = 0,
	directionUp = 1 << 0,
	directionDown = 1 << 1,
	directionLeft = 1 << 2,
	directionRight = 1 << 3,
};

struct Point {
	int x;
	int y;
	bool operator==(const Point &o){
	    return x==o.x && y==o.y;
	}
};

struct Box {
	Point topLeft;
	Point bottomRight;
};

struct EnemyPath {
	Point start;
	Point end;
};
bool in_box(const Point &p, const Box &b ){
    return p.x >= b.topLeft.x && p.x <= b.bottomRight.x && p.y <= b.bottomRight.y && p.y >= b.topLeft.y;
}
///returns a random direction
Direction random_direction(){
    int s = rand() % 5;
    switch (s) {
        case 0:
            return directionNone;
        case 1:
            return directionUp;
        case 2:
            return directionDown;
        case 3:
            return directionLeft;
        case 4:
            return directionRight;
//        case 5:
//            return directionRight|directionUp;
//        case 6:
//            return directionLeft|directionUp;
//        case 7:
//            return directionRight|directionDown;
//        case 8:
//            return directionLeft|directionDown;

    }
}
Point move_point(const Point& p,Direction d) {
    switch (d) {
        case directionDown:
            return {p.x, (p.y + PLAYER_SPEED)};
        case directionUp:
            return {p.x, (p.y - PLAYER_SPEED)};
        case directionLeft:
            return {(p.x - PLAYER_SPEED), p.y};

        case directionRight:
            return {(p.x + PLAYER_SPEED), p.y};
        case directionNone:
            return p;


    }
}
struct LevelDescription {
    std::vector<Box> playableArea; ///< The union of these boxes makes the level's area
    Box start; ///< The starting area, the player starts at the center of this box
    Box end; ///< The player needs to touch this box to finish succesfully
    std::vector<EnemyPath> enemyPaths; ///< All enemies in the level, if a player touches them he looses

    bool in_playable_area(const Point& p) const{
        for(Box b: playableArea){

            if (in_box(p,b)) {
                return true;
            }
        }
        return false;

    }
    bool in_playable_area_with_direction(const Point &p,Direction d) const{

        Point np = move_point(p,d);
        return in_playable_area(np);


    }

    double dist_to_finish(const Point &p) const{
        return (std::sqrt(std::pow((end.topLeft.x - p.x),2)+std::pow((end.topLeft.y - p.y),2)*1.0));
    }



};
/// represents individual in the population. Holds current position and path they've taken;
struct Individual {
    Point pos;
    std::vector<Direction> path;
    //with those directions I went to those points;
    std::vector<Point> moves;
    bool dead;
    bool reached;
    double dtf;

    Individual (): path(BUDGET)
    ,moves(BUDGET){}

    bool operator <(const Individual &o){
        if (reached && o.reached ) {
            std::cout<<"has reached ?!"<<std::endl;
            return path.size() < o.path.size();
        }
        else if(reached) {
            std::cout<<"has reached ?!"<<std::endl;
            return false;
        }
        else if(o.reached) {
            std::cout<<"has reached ?!"<<std::endl;
            return true;
        }
        else
            return dtf<o.dtf;


    }

    void mutate(const LevelDescription & l){
        double rand;
        Direction d;
        for(int i = 0;i < BUDGET;i++) {
            rand = (double) (std::rand() % 10) / 10;
            if (dead && i > (path.size() - 10)) {
                rand = (double) (std::rand() % 2) / 10;
            }
            if (rand > MUTATION_RATE) {
                do {
                    d = random_direction();
                } while (!l.in_playable_area_with_direction(moves[i], d));
                path[i] = d;
                moves[i] = move_point(moves[i], d);

            }
        }
        pos = moves[BUDGET-1];

    }

};




struct Enemy {
    Point pos;
    EnemyPath path;
    Direction d;

    void move_enemy(){
        if(pos == path.start){
            d = directionRight;
            pos.x+=ENEMY_SPEED;
        }
        else if (pos == path.end){
            d = directionLeft;
            pos.x -= ENEMY_SPEED;
        }
        else if (d == directionRight)
            pos.x +=ENEMY_SPEED;
        else
            pos.x -=ENEMY_SPEED;

    }

};







struct Grid {
    std::vector<std::vector<char>> grid;
    explicit Grid(const LevelDescription &ld):
    grid(ld.start.topLeft.y,std::vector<char> (ld.end.bottomRight.x, ' '))
    {}

    void print (const std::vector<Individual> &inds, const std::vector<Enemy> &es){
        for ( auto &i : inds ){
            grid[i.pos.x][i.pos.y] = '*';
        }
        for(auto &e : es){
            grid[e.pos.x] [e.pos.y] = 'o';
        }
    }
};





///generate initial population
void  gen_population(std::vector<Individual> &population,const LevelDescription &level){

    for (int i = 0; i<POPULATION_SIZE; i++){
        for(int j = 0; j<BUDGET;j++) {

                Direction d;
                do {
                    d = random_direction();
                } while (!level.in_playable_area_with_direction(population[i].pos, d));

                population[i].pos = move_point(population[i].pos, d);
                population[i].moves[j] = population[i].pos;
                population[i].path[j] = d;
                population[i].dtf = level.dist_to_finish(population[i].pos);
            }

        }
    }

void move_enemies( std::vector<Enemy> &enemies){
    for (auto & e : enemies){
        e.move_enemy();
    }
}

// a bit slow
void check_collisions (std::vector<Individual> &population, std::vector<Enemy> &enemies){
    for(auto &i : population){
        for(auto &enemie : enemies){
            if(i.pos == enemie.pos)
                i.dead = true;
        }
    }
}









/// Based on a level description generate what commands would a player give to complete the level
/// Direction may be combined, like directionDown | directionRight to move diagonally to down-right
std::vector<Direction> generateBestPath(const LevelDescription &level){
    std::vector<Individual> population (POPULATION_SIZE);
    std::vector<Enemy> enemies (level.enemyPaths.size());
    for (int i =0;i<POPULATION_SIZE;i++){
        population [i].pos = level.start.topLeft;
        population[i].dead = false;
        population[i].reached = false;
        population[i].dtf = level.dist_to_finish(population[i].pos);

    }
    for (int i = 0; i < enemies.size();i++){
        enemies[i].pos = level.enemyPaths[i].start;
        enemies[i].path = level.enemyPaths[i];
    }

    gen_population(population, level);


    for(int j =0 ;j<GENERATIONS;j++) {



        std::vector<Individual> new_gen (POPULATION_SIZE);
        std::sort(population.begin(), population.end());
       std::cout<<"Generation: "<< j << '\n' << "Best dtf : "<< population[0].dtf<<std::endl;







        new_gen[0] = population[0];
        for (int i = 1; i < POPULATION_SIZE; i++) {
              new_gen[i] =population[i];

               new_gen[i].mutate(level);
                new_gen[i].dtf = level.dist_to_finish(new_gen[i].pos);


        }
        population = new_gen;

    }
    return population[0].path;

}
