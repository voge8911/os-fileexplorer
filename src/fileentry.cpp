#include <iostream>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "fileentry.h"

FileEntry::FileEntry()
{
    data.font = NULL;
    data.icon = NULL;
    data.text = NULL;
    data.info = NULL;
    data.permissions = NULL;
    _permissions = NULL;
    _file_size = 0;
    _indent = 0;
    isInitialized = false;
}
FileEntry::~FileEntry()
{
    delete[] img_surf;
}

void FileEntry::setIndent(int indent)
{
    _indent = indent;
}

// Sort files by file type and alphabetically
bool FileComparator::operator ()(const FileEntry *f1, const FileEntry *f2)
{
    // By File type
    if (f1->sort_order < f2->sort_order && f1->_indent == f2->_indent) 
    {
        return true;
    }
    // Alphabetically
    if (f1->sort_order == f2->sort_order && f1->_file_name < f2->_file_name && f1->_indent == f2->_indent) 
    {
        return true;
    }
    return false;
}

void FileEntry::setNameAndSize(std::string file_name, std::string file_path, struct stat file_info)
{
    _file_name = file_name;
    _full_path = file_path;
    if (!S_ISDIR(file_info.st_mode))
    {
        _file_size = file_info.st_size;
    }
    /*
    if (S_IRUSR & file_info.st_mode)
        strcpy(_permissions, "r");
    else
        strcpy(_permissions, "-");
    if (S_IWUSR & file_info.st_mode)
        strcpy(_permissions, "w");
    else
        strcpy(_permissions, "-");
    if (S_IXUSR & file_info.st_mode)
        strcpy(_permissions, "x");
    else
        strcpy(_permissions, "-");
        */
}

void FileEntry::initializeFile(SDL_Renderer *renderer, SDL_Surface *img_surf)
{
    // Set font
    data.font = TTF_OpenFont("resrc/OpenSans-Regular.ttf", 12);

    // Set icon
    data.icon = SDL_CreateTextureFromSurface(renderer, img_surf);
    SDL_FreeSurface(img_surf);

    // Set text and text color for file name
    SDL_Color color = { 255, 255, 255 };
    SDL_Surface *phrase_surf = TTF_RenderText_Blended(data.font, _file_name.c_str(), color);
    data.text = SDL_CreateTextureFromSurface(renderer, phrase_surf);
    SDL_FreeSurface(phrase_surf);

    // Set text and text color for file info
    if (_file_size != 0)
    {
        std::string file_size_string;
        
        if (_file_size >= 1073741824) // GB
        {
            _file_size = _file_size / 1073741824;
            file_size_string = std::to_string(_file_size) + " GiB";
        }
        else if (_file_size >= 1048576) // MB
        {
            _file_size = _file_size / 1048576;
            file_size_string = std::to_string(_file_size) + " MiB";
        }
        else if (_file_size >= 1024) // KB
        {
            _file_size = _file_size / 1024;
            file_size_string = std::to_string(_file_size) + " KiB";
        }
        else
        {
            file_size_string = std::to_string(_file_size) + " Bytes";
        }
        SDL_Surface *info_surf = TTF_RenderText_Blended(data.font, file_size_string.c_str(), color);
        data.info = SDL_CreateTextureFromSurface(renderer, info_surf);
        SDL_FreeSurface(info_surf);

        SDL_Surface *perm_surf = TTF_RenderText_Blended(data.font, _permissions, color);
        data.permissions = SDL_CreateTextureFromSurface(renderer, perm_surf);
        SDL_FreeSurface(perm_surf);
    }
    
    isInitialized = true;
}

void FileEntry::renderFile(SDL_Renderer *renderer, int x, int y)
{
    x = x + _indent;
    SDL_Rect icon_rect = {x     , y - 45, 165, 200};
    SDL_Rect text_rect = {x + 55, y - 30, 165, 200};
    // Render icon
    SDL_QueryTexture(data.icon, NULL, NULL, &(icon_rect.w), &(icon_rect.h));
    SDL_RenderCopy(renderer, data.icon, NULL, &icon_rect);
    //printf("\nicon x=%d y=%d w=%d h=%d\n", icon_rect.x, icon_rect.y, icon_rect.w, icon_rect.h);
    
    // Render text
    SDL_QueryTexture(data.text, NULL, NULL, &(text_rect.w), &(text_rect.h));
    SDL_RenderCopy(renderer, data.text, NULL, &text_rect);

    //printf("text x=%d y=%d w=%d h=%d\n", text_rect.x, text_rect.y, text_rect.w, text_rect.h);
    // Set file position
    x_position = x;
    y_position = y - 45;
    w_position = (text_rect.x + text_rect.w) - icon_rect.x;
    h_position = icon_rect.h;

    if (_file_size != 0)
    {   // Render text for file info
        SDL_Rect info_rect = {x + 55, y - 10, 165, 200};
        SDL_QueryTexture(data.info, NULL, NULL, &(info_rect.w), &(info_rect.h));
        SDL_RenderCopy(renderer, data.info, NULL, &info_rect);
        // Render text for file info
        SDL_Rect permissions_rect = {x + 75, y - 10, 165, 200};
        SDL_QueryTexture(data.permissions, NULL, NULL, &(permissions_rect.w), &(permissions_rect.h));
        SDL_RenderCopy(renderer, data.permissions, NULL, &permissions_rect);
    }
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

void FileEntry::quit()
{
    if (data.icon != NULL && data.text != NULL && data.font != NULL && data.info != NULL && data.permissions != NULL)
    {
        SDL_DestroyTexture(data.icon);
        SDL_DestroyTexture(data.text);
        SDL_DestroyTexture(data.info);
        SDL_DestroyTexture(data.permissions);
        TTF_CloseFont(data.font);
    }
}