/*
 * map-texture.cpp
 * 2006 Shamus Young
 *
 * This generates the zone textures. I'm still not happy with how this works,
 * but it can't be improved until you decide how you want to use it.
 *
 * Want static lighting? then dump this incremental update stuff and just
 * make these tectures during your "loading screen". This will speed up
 * framerate and make everything much simpler.
 *
 * Want moving shadows? Then this needs work. There are actually two groups of
 * textures. One group is in use and SHOULD NOT BE CHANGED, or the user will
 * see the textures changing. The other group is the ones being worked on now.
 * Every time one set is complete, it goes into use. Once the old ones fall
 * out of use, you can start on them. All of this is a drag on FPS and you
 * still have the problem of the shadows jumping whenever you switch to the new
 * texture set.
 */

#include "map-texture.hpp"

#include <SDL.h>
#include <SDL_opengl.h>

#include "camera.hpp"
#include "console.hpp"
#include "ini.hpp"
#include "texture.hpp"
#include "map.hpp"
#include "ztexture.hpp"

enum {
    GRID_FRONT,
    GRID_BACK,
    GRID_COUNT,
};

static unsigned char *buffer;
static ztexture **zone_texture;
static unsigned int layer_texture[LAYER_COUNT];
static unsigned int *res_texture;
static unsigned int current_grid;
static unsigned int current_zone;
static unsigned int current_layer;
static unsigned int pixel_count;
static unsigned int build_time;
static int ref_count;
static int camera_zone_x;
static int camera_zone_y;
static int zone_size;
static int max_resolution;
static int map_texture_update_time;
static int show_resolution;
static int uv_scale;
static unsigned int zone_grid;

// Which zone is the camera over?
static void get_camera_zone(void)
{
    gl_vector_3d cam;
    int zone_size;

    cam = camera_position();
    zone_size = map_size() / zone_grid;
    camera_zone_x = ((int)cam.x_ + (map_size() / 2)) / zone_size;
    camera_zone_y = ((int)cam.y_ + (map_size() / 2)) / zone_size;
}

// Get the current texture for the requested zone
unsigned int map_texture(unsigned int zone)
{
    int grid;

    if(current_grid == GRID_FRONT) {
        grid = GRID_BACK;
    }
    else {
        grid = GRID_FRONT;
    }

    // If the very last texture is requested, then we know the terrain is about
    // to change over to a new set, and we can increment our ref count.
    // Once this reaches 2, we know both terrains (the one being built and the
    // one in use) are using our latest texture set, and it is safe to work on
    // the old set.
    if(zone == ((zone_grid * zone_grid) - 1)) {
        ++ref_count;
    }
    if(!zone_texture[grid][zone].ready_) {
        return 0;
    }

    return zone_texture[grid][zone].texture_;
}

void map_texture_init(void)
{
    ini_manager ini_mgr;
    max_resolution = ini_mgr.get_int("Map Texture", "max_resolution");

    res_texture = new unsigned int[max_resolution];

    map_texture_update_time = 
        ini_mgr.get_int("Map Texture", "map_texture_update_time");

    show_resolution = ini_mgr.get_int("Map Texture", "show_resolution");
    uv_scale = ini_mgr.get_int("Map Texture", "uv_scale");
    zone_grid = ini_mgr.get_int("Map Settings", "zone_grid");

    zone_texture = new ztexture*[GRID_COUNT];
    zone_texture[0] = new ztexture[GRID_COUNT * (zone_grid * zone_grid)];

    for(int i = 1; i < GRID_COUNT; ++i) {
        zone_texture[i] = zone_texture[i - 1] + (zone_grid * zone_grid);
    }

    int grid;
    unsigned int zone;

    for(grid = GRID_FRONT; grid < GRID_COUNT; ++grid) {
        for(zone = 0; zone < (zone_grid * zone_grid); ++zone) {
            zone_texture[grid][zone].size_ = 0;
            zone_texture[grid][zone].ready_ = false;
            glGenTextures(1, &zone_texture[grid][zone].texture_);
        }
    }

    buffer = new unsigned char[(max_resolution * max_resolution) * 4];
    zone_size = map_size() / zone_grid;
    layer_texture[LAYER_GRASS] = texture_from_name("grassa512");
    layer_texture[LAYER_LOWGRASS] = texture_from_name("grassb512");
    layer_texture[LAYER_SAND] = texture_from_name("sand512");
    layer_texture[LAYER_ROCK] = texture_from_name("rock512");
    layer_texture[LAYER_DIRT] = texture_from_name("dirt512");

    if(show_resolution != 0) {
        res_texture[8] = texture_from_name("n8");
        res_texture[16] = texture_from_name("n16");
        res_texture[32] = texture_from_name("n32");
        res_texture[64] = texture_from_name("n64");
        res_texture[128] = texture_from_name("n128");
        res_texture[256] = texture_from_name("n256");
        res_texture[512] = texture_from_name("n512");
    }

    get_camera_zone();
}

