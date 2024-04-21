#ifndef IMAGE_MANAGER_H
#define IMAGE_MANAGER_H
#include "Common.h"
#include <filesystem>
#include <cstdlib>
#include "ConfigManager.h"
#include "MapHelper.h"
#include "WindowRenderer.h"
#include "Camera.h"

class UIRenderRequest
{
public:
    std::string image_name;
    int x;
    int y;
    SDL_Color color = {255, 255, 255, 255};
    int sorting_order = 0;
    UIRenderRequest(const std::string& image_name, int x, int y)
    {
        this->image_name = image_name;
        this->x = x;
        this->y = y;
    }
};

class ImageRenderRequest
{
public:
    std::string name;
    SDL_Color color;
    float x;
    float y;
    int sorting_order;
    int rotation;
    float scale_x;
    float scale_y;
    float pivot_x;
    float pivot_y;
    ImageRenderRequest(const std::string& name, float x, float y)
    {
        this->name = name;
        this->x = x;
        this->y = y;
        this->color = {255, 255, 255, 255};
        this->sorting_order = 0;
        this->rotation = 0;
        this->scale_x = 1;
        this->scale_y = 1;
        this->pivot_x = 0.5f;
        this->pivot_y = 0.5f;
    }
    ImageRenderRequest(const std::string& name, float x, float y, SDL_Color color, int sorting_order, float rotation, float scale_x, float scale_y, float pivot_x, float pivot_y)
    {
        this->name = name;
        this->x = x;
        this->y = y;
        this->color = color;
        this->sorting_order = sorting_order;
        this->rotation = rotation;
        this->scale_x = scale_x;
        this->scale_y = scale_y;
        this->pivot_x = pivot_x;
        this->pivot_y = pivot_y;
    }   
};

class DrawPixelRequest
{
public:
    int x;
    int y;
    SDL_Color color;
    DrawPixelRequest(int x, int y, SDL_Color color)
    {
        this->x = x;
        this->y = y;
        this->color = color;
    }
};


