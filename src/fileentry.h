#ifndef __FILE_H_
#define __FILE_H_

#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

typedef struct FileData
{
    TTF_Font *font;
    SDL_Texture *icon;
    SDL_Texture *text;
    SDL_Texture *info;
    SDL_Texture *permissions;
    SDL_Rect icon_location;
    SDL_Rect text_location;

} FileData;

class FileEntry {
private:
    
    FileData data;
public:
    FileEntry();
    ~FileEntry();

    SDL_Surface *img_surf;
    std::string _file_name;
    std::string _full_path;
    std::string _permissions;
    bool isInitialized;
    
    int _file_size;
    int sort_order;
    int x_position, y_position, w_position, h_position;
    int _indent;

    void setIndent(int indent);
    void setNameAndSize(std::string file_name, std::string file_path);
    void initializeFile(SDL_Renderer *renderer, SDL_Surface *img_surf);
    void renderFile(SDL_Renderer *renderer, int x, int y);
    void quit();
};

class Directory: public FileEntry {
private:
    
public:
    Directory();
};

struct FileComparator {
    bool operator ()(const FileEntry *f1, const FileEntry *f2);
};

class Excecutable: public FileEntry {
private:
    
public:
    Excecutable();
};

class Image: public FileEntry {
private:
    
public:
    Image();
};

class Video: public FileEntry {
private:
    
public:
    Video();
};

class CodeFile: public FileEntry {
private:
    
public:
    CodeFile();
};

class Other: public FileEntry {
private:
    
public:
    Other();
};

#endif // __FILE_H_