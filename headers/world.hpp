//
// Created by jorda on 5/27/2018.
//

#ifndef RTMOBAS_GAME_TILE_H
#define RTMOBAS_GAME_TILE_H

using namespace sf;
using namespace std;

class Tile : public Drawable {

private:
    /** Allows window.draw(Tile) to be used in SFML */
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

    /** Position of corners for tile */
    VertexArray vertices;

    /** Path to the texture for the entity */
    Texture texture;
public:
    /** Size of tiles, which is the same for all tiles */
    static const int TILE_SIZE = 16;

    /** Position of top left corner of tile */
    int xposition;
    int yposition;

    /** True if an entity can be drawn on top of this tile, false otherwise */
    bool is_passable;

    /** Modifier the the speed that entities move over this tile. Standard is 1.0 */
    double movement_multiplier;

    /** Amount of damage tile inflicts to units every tick */
    int damage_factor;

    /** Used in constructor for assigning several properties for each tile type */
    class Types{
    public:
        static const int PLAINS = 0x008000;
        static const int MOUNTAINS = 0x808040;
        static const int DESERT = 0xC0C000;
        static const int WATER = 0x004080;
    };

    int type;

    /** Constructors */
    Tile() = default;
    Tile(int xposition, int yposition, int type);

    void offsetTile(int xoffset, int yoffset);
};

class World : public Drawable {

private:
    /** Allows window.draw(World) to be used in SFML */
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

public:
    /** Array of all tiles contained in the world */
    Tile* tiles = nullptr;

    /** Lists of all entities in the world. Resources rendered first, then structures, then units */
    list<Resource> resources;
    list<Structure> structures;
    list<Unit> units;

    /** Number of tiles in the world */
    int tiles_size;

    /** Keeps track of whether or not a tile was modified */
    bool tiles_modified;

    /** Dimensions of the world, in units of tiles */
    int xtiles;
    int ytiles;

    /** Offset of where to start rendering world */
    int xoffset;
    int yoffset;

    World(char *map_path, char *spawn_path);

    void spawnEntities(char *spawn_path);
};


#endif //RTMOBAS_GAME_TILE_H