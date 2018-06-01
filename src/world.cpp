//
// Created by jorda on 5/29/2018.
//

#include "helper.hpp"

using namespace std;

Tile::Tile(int xposition, int yposition, int type) {
    this->tile_type = type;
    switch (type) {
        case PLAINS:
            is_passable = true;
            movement_multiplier = 1.0;
            damage_factor = 0;
            break;
        case DESERT:
            is_passable = true;
            movement_multiplier = 0.5;
            damage_factor = 10;
            break;
        case MOUNTAINS:
            is_passable = false;
            movement_multiplier = 1.0;
            damage_factor = 0;
            break;
        case WATER:
            is_passable = false;
            movement_multiplier = 1.0;
            damage_factor = 0;
            break;
        default:
            is_passable = true;
            movement_multiplier = 1.0;
            damage_factor = INT32_MAX;
            break;
    }
    this->xposition = xposition;
    this->yposition = yposition;
    vertices = generateVertices(this->xposition, this->yposition, TILE_SIZE, TILE_SIZE);
}

void Tile::offsetTile(int xoffset, int yoffset) {
    xposition -= xoffset;
    yposition -= yoffset;
    vertices[0].position = Vector2f(xposition, yposition);
    vertices[1].position = Vector2f(xposition, yposition + TILE_SIZE);
    vertices[2].position = Vector2f(xposition + TILE_SIZE, yposition + TILE_SIZE);
    vertices[3].position = Vector2f(xposition + TILE_SIZE, yposition);
}

void Tile::draw(RenderTarget &target, RenderStates states) const {
    target.draw(vertices, states);
}

World::World(char *map_path, char *spawn_path) {
    resources::load(resources::PLAINS_TEXTURE);
    resources::load(resources::DESERT_TEXTURE);
    resources::load(resources::MOUNTAINS_TEXTURE);
    resources::load(resources::WATER_TEXTURE);

    int *tile_info = readBMP(map_path);

    this->world_width = tile_info[0];
    this->world_height = tile_info[1];

    this->tiles_size = this->world_width * this->world_height;
    this->tiles = new Tile[tiles_size];

    for(int i = 0; i < tiles_size; i++) {
        this->tiles[i] = Tile((i % this->world_width) * Tile::TILE_SIZE, (i / this->world_height) * Tile::TILE_SIZE, tile_info[i + 2]);
    }

    spawnEntities(spawn_path);

    xoffset = 0;
    yoffset = 0;
}

void World::spawnEntities(char *spawn_path) {
    FILE *spawn_file = fopen(spawn_path, "rb");
    char spawn_info[100];
    while(fgets(spawn_info, 100, spawn_file) != nullptr) {
        char *data = strtok(spawn_info, ",");
        int subtype = atoi(data);
        int type = subtype/100;
        data = strtok(NULL, ",");
        int xposition = atoi(data);
        data = strtok(NULL, ",");
        int yposition = atoi(data);
        int alt_xposition;
        int alt_yposition;
        switch(type){
            case Entity::PROD:
            case Entity::RSCH:
            case Entity::COLL:
                structures.emplace_back(Structure(xposition, yposition, subtype));
                alt_xposition = xtiles - xposition - (structures.back().xsize / Tile::TILE_SIZE);
                alt_yposition = ytiles - yposition - (structures.back().ysize / Tile::TILE_SIZE);
                structures.emplace_back(Structure(alt_xposition, alt_yposition, subtype));
                break;
            case Entity::RSRC:
                resources.emplace_back(Resource(xposition, yposition, subtype));
                alt_xposition = xtiles - xposition - (resources.back().xsize / Tile::TILE_SIZE);
                alt_yposition = ytiles - yposition - (resources.back().ysize / Tile::TILE_SIZE);
                resources.emplace_back(Resource(alt_xposition, alt_yposition, subtype));
                break;
            case Entity::UNIT:
                xposition *= Tile::TILE_SIZE;
                yposition *= Tile::TILE_SIZE;
                units.emplace_back(Unit(xposition, yposition, subtype));
                alt_xposition = (xtiles * Tile::TILE_SIZE) - xposition - units.back().xsize;
                alt_yposition = (ytiles * Tile::TILE_SIZE) - yposition - units.back().ysize;
                units.emplace_back(Unit(alt_xposition, alt_yposition, subtype));
                break;
            default:
                break;
        }
    }
}

void World::draw(RenderTarget &target, RenderStates states) const {
    int startx = max((xoffset / Tile::TILE_SIZE) - 1, 0);
    int starty = max((yoffset / Tile::TILE_SIZE) - 1, 0);
    int tempx = xoffset + target.getSize().x;
    int tempy = yoffset + target.getSize().y;
    int endx = min((tempx / Tile::TILE_SIZE) + 1, xtiles);
    int endy = min((tempy / Tile::TILE_SIZE) + 1, ytiles);
    for(int x = startx; x < endx; x++) {
        for(int y = starty; y < endy; y++) {
            Tile selected = tiles[x + y * xtiles];
            selected.offsetTile(xoffset, yoffset);
            target.draw(selected);
            selected.offsetTile(-1 * xoffset, -1 * yoffset);
        }
    }
    for(Resource r : resources) {
        if(isOnScreen(r, startx, starty, endx, endy)) {
            r.offsetEntity(xoffset, yoffset);
            target.draw(r);
            r.offsetEntity(-1 * xoffset, -1 * yoffset);
        }
    }
    for(Structure s : structures) {
        if(isOnScreen(s, startx, starty, endx, endy)) {
            s.offsetEntity(xoffset, yoffset);
            target.draw(s);
            s.offsetEntity(-1 * xoffset, -1 * yoffset);
        }
    }
    for(Unit u : units) {
        if(isOnScreen(u, startx, starty, endx, endy)) {
            u.offsetEntity(xoffset, yoffset);
            target.draw(u);
            u.offsetEntity(-1 * xoffset, -1 * yoffset);
        }
    }
}