#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <dirent.h>
#include <sys/stat.h>
#include <vector>
#include <algorithm>
#include "fileentry.h"

#define WIDTH 900
#define HEIGHT 600

typedef struct AppData
{
    TTF_Font *font;
    SDL_Texture *penguin;
    SDL_Texture *phrase;
    SDL_Rect penguin_location;
    SDL_Rect phrase_location;
    bool doc_selected;
    bool phrase_selected;
} AppData;


void init(SDL_Renderer *renderer, AppData *data_ptr);
void render(SDL_Renderer *renderer, AppData *data_ptr);
void quit(AppData *data_ptr);
void listDirectory(std::string dirname, int indent, std::vector<FileEntry *> &files);

int main(int argc, char **argv)
{
    char *home = getenv("HOME");
    printf("HOME: %s\n", home);
    std::vector<FileEntry *> files;
    listDirectory(home, 0, files);

    printf("# of files = %ld\n", files.size());

    // initializing SDL as Video
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    // create window and renderer
    SDL_Renderer *renderer;
    SDL_Window *window;
    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer);

    // initialize and perform rendering loop
    //AppData data;
    //init(renderer, &data);
    //render(renderer, &data);

    // erase renderer content
    SDL_SetRenderDrawColor(renderer, 235, 235, 235, 255);
    SDL_RenderClear(renderer);

    int i, x, y;
    x = 10;
    y = 10;
    for (i = 0; i < files.size(); i++)
    {
        files[i]->initializeFile(renderer);
        files[i]->renderFile(renderer, x, y);
        y += 20;
        if (y >= HEIGHT)
        {
            x += 25;
        }
    }

    // show rendered frame
    SDL_RenderPresent(renderer);
    
    SDL_Event event;
    SDL_WaitEvent(&event);
    while (event.type != SDL_QUIT)
    {
        //render(renderer);
        SDL_WaitEvent(&event);
        switch (event.type)
        {
        case SDL_MOUSEMOTION:
            std::cout << "mouse: " << event.motion.x << " " << event.motion.y << std::endl;
            break;
        case SDL_MOUSEBUTTONDOWN:
            std::cout << "Button Down" << std::endl;
        case SDL_MOUSEBUTTONUP:
            std::cout << "Button Up" << std::endl;
        default:
            break;
        }
    }

    // clean up
    // SDL_DestroyTexture(data.penguin);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    TTF_Quit();
    for (i = 0; i < files.size(); i++)
    {
        files[i]->quit();
    }

    return 0;
}

void init(SDL_Renderer *renderer, AppData *data_ptr)
{
    data_ptr->font = TTF_OpenFont("resrc/OpenSans-Regular.ttf", 24);

    SDL_Surface *img_surf = IMG_Load("resrc/images/document.png");
    data_ptr->penguin = SDL_CreateTextureFromSurface(renderer, img_surf);
    SDL_FreeSurface(img_surf);
  

    SDL_Color color = { 0, 0, 0 };
    SDL_Surface *phrase_surf = TTF_RenderText_Solid(data_ptr->font, "Hello World!", color);
    data_ptr->phrase = SDL_CreateTextureFromSurface(renderer, phrase_surf);
    SDL_FreeSurface(phrase_surf);

    data_ptr->phrase_selected = false;
    data_ptr->doc_selected = false;
}

void render(SDL_Renderer *renderer, AppData *data_ptr)
{
    // erase renderer content
    SDL_SetRenderDrawColor(renderer, 235, 235, 235, 255);
    SDL_RenderClear(renderer);
    
    // TODO: draw!
    SDL_Rect rect;
    rect.x = 200;
    rect.y = 100;
    rect.w = 165;
    rect.h = 200;
    SDL_RenderCopy(renderer, data_ptr->penguin, NULL, &rect);

    rect.x = 400;
    rect.y = 300;
    SDL_RenderCopy(renderer, data_ptr->penguin, NULL, &rect);

    SDL_QueryTexture(data_ptr->phrase, NULL, NULL, &(rect.w), &(rect.h));
    rect.x = 10;
    rect.y = 500;
    SDL_RenderCopy(renderer, data_ptr->phrase, NULL, &rect);

    // show rendered frame
    SDL_RenderPresent(renderer);
}

void quit(AppData *data_ptr)
{
    SDL_DestroyTexture(data_ptr->penguin);
    SDL_DestroyTexture(data_ptr->phrase);
    TTF_CloseFont(data_ptr->font);
}

// Print all entries in directory in alphabetical order
// In addition to file name, also print file size (or 'directory' if entry is a folder)
void listDirectory(std::string dirname, int indent, std::vector<FileEntry *> &files)
{
    int i;
    std::string space = "";
    for (i = 0; i < indent; i++)
    {
        space += " ";
    }

    struct stat info;
    int err = stat(dirname.c_str(), &info);
    if (err == 0 && S_ISDIR(info.st_mode))
    {   
        std::vector<std::string> list;
        DIR* dir = opendir(dirname.c_str());

        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            list.push_back(entry->d_name);
        }
        closedir(dir);

        std::sort(list.begin(), list.end());

        int i, file_err;
        struct stat file_info;
        for (i = 0; i < list.size(); i++)
        {
            std::string full_path = dirname + "/" + list[i];
            file_err = stat(full_path.c_str(), &file_info);
            if (file_err)
            {
                fprintf(stderr, "UH OH, Shouldn't be here\n");
            }
            else if (S_ISDIR(file_info.st_mode))
            {
                //printf("%s%s (directory)\n", space.c_str(), list[i].c_str());
                if(list[i] != "." && list[i] != "..")
                {
                    // list sub-directories
                    // Add button feature to toggle this on and off
                    //listDirectory(full_path, indent + 2);
                }
                FileEntry *file = new FileEntry(list[i].c_str());
                files.push_back(file);
            }
            else
            {
                //printf("%s%s (%ld bytes)\n", space.c_str(), list[i].c_str(), file_info.st_size);
                FileEntry *file = new FileEntry(list[i].c_str());
                files.push_back(file);
            }
        }
    }
    else
    {
        fprintf(stderr, "Error: directory '%s' not found\n", dirname.c_str());
    }
}