void map_texture_term(void)
{
    delete[] res_texture;
    delete[] buffer;
}

static void draw_layer(int origin_x, int origin_y, int size, int layer)
{
    gl_rgba color1;
    gl_rgba color2;
    int step;
    int x;
    int y;
    int xx;
    int yy;
    int y2;
    float cell_size;
    bool drawing;
    bool blank;

    glBindTexture(GL_TEXTURE_2D, layer_texture[layer]);
    cell_size = size / (float)zone_size;

    if(cell_size >= 1.0f) {
        step = 1;
    }
    else {
        step = (int)(1.0f / cell_size);
    }

    if(layer == LAYER_GRASS) {
        glColor3f(1, 1, 1);
        glBegin(GL_QUAD_STRIP);
        glTexCoord2f(0.0f, 0.0f);
        glVertex2i(0, 0);
        glTexCoord2f(0.0f, uv_scale);
        glVertex2i(0, size);
        glTexCoord2f(uv_scale, 0);
        glVertex2i(size, 0);
        glTexCoord2f(uv_scale, uv_scale);
        glVertex2i(size, size);
        glEnd();

        return;
    }

    if(layer == LAYER_LIGHTING) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glBlendFunc(GL_DST_COLOR, GL_ZERO);

        for(y = -1; y < ((zone_size + step) + 1); y += step) {
            yy = origin_y + y;
            y2 = y + 1;
            glBegin(GL_QUAD_STRIP);
            for(x = -1; x < ((zone_size + step) + 1); x += step) {
                xx = origin_x + x;
                color1 = map_light(xx, yy);
                color2 = map_light(xx, yy + step);
                glColor3fv(&color1.red_);
                glVertex2i((int)((float)x * cell_size),
                           (int)((float)y * cell_size));

                glColor3fv(&color2.red_);
                glVertex2i((int)((float)x * cell_size),
                           (int)((float)y2 * cell_size));
            }

            glEnd();
        }

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        return;
    }

    if(layer == LAYER_SPECIAL) {
        glBindTexture(GL_TEXTURE_2D, res_texture[size]);
        glBlendFunc(GL_SRC_COLOR, GL_ONE);

        if(current_grid != GRID_FRONT) {
            glColor4f(0.6f, 0.0f, 0.6f, 0.3f);
        }
        else {
            glColor4f(0.6f, 0.6f, 0.0f, 0.3f);
        }

        glBegin(GL_QUAD_STRIP);
        glTexCoord2f(0.0f, 0.0f);
        glVertex2i(0, 0);
        glTexCoord2f(0.0f, 1.0f);
        glVertex2i(0, size);
        glTexCoord2f(1.0f, 0.0f);
        glVertex2i(size, 0);
        glTexCoord2f(1.0f, 1.0f);
        glVertex2i(size, size);
        glEnd();
        
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        return;
    }

    color2 = gl_rgba(1.0f);
    color1 = color2;

    for(y = -1; y < ((zone_size + step) + step); y += step) {
        drawing = false;
        blank = true;
        yy = origin_y + y;
        y2 = y + 1;

        for(x = -1; x < ((zone_size + step) + 1); x += step) {
            xx = origin_x + x;
            color1.alpha_ = map_layer(xx, yy, layer);
            color2.alpha_ = map_layer(xx, yy + step, layer);

            if((color1.alpha_ == 0.0f) && (color2.alpha_ == 0.0f)) {
                if(blank) {
                    if(drawing) {
                        glEnd();
                        drawing = false;
                    }
                }

                blank = true;
            }
            else if(!drawing) {
                glBegin(GL_QUAD_STRIP);
                blank = false;
                drawing = true;
            }

            if(drawing) {
                glColor4fv(&color1.red_);
                glTexCoord2f(((float)x / (float)zone_size) * uv_scale,
                             ((float)y / (float)zone_size) * uv_scale);

                glVertex2i((int)((float)x * cell_size),
                           (int)((float)y * cell_size));

                glColor4fv(&color2.red_);
                glTexCoord2f(((float)x / (float)zone_size) * uv_scale,
                             ((float)y / (float)zone_size) * uv_scale);

                glVertex2i((int)((float)x * cell_size),
                           (int)((float)y * cell_size));
            }
        }

        if(drawing) {
            glEnd();
        }
    }
}

