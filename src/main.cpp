#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <vector>
#include <algorithm>
#include "fileentry.h"

#define WIDTH 900
#define HEIGHT 600

void renderFiles(SDL_Renderer *renderer, std::vector<FileEntry *> &files, int x_offset, int y_offset, int scroll_number);
void listDirectory(std::string dirname, int indent, std::vector<FileEntry *> &files);

int main(int argc, char **argv)
{
    char *home = getenv("HOME");
    printf("HOME: %s\n", home);
    std::vector<FileEntry *> files;

    listDirectory(home, 0, files);
    std::sort(files.begin(), files.end(), FileComparator());
    
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

    // render files
    int scroll_number = 0;
    renderFiles(renderer, files, 0, 0, scroll_number);

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
            //std::cout << "mouse: x=" << event.motion.x << " y=" << event.motion.y << std::endl;
            break;
        case SDL_MOUSEBUTTONDOWN:
            std::cout << "Button Down" << std::endl;
            break;
        case SDL_MOUSEBUTTONUP:
            std::cout << "Button Up" << std::endl;
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                int index = -1;
                int i;
                // Loop through files currently display on the window, and find if a file was clicked
                for (i = scroll_number; i < files.size() + 12; i++)
                {
                    if(event.button.x >= files[i]->x_position &&
                       event.button.x <= files[i]->x_position + files[i]->w_position && 
                       event.button.y >= files[i]->y_position &&
                       event.button.y <= files[i]->y_position + files[i]->h_position)
                    {
                        std::cout << "index = " << i << std::endl;
                        index = i;
                        break;
                    }
                }
                // If a file was clicked
                if (index != -1)
                {   // If the file clicked is a directory, open that directory
                    if (files[index]->sort_order == 0)
                    {
                        scroll_number = 0; // reset scroll number
                        std::string dir_name = files[index]->_full_path;
                        std::cout << dir_name << std::endl;

                        files.erase(files.begin(), files.end());

                        listDirectory(dir_name, 0, files);
                        std::sort(files.begin(), files.end(), FileComparator());

                        // render files
                        renderFiles(renderer, files, 0, 0, 0);
                        // show rendered frame
                        SDL_RenderPresent(renderer);
                    }
                    // If the file clicked is a file, open that file with its preferred application  
                    else if (files[index]->sort_order == 2 || files[index]->sort_order == 3 || 
                             files[index]->sort_order == 4 || files[index]->sort_order == 5)
                    {
                        int pid = fork();
                        if (pid == 0) //child process
                        {    
                            execlp("/usr/bin/xdg-open", "xdg-open", files[index]->_full_path.c_str(), NULL);
                            exit(1);
                        }
                        else // parent process
                        {
                        } 
                    }
                }
            }
            break;
        case SDL_MOUSEWHEEL:
            if (event.wheel.y > 0) // scroll up
            {
                if (scroll_number > 0)
                {
                    scroll_number--;
                    std::cout << scroll_number << std::endl;

                    // render files
                    renderFiles(renderer, files, 0, 0, scroll_number);
                    // show rendered frame
                    SDL_RenderPresent(renderer);
                }
            }
            else if (event.wheel.y < 0) // scroll down
            {
                int size;
                if (files.size() <= scroll_number + 13)
                    size = files.size();
                else
                    size = scroll_number + 13;

                std::cout << files[size - 1]->y_position << std::endl;
                if (files[size - 1]->y_position >= HEIGHT)  // if a files postition is below the window
                {
                    // Scroll down
                    scroll_number++;
                    std::cout << scroll_number << std::endl;

                    // render files
                    renderFiles(renderer, files, 0, 0, scroll_number);
                    // show rendered frame
                    SDL_RenderPresent(renderer);
                }
                else 
                    break;
            }
        default:
            break;
        }
    }

    // clean up
    int i;
    for (i = 0; i < files.size(); i++)
    {
        files[i]->quit();
    }
    files.clear();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    TTF_Quit();
    
    return 0;
}

void renderFiles(SDL_Renderer *renderer, std::vector<FileEntry *> &files, int x_offset, int y_offset, int scroll_number)
{
    // erase renderer content
    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 0);
    SDL_RenderClear(renderer);

    // draw and render window
    SDL_SetRenderDrawColor(renderer, 35, 35, 35, 0);
    SDL_Rect rect = {200, 0, 700, 600};
    SDL_RenderFillRect(renderer, &rect);

    int j, x, y, end_loop;
    x = 220 + x_offset;
    y = 45 + y_offset;
    // Loop through Files and render them
    if (files.size() <= scroll_number + 13)
        end_loop = files.size();
    else
        end_loop = scroll_number + 13;

    for (j = scroll_number; j < end_loop; j++)
    {   
        // Initialize and Render File
        if (!files[j]->isInitialized) // If file is already initialized don't initialize again
        {
            files[j]->initializeFile(renderer, files[j]->img_surf);
        }
        files[j]->renderFile(renderer, x, y);
        y += 50;
    }
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
        for (i = 0; i <  list.size(); i++)
        {
            std::string full_path = dirname + "/" + list[i];
            std::string name = list[i];
            std::string extension = name.substr(name.find_last_of(".") + 1);

            file_err = stat(full_path.c_str(), &file_info);
            if (file_err)
            {
                fprintf(stderr, "UH OH, Shouldn't be here\n");
            }
            // Assign file type
            else if (list[i] != ".")
            {   
                // Directory
                if (S_ISDIR(file_info.st_mode))
                {
                    if (list[i] != "..")
                    {
                        // list sub-directories
                        // Add button feature to toggle this on and off
                        //listDirectory(full_path, indent + 2, files);
                    }
                    Directory *dir = new Directory;
                    dir->setNameAndSize(list[i].c_str(), full_path, file_info);
                    files.push_back(dir);
                }
                // File as excecute permissions
                else if (S_IXUSR & file_info.st_mode)
                {
                    Excecutable *exec = new Excecutable;
                    exec->setNameAndSize(list[i].c_str(), full_path, file_info);
                    files.push_back(exec);
                }
                // Image
                else if (extension == "jpg" || extension == "jpeg" || extension == "png" ||
                         extension == "tif" || extension == "tiff" || extension == "gif")
                {
                    Image *image = new Image;
                    image->setNameAndSize(list[i].c_str(), full_path, file_info);
                    files.push_back(image);
                }
                // Video
                else if (extension == "mp4" || extension == "mov" || extension == "mkv" ||
                         extension == "avi" || extension == "webm") 
                {
                    Video *vid = new Video;
                    vid->setNameAndSize(list[i].c_str(), full_path, file_info);
                    files.push_back(vid);
                }
                // Code file
                else if (extension == "h"  || extension == "c"    || extension == "cpp" ||
                         extension == "py" || extension == "java" || extension == "js"  )  
                {
                    CodeFile *code = new CodeFile;
                    code->setNameAndSize(list[i].c_str(), full_path, file_info);
                    files.push_back(code);
                }
                else
                {
                    //printf("%s%s (%ld bytes)\n", space.c_str(), list[i].c_str(), file_info.st_size);
                    Other *file = new Other;
                    file->setNameAndSize(list[i].c_str(), full_path, file_info);
                    files.push_back(file);
                }
            }
        }
    }
    else
    {
        fprintf(stderr, "Error: directory '%s' not found\n", dirname.c_str());
    }
}
