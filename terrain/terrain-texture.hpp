#ifndef TERRAIN_TEXTURE_HPP_
#define TERRAIN_TEXTURE_HPP_

#include "entity-manager.hpp"

#include <SDL2/SDL_opengl.h>

#include "camera.hpp"
#include "enums.hpp"
#include "ini-manager.hpp"
#include "terrain-map.hpp"
#include "texture-manager.hpp"
#include "ztexture.hpp"
#include "world-fwd.hpp"

class terrain_texture : public entity_item {
public:
    terrain_texture();
    virtual ~terrain_texture();

    void init(texture_manager &texture_mgr,
              terrain_map const &terrain_map_entity,
              camera const &camera_object,
              ini_manager const &ini_mgr);
    void term();

    GLuint get_texture(GLuint zone);

    virtual void update();
    virtual void render();

private:
    void draw_layer(GLint origin_x, GLint origin_y, GLint size, GLint layer);
    void get_camera_zone();

    terrain_map const *map_;
    camera const *camera_;
    ini_manager const *ini_mgr_;

    GLushort *buffer_;
    ztexture **zone_texture_;
    GLuint layer_texture_[terrain::enums::LAYER_COUNT];
    GLuint *res_texture_;
    GLuint current_grid_;
    GLuint current_zone_;
    GLuint current_layer_;
    GLuint pixel_count_;
    GLuint build_time_;
    GLint ref_count_;
    GLint camera_zone_x_;
    GLint camera_zone_y_;
    GLint zone_size_;
    GLint max_resolution_;
    GLint map_texture_update_time_;
    GLint show_resolution_;
    GLint uv_scale_;
    GLuint zone_grid_;
};

#endif
