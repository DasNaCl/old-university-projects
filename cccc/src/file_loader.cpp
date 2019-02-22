#include <file_loader.hpp>

/***********************/
/* Some ugly c headers */
#include <stdlib.h>  /**/
#include <fcntl.h>   /**/
#include <stdio.h>   /**/
#include <unistd.h>  /**/
#include <sys/mman.h>/**/
/* For speed purposes! */
/***********************/

#include <stdexcept>
#include <cassert>

file_loader::file_loader(std::string path)
    : content(), s(), fd(open(path.c_str(), O_RDONLY))
{
    if(fd == -1)
        throw std::invalid_argument("Could not open the file \"" + path + "\"");

    int status = fstat(fd, &s);
    if(status < 0)
        throw std::invalid_argument("Could not retrieve the information from fstat");
    if(s.st_size == 0)
        return;

    content = (const char*)
        mmap(0, s.st_size, PROT_READ, MAP_SHARED | MAP_NORESERVE | MAP_POPULATE, fd, 0);
    if(content == MAP_FAILED)
    {
        close(fd);
        throw std::invalid_argument("Could not open the file \"" + path + "\" using mmap");
    }
}

file_loader::~file_loader()
{
    if(s.st_size > 0)
    {
#ifndef NDEBUG
        auto v = munmap((void*) content, s.st_size);
        assert(v == 0);
#else
        munmap((void*) content, s.st_size);
#endif
    }
    close(fd);
}

std::size_t file_loader::len() const
{
    return s.st_size;
}
