#pragma once

#include <sys/stat.h>

#include <string>

class file_loader
{
public:
    explicit file_loader(std::string filepath);
    ~file_loader();

    std::size_t len() const;
    
    operator const char*()
        { return content; }
private:
    const char* content;
    struct stat s;
    int fd;
};