void map_texture_update()
{
    ztexture *z;
    unsigned long end;
    unsigned long now;
    int delta_x;
    int delta_y;
    int delta;
    int zone_x;
    int zone_y;
    int origin_x;
    int origin_y;

    if(ref_count < 2) {
        return;
    }

    now = SDL_GetTicks();
    end = now + map_texture_update_time;

    while(SDL_GetTicks() < end) {
        z = &zone_texture[current_grid][current_zone];
        zone_x = current_zone % zone_grid;
        zone_y = (current_zone - zone_x) / zone_grid;
        origin_x = zone_x * zone_size;
        origin_y = zone_y * zone_size;
        glBindTexture(GL_TEXTURE_2D, z->texture_);

        if(current_layer == 0) {
            delta_x = (camera_zone_x - zone_x);

            if((camera_zone_x - zone_x) < 0) {
                delta_x *= -1;
            }

            delta_y = (camera_zone_y - zone_y);

            if((camera_zone_y - zone_y) < 0) {
                delta_y *= -1;
            }

            if(delta_x > delta_y) {
                delta = delta_x;
            }
            else {
                delta = delta_y;
            }

            if(delta < 2) {
                z->size_ = max_resolution;
            }
            else if(delta < 3) {
                z->size_ = max_resolution / 2;
            }
            else if(delta < 4) {
                z->size_ = max_resolution / 4;
            }
            else if(delta < 6) {
                z->size_ = max_resolution / 8;
            }
            else if(delta < 7) {
                z->size_ = max_resolution / 16;
            }
            else if(delta < 8) {
                z->size_ = max_resolution / 32;
            }
            else {
                z->size_ = max_resolution / 64;
            }

            if(z->size_ < 8) {
                z->size_ = 8;
            }
            else if(z->size_ > 512) {
                z->size_ = 512;
            }

            glTexImage2D(GL_TEXTURE_2D,
                         0,
                         3,
                         z->size_, 
                         z->size_,
                         0,
                         GL_RGB,
                         GL_UNSIGNED_BYTE,
                         buffer);
        }

        glEnable(GL_TEXTURE_2D);
        glDisable(GL_CULL_FACE);
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
        glLoadIdentity();
        glViewport(0, 0, z->size_, z->size_);
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0.0f, z->size_, 0, z->size_, 0, 1.0f);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glEnable(GL_BLEND);
        glBindTexture(GL_TEXTURE_2D, z->texture_);
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_QUAD_STRIP);
        glTexCoord2f(0.0f, 0.0f);
        glVertex2i(0, 0);
        glTexCoord2f(0.0f, 1.0f);
        glVertex2i(0, z->size_);
        glTexCoord2f(1.0f, 0.0f);
        glVertex2i(z->size_, 0);
        glTexCoord2f(1.0f, 1.0f);
        glVertex2i(z->size_, z->size_);
        glEnd();

        while((current_layer != LAYER_COUNT) && (SDL_GetTicks() < end)) {
            draw_layer(origin_x, origin_y, z->size_, current_layer);
            ++current_layer;
        }

        if((show_resolution != 0) && (current_layer == LAYER_COUNT)) {
            draw_layer(origin_x, origin_y, z->size_, LAYER_SPECIAL);
        }
        
        if(current_layer == LAYER_COUNT) {
            draw_layer(origin_x, origin_y, z->size_, LAYER_LIGHTING);
        }

        // Save the result in our block texture
        glBindTexture(GL_TEXTURE_2D, z->texture_);
        glReadPixels(0, 0, z->size_, z->size_, GL_RGB, GL_UNSIGNED_BYTE, buffer);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     3,
                     z->size_,
                     z->size_,
                     0,
                     GL_RGB,
                     GL_UNSIGNED_BYTE,
                     buffer);

        // Enable the following line to enable mipmapping
        // gluBuild2DMipmaps(GL_TEXTURE_2D,
        //                   4,
        //                   TEXTURE_SIZE,
        //                   TEXTURE_SIZE,
        //                   GL_RGB,
        //                   GL_UNSIGNED_BYTE,
        //                   buffer);

        // Restore the projection matrix and cleanup
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);

        if(current_layer == LAYER_COUNT) {
            // Did we just finish this texture?
            pixel_count += (z->size_ * z->size_);
            current_layer = 0;
            z->ready_ = true;
            ++current_zone;

            if(current_zone == (zone_grid * zone_grid)) {
                // Little debug stuff here. Figure out how many 
                // Mb of memory we just ate.
                float meg = (float)pixel_count / 1048576.0f;
                console("Zonemap: %1.2fMb of data in %dms",
                        meg * 4.0f,
                        build_time);

                pixel_count = 0;
                build_time = 0;
                
                if(current_grid == GRID_FRONT) {
                    current_grid = GRID_BACK;
                }
                else {
                    current_grid = GRID_FRONT;
                }

                current_zone = 0;
                ref_count = 0;
                get_camera_zone();
            }
        }
    }

    build_time += (SDL_GetTicks() - now);
}
