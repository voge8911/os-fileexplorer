#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "fileentry.h"

FileEntry::FileEntry()
{
}
FileEntry::~FileEntry()
{
}

void FileEntry::setName(std::string file_name)
{
    _file_name = file_name;
}

void FileEntry::initializeFile(SDL_Renderer *renderer, SDL_Surface *img_surf)
{
    // Set font
    data.font = TTF_OpenFont("resrc/OpenSans-Regular.ttf", 12);

    // Set icon
    data.icon = SDL_CreateTextureFromSurface(renderer, img_surf);
    SDL_FreeSurface(img_surf);

    // Set text and text color
    SDL_Color color = { 255, 255, 255 };
    SDL_Surface *phrase_surf = TTF_RenderText_Solid(data.font, _file_name.c_str(), color);
    data.text = SDL_CreateTextureFromSurface(renderer, phrase_surf);
    SDL_FreeSurface(phrase_surf);

    data.text_selected = false;
    data.icon_selected = false;
}

void FileEntry::renderFile(SDL_Renderer *renderer, int x, int y)
{
    SDL_Rect rect;
    rect.x = x;
    printf("x=%d, y=%d\n", x, y);
    rect.y = y - 45;
    rect.w = 165;
    rect.h = 200;
    SDL_QueryTexture(data.icon, NULL, NULL, &(rect.w), &(rect.h));
    SDL_RenderCopy(renderer, data.icon, NULL, &rect);

    rect.x = x;
    rect.y = y;
    rect.w = 165;
    rect.h = 200;

    SDL_QueryTexture(data.text, NULL, NULL, &(rect.w), &(rect.h));
    SDL_RenderCopy(renderer, data.text, NULL, &rect);

}

void FileEntry::quit()
{
    SDL_DestroyTexture(data.icon);
    SDL_DestroyTexture(data.text);
    TTF_CloseFont(data.font);
}

// Sort files by file type and alphabetically
bool FileComparator::operator ()(const FileEntry *f1, const FileEntry *f2)
{
    // By File type
    if (f1->sort_order < f2->sort_order) 
    {
        return true;
    }
    // Alphabetically
    if (f1->sort_order == f2->sort_order && f1->_file_name < f2->_file_name) 
    {
        return true;
    }
    return false;
}

Directory::Directory()
{
    img_surf = IMG_Load("resrc/images/directory.png");
    sort_order = 0;
}

Excecutable::Excecutable()
{
    img_surf = IMG_Load("resrc/images/exe.png");
    sort_order = 1;
}

Image::Image()
{
    img_surf = IMG_Load("resrc/images/image.png");
    sort_order = 2;
}

Video::Video()
{
    img_surf = IMG_Load("resrc/images/video.png");
    sort_order = 3;
}

CodeFile::CodeFile()
{
    img_surf = IMG_Load("resrc/images/code.png");
    sort_order = 4;
}

Other::Other()
{
    img_surf = IMG_Load("resrc/images/other.png");
    sort_order = 5;
}