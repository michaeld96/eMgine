#ifndef TEXT_MANAGER_H
#define TEXT_MANAGER_H  
#include "Common.h"
#include "ConfigManager.h"
#include "WindowRenderer.h"
#include <SDL2/include/SDL_ttf.h>

class TextRenderRequest 
{
public:
    std::string text;
    std::string font;
    SDL_Color color;
    int size;
    int x;
    int y;

    TextRenderRequest(const std::string& text, const std::string& font, SDL_Color color, int size, int x, int y)
    {
        this->text = text;
        this->font = font;
        this->color = color;
        this->size = size;
        this->x = x;
        this->y = y;
    }
};


class TextManager
{
public:
static inline std::vector<std::string> intro_texts;
static inline TTF_Font* font;
static inline size_t intro_text_index = 0;
static inline std::unordered_map<std::string, std::unordered_map<int, TTF_Font*>> cached_fonts;
static inline std::queue<TextRenderRequest> text_render_requests;
static void load_text()
{

    TTF_Init();

    // rapidjson::Document document;
    // ConfigManager::read_json_file("resources/game.config", document);

    // if (!document.HasMember("font") && document.HasMember("intro_text"))
    // {
    //     std::cout << "error: text render failed. No font configured";
    //     exit(0);
    // }
    
    // if (document.HasMember("intro_text") && document["intro_text"].IsArray()) 
    // {
    //     intro_texts.reserve(document["intro_text"].Size());
    //     const rapidjson::Value& intro_text = document["intro_text"];
    //     for (rapidjson::SizeType i = 0; i < intro_text.Size(); i++)
    //     {
    //         intro_texts.push_back(intro_text[i].GetString());
    //     }
    //     std::string font_path = "resources/fonts/" + std::string(document["font"].GetString()) + ".ttf";
    //     font = TTF_OpenFont(font_path.c_str(), 16);

    //     if (!font)
    //     {
    //         // error: font <font_name> missing
    //         std::cout << "error: font " << document["font"].GetString() << " missing";
    //         exit(0);
    //     }
    // }

    // if (document.HasMember("font") && document["font"].IsString())
    // {
    //     std::string font_path = "resources/fonts/" + std::string(document["font"].GetString()) + ".ttf";
    //     font = TTF_OpenFont(font_path.c_str(), 16);

    //     if (!font)
    //     {
    //         // error: font <font_name> missing
    //         std::cout << "error: font " << document["font"].GetString() << " missing";
    //         exit(0);
    //     }
    // }
}

static void load_intro_text_to_buffer(SDL_Renderer* r, WindowRenderer* window, size_t index)
{
    if (index < intro_texts.size())
    {
        SDL_Color color = {255, 255, 255, 255};
        SDL_Surface* surface = TTF_RenderText_Solid(font, intro_texts[index].c_str(), color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(r, surface);

        SDL_Rect dst;
        SDL_QueryTexture(texture, NULL, NULL, &dst.w, &dst.h);
        dst.x = 25;
        dst.y = window->get_view_height() - 50;
        SDL_RenderCopy(r, texture, NULL, &dst);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }
}

static void load_text_to_buffer(SDL_Renderer* r, WindowRenderer* window, const std::string& text, int x, int y)
{
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(r, surface);

    SDL_Rect dst;
    SDL_QueryTexture(texture, NULL, NULL, &dst.w, &dst.h);
    dst.x = x;
    dst.y = y;
    SDL_RenderCopy(r, texture, NULL, &dst);
    // SDL_RenderCopyEx(r, texture, NULL, &dst, 0, NULL, SDL_FLIP_NONE);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

static void render_dialogue(SDL_Renderer* r, WindowRenderer* w, std::vector<std::string>& dialogues)
{
    int m = dialogues.size();
    for (size_t i = 0; i < dialogues.size(); i++)
    {
        int y = w->get_view_height() - 50 - (m - 1 - i) * 50;
        load_text_to_buffer(r, w, dialogues[i], 25, y);
    }
}
// TODO: TEST!!
static TTF_Font* load_text(const std::string& font_name, int font_size)
{
    std::string filepath = "resources/fonts/" + font_name + ".ttf";
    // check if filepath exists.
    if (std::filesystem::exists(filepath))
    {
        // now let's check the font is cached.
        if (cached_fonts.find(font_name) == cached_fonts.end())
        {
            // need to load in the font.
            TTF_Font* font = TTF_OpenFont(filepath.c_str(), font_size);
            if (font)
            {
                cached_fonts[font_name][font_size] = font;
                return font;
            }
            else
            {
                // error: font <font_name> missing
                std::cout << "error: font " << font_name << " missing";
                exit(0);
            }
        }
        // if it is found, just return it.
        return cached_fonts[font_name][font_size];
    }
    return nullptr; // this is OK?
}
static void Lua_DrawText(const std::string& content, int x, int y, 
                    const std::string& fontName, 
                    int fontSize, int r, int g, int b, int a)
{
    // create SDL color
    SDL_Color local_color = { static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b), static_cast<Uint8>(a) };
    // create a text render request
    TextRenderRequest request(content, fontName, local_color, fontSize, x, y);
    // push the request to the queue
    TextManager::text_render_requests.push(request);
}

static void RenderTextRequest(SDL_Renderer* r, WindowRenderer* w)
{
    while(!TextManager::text_render_requests.empty())
    {
        TextRenderRequest& request = TextManager::text_render_requests.front();
        // load the fort
        TTF_Font* local_font = TextManager::load_text(request.font, request.size);
        // render the text as a texture
        // SDL_Color local_color = {255, 255, 255, 255};
        SDL_Surface* surface = TTF_RenderText_Solid(local_font, request.text.c_str(), request.color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(r, surface);

        // render the texture
        SDL_Rect dst;
        SDL_QueryTexture(texture, NULL, NULL, &dst.w, &dst.h);
        dst.x = request.x;
        dst.y = request.y;
        SDL_RenderCopy(r, texture, NULL, &dst);

        // free the surface and texture
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
        // pop the request from the queue
        TextManager::text_render_requests.pop();
    }
}

}; // end of TextManager class



#endif // TEXT_MANAGER_H