class ImageManager
{
public:
static inline bool EDITOR_MODE = false;
static inline std::vector<std::string> intro_images;
static inline std::vector<SDL_Texture*> intro_textures;
static inline size_t intro_img_index = 0;
static inline std::unordered_map<std::string, SDL_Texture*> loaded_textures;
static inline std::string hp_img_name;
static inline std::string bad_ending_img_name;
static inline std::string good_ending_img_name;
// static inline std::queue<UIRenderRequest> ui_render_requests;
static inline std::vector<UIRenderRequest> ui_render_requests;
static inline std::vector<ImageRenderRequest> image_render_requests;
static inline std::vector<DrawPixelRequest> draw_pixel_requests;

static void load_intro_imgs_and_health(SDL_Renderer* renderer)
{
	rapidjson::Document document;
	ConfigManager::read_json_file("resources/game.config", document);
    
	if (document.HasMember("intro_image") && document["intro_image"].IsArray()) 
	{
        intro_images.reserve(document["intro_image"].Size());
        const rapidjson::Value& intro_image = document["intro_image"];
        for (rapidjson::SizeType i = 0; i < intro_image.Size(); i++)
        {
            // adding png to the end of the string.
            std::string img = intro_image[i].GetString();
            img += ".png";
            intro_images.push_back(img);
        }
	}
    if (document.HasMember("hp_image") && document["hp_image"].IsString())
    {
        std::string hp_img = document["hp_image"].GetString();
        hp_img_name = hp_img;
        hp_img += ".png";
        auto full_path = "resources/images/" + hp_img;
        if (!std::filesystem::exists(full_path))
        {
            std::cout << "error: missing image " << hp_img;
            exit(0);
        }
        SDL_Texture* texture = IMG_LoadTexture(renderer, full_path.c_str());
        if (!texture)
        {
            std::cout << "Failed to load texture: " << IMG_GetError() << std::endl;
        }
        loaded_textures[hp_img_name] = texture;
    }
}

static void load_intro_textures(SDL_Renderer* renderer)
{
    const std::filesystem::path images_path = "resources/images/";
    for (const auto& img_name : intro_images)
    {
        auto full_path = images_path / img_name;
        if (!std::filesystem::exists(full_path))
        {
            // remove .png from image name
            std::string img_name_without_extension = img_name.substr(0, img_name.find(".png"));
            std::cout << "error: missing image " << img_name_without_extension;
            exit(0);
        }
        
        SDL_Texture* texture = IMG_LoadTexture(renderer, full_path.string().c_str());
        if (!texture)
        {
            std::cout << "Failed to load texture: " << IMG_GetError() << std::endl;
        }

        loaded_textures[img_name] = texture;
        
        intro_textures.push_back(texture);
    }
}

static void load_intro_image_to_buffer(SDL_Renderer* r, size_t index)
{
    if (index < intro_images.size())
    {
        SDL_RenderCopy(r, intro_textures[index], NULL, NULL);
    }
}

static void load_actor_imgs(SDL_Renderer* renderer, std::string initial_scene)
{
    rapidjson::Document document;
    // check if file exists.
    std::filesystem::path scene_path = "resources/scenes/" + initial_scene + ".scene";
    if (std::filesystem::exists(scene_path))
    {
        ConfigManager::read_json_file(scene_path.string(), document);
        if (document.HasMember("actors") && document["actors"].IsArray()) 
        {
            const rapidjson::Value& actors = document["actors"];
            for (rapidjson::SizeType i = 0; i < actors.Size(); i++)
            {
                const rapidjson::Value& actor = actors[i];
                if (actor.HasMember("view_image") && actor["view_image"].IsString() 
                    && (loaded_textures.find(actor["view_image"].GetString()) == loaded_textures.end()))
                {
                    std::string img = actor["view_image"].GetString();
                    std::string img_png = img + ".png";
                    auto full_path = "resources/images/" + img_png;
                    if (!std::filesystem::exists(full_path))
                    {
                        std::cout << "error: missing image " << img_png;
                        exit(0);
                    }
                    SDL_Texture* texture = IMG_LoadTexture(renderer, full_path.c_str());
                    if (!texture)
                    {
                        std::cout << "Failed to load texture: " << IMG_GetError() << std::endl;
                    }
                    loaded_textures[img] = texture;
                }
                // check to see if the actor also has a back.
                if (actor.HasMember("view_image_back")
                    && (loaded_textures.find(actor["view_image_back"].GetString()) == loaded_textures.end()))
                {
                    std::string img = actor["view_image_back"].GetString();
                    std::string img_png = img + ".png";
                    auto full_path = "resources/images/" + img_png;
                    if (!std::filesystem::exists(full_path))
                    {
                        std::cout << "error: missing image " << img_png;
                        exit(0);
                    }
                    SDL_Texture* texture = IMG_LoadTexture(renderer, full_path.c_str());
                    if (!texture)
                    {
                        std::cout << "Failed to load texture: " << IMG_GetError() << std::endl;
                    }
                    loaded_textures[img] = texture;
                }
                if (actor.HasMember("view_image_damage")
                    && (loaded_textures.find(actor["view_image_damage"].GetString()) == loaded_textures.end()))
                {
                    std::string img = actor["view_image_damage"].GetString();
                    std::string img_png = img + ".png";
                    auto full_path = "resources/images/" + img_png;
                    if (!std::filesystem::exists(full_path))
                    {
                        std::cout << "error: missing image " << img_png;
                        exit(0);
                    }
                    SDL_Texture* texture = IMG_LoadTexture(renderer, full_path.c_str());
                    if (!texture)
                    {
                        std::cout << "Failed to load texture: " << IMG_GetError() << std::endl;
                    }
                    loaded_textures[img] = texture;
                }
                if (actor.HasMember("view_image_attack")
                    && (loaded_textures.find(actor["view_image_attack"].GetString()) == loaded_textures.end()))
                {
                    std::string img = actor["view_image_attack"].GetString();
                    std::string img_png = img + ".png";
                    auto full_path = "resources/images/" + img_png;
                    if (!std::filesystem::exists(full_path))
                    {
                        std::cout << "error: missing image " << img_png;
                        exit(0);
                    }
                    SDL_Texture* texture = IMG_LoadTexture(renderer, full_path.c_str());
                    if (!texture)
                    {
                        std::cout << "Failed to load texture: " << IMG_GetError() << std::endl;
                    }
                    loaded_textures[img] = texture;
                }
                // check to see if actor is a template.
                // if (actor.HasMember("template") 
                //     && (loaded_textures.find(actor["template"].GetString()) == loaded_textures.end()))
                // {
                //     std::string template_name = actor["template"].GetString();
                //     TemplateManager::check_if_template_exists(template_name);
                //     // now if we did not error out, this means the template exists.
                //     // we need to load in this template image into out loaded textures.
                //     std::string img = template_name;
                //     std::string img_png = img + ".png";
                //     auto full_path = "resources/images/" + img_png;
                //     if (std::filesystem::exists(full_path))
                //     {
                //         SDL_Texture* texture = IMG_LoadTexture(renderer, full_path.c_str());
                //         if (!texture)
                //         {
                //             std::cout << "Failed to load texture: " << IMG_GetError() << std::endl;
                //         }
                //         loaded_textures[img] = texture;
                //     }
                // }
            }
        }
    }

    

}

static void load_bad_ending_img(SDL_Renderer* r)
{
    rapidjson::Document document;
	ConfigManager::read_json_file("resources/game.config", document);
    if (document.HasMember("game_over_bad_image") && document["game_over_bad_image"].IsString())
    {
        std::string img = document["game_over_bad_image"].GetString();
        std::string img_png = img + ".png";
        auto full_path = "resources/images/" + img_png;
        if (!std::filesystem::exists(full_path))
        {
            std::cout << "error: missing image " << img_png;
            exit(0);
        }
        SDL_Texture* texture = IMG_LoadTexture(r, full_path.c_str());
        if (!texture)
        {
            std::cout << "Failed to load texture: " << IMG_GetError() << std::endl;
        }
        loaded_textures[img] = texture;
        bad_ending_img_name = img;
    }
}

static void load_good_ending_img(SDL_Renderer* r)
{
    rapidjson::Document document;
    ConfigManager::read_json_file("resources/game.config", document);
    if (document.HasMember("game_over_good_image") && document["game_over_good_image"].IsString())
    {
        std::string img = document["game_over_good_image"].GetString();
        std::string img_png = img + ".png";
        auto full_path = "resources/images/" + img_png;
        if (!std::filesystem::exists(full_path))
        {
            std::cout << "error: missing image " << img_png;
            exit(0);
        }
        SDL_Texture* texture = IMG_LoadTexture(r, full_path.c_str());
        if (!texture)
        {
            std::cout << "Failed to load texture: " << IMG_GetError() << std::endl;
        }
        loaded_textures[img] = texture;
        good_ending_img_name = img;
    }

}

static SDL_Texture* get_texture(const std::string& texture_name)
{
    if (loaded_textures.find(texture_name) == loaded_textures.end())
    {
        std::cout << "error: missing texture " << texture_name;
        exit(0);
    }
    return loaded_textures[texture_name];
}

static SDL_Texture* LoadTexture(SDL_Renderer* r, const std::string& texture_name)
{
    std::filesystem::path scene_path = "resources/images/" + texture_name + ".png";
    if (!std::filesystem::exists(scene_path))
    {
        std::cout << "error: missing image " << texture_name;
        exit(0);
    }
    // see if this image is already cached.
    if (loaded_textures.find(texture_name) != loaded_textures.end())
    {
        return loaded_textures[texture_name];
    }
    // if not we are going to cache it then return it.
    SDL_Texture* texture = IMG_LoadTexture(r, scene_path.string().c_str());
    if (!texture)
    {
        std::cout << "Failed to load texture: " << IMG_GetError() << std::endl;
    }
    loaded_textures[texture_name] = texture;
    return texture;
}

static void DrawUI(const std::string& image_name, float x, float y)
{
    UIRenderRequest request(image_name, static_cast<int>(x), static_cast<int>(y));
    // ui_render_requests.push(request);
    ui_render_requests.push_back(request);
}

static void DrawUIEx(const std::string& image_name, float x, float y, float r, float g, float b, float a, int sorting_order)
{
    UIRenderRequest request(image_name, static_cast<int>(x), static_cast<int>(y));
    request.color = {static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b), static_cast<Uint8>(a)};
    request.sorting_order = sorting_order;
    // ui_render_requests.push(request);
    ui_render_requests.push_back(request);
}

static void RenderUIRequest(SDL_Renderer* r)
{
    // sort the ui_render_requests by sorting_order
    std::stable_sort(ui_render_requests.begin(), ui_render_requests.end(), 
        [](const UIRenderRequest& a, const UIRenderRequest& b) -> bool
        {
            return a.sorting_order < b.sorting_order;
        });
    for (const auto& request : ui_render_requests)
    {
        // load the texture
        SDL_Texture* texture = LoadTexture(r, request.image_name);
        // apply color and alpha modifications.
        SDL_SetTextureColorMod(texture, request.color.r, request.color.g, request.color.b);
        SDL_SetTextureAlphaMod(texture, request.color.a);

        SDL_Rect dest;
        SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);
        dest.x = request.x;
        dest.y = request.y;

        SDL_RenderCopy(r, texture, NULL, &dest);

        // reset the color and alpha modifications.
        SDL_SetTextureColorMod(texture, 255, 255, 255);
        SDL_SetTextureAlphaMod(texture, 255);
    }
    ui_render_requests.clear();
}

static void Draw(const std::string& name, float x, float y)
{
    ImageRenderRequest request(name, x, y);
    image_render_requests.push_back(request);
}

static void RenderImageRequests(SDL_Renderer* r, WindowRenderer* window, int x_res, int y_res)
{
    // SDL_Texture* target_texture = nullptr;
    // if (EDITOR_MODE)
    // {
    //     // create a texture to render to 
    //     target_texture = SDL_CreateTexture(r, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, window->get_view_width(), window->get_view_height());
    //     //set the new texture as the render target.
    //     SDL_SetRenderTarget(r, target_texture);
    // }
    const int pixel_per_meter = 100;
    // sort the image_render_requests by sorting_order
    std::stable_sort(image_render_requests.begin(), image_render_requests.end(), 
        [](const ImageRenderRequest& a, const ImageRenderRequest& b) -> bool
        {
            return a.sorting_order < b.sorting_order;
        });
    for (const auto& request : image_render_requests)
    {
        glm::vec2 camera_position = {Camera::GetPositionX(), Camera::GetPositionY()};
        glm::vec2 final_rendering_position = glm::vec2(request.x, request.y) - camera_position;

        SDL_Texture* texture = LoadTexture(r, request.name);
        SDL_Rect dest;
        SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);

        // apply scale
        int flip_mode = SDL_FLIP_NONE;
        if (request.scale_x < 0)
        {
            flip_mode |= SDL_FLIP_HORIZONTAL;
        }
        if (request.scale_y < 0)
        {
            flip_mode |= SDL_FLIP_VERTICAL;
        }

        float x_scale = std::abs(request.scale_x);
        float y_scale = std::abs(request.scale_y);

        dest.w *= x_scale;
        dest.h *= y_scale;

        SDL_Point pivot_point = { static_cast<int>(request.pivot_x * dest.w), static_cast<int>(request.pivot_y * dest.h) };

        // glm::ivec2 camera_dims = {window->get_view_width(), window->get_view_height()};
        glm::ivec2 camera_dims = {x_res, y_res};

        dest.x = static_cast<int>(final_rendering_position.x * pixel_per_meter + camera_dims.x * 0.5f * (1.0f / Camera::GetZoom()) - pivot_point.x);
        dest.y = static_cast<int>(final_rendering_position.y * pixel_per_meter + camera_dims.y * 0.5f * (1.0f / Camera::GetZoom()) - pivot_point.y);

        SDL_SetTextureColorMod(texture, request.color.r, request.color.g, request.color.b);
        SDL_SetTextureAlphaMod(texture, request.color.a);

        Helper::SDL_RenderCopyEx498(0, "", r, texture, NULL, &dest, request.rotation, &pivot_point, static_cast<SDL_RendererFlip>(flip_mode));

        SDL_RenderSetScale(r, Camera::GetZoom(), Camera::GetZoom());

        // reset the color and alpha modifications.
        SDL_SetTextureColorMod(texture, 255, 255, 255);
        SDL_SetTextureAlphaMod(texture, 255);
    }

    SDL_RenderSetScale(r, 1, 1);

    image_render_requests.clear();
}

static void DrawEx(const std::string &name, float x, float y, float rotation, float scale_x, float scale_y, float pivot_x, float pivot_y, float r, float g, float b, float a, int sorting_order)
{
    ImageRenderRequest request(name, x, y, {static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b), static_cast<Uint8>(a)}, sorting_order, static_cast<int>(rotation), scale_x, scale_y, pivot_x, pivot_y);
    image_render_requests.push_back(request);
}

static void DrawPixel(float x, float y, float r, float g, float b, float a)
{
    DrawPixelRequest request(static_cast<int>(x), static_cast<int>(y), {static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b), static_cast<Uint8>(a)});
    draw_pixel_requests.push_back(request);
}

static void RenderPixelRequests(SDL_Renderer* r, WindowRenderer* window)
{
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    for (const auto& request : draw_pixel_requests)
    {
        SDL_SetRenderDrawColor(r, request.color.r, request.color.g, request.color.b, request.color.a);
        SDL_RenderDrawPoint(r, request.x, request.y);
    }
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
    draw_pixel_requests.clear();
}

static void clean_up()
{
    for (auto& [key, value] : loaded_textures)
    {
        SDL_DestroyTexture(value);
    }
    loaded_textures.clear();
    
}

};
#endif // IMAGE_MANAGER_H