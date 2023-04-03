#ifndef globals_h

struct plant{
    int health; //this is the amount of damage that the plant can take
    int type; 
    //types:
    //0: shield
    //1: sunflower
    //2: peashooter
    //3: mine
    //4: jalapeno
    int damage; //amount of damage it causes on a zombie
    int x_grid_location;
    int y_grid_location;
    int cost; //cost in sunflowers
    char* name;
};

struct zombie{
    int health;
    int type;
    //types:
    //0: slowest zombie in levels 0-3
    //1: medium speed zombie
    //2: fastest zombie
    int x_grid_location;
    int y_grid_location;
    int speed; //number of pixels it will move across the screen by
    int damage;
};

#endif