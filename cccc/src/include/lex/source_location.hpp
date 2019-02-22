#pragma once

#include <string>

namespace mhc4
{

class source_location
{
public:
    explicit source_location(const std::string& filepath, std::size_t line,
                             std::size_t column);
    bool operator==(const source_location& src_loc) const noexcept;
    bool operator!=(const source_location& src_loc) const noexcept;

    const std::string& filepath() const;
    std::size_t line() const;
    std::size_t column() const;
private:
    const std::string* filepath_;
    std::size_t line_;
    std::size_t column_;
};

std::ostream& operator<<(std::ostream& os, const source_location& loc);

}

namespace std
{
    template<>
    struct hash<mhc4::source_location>
    {
        std::size_t operator()(const mhc4::source_location& loc) const
        {
            return hash<std::string>()(loc.filepath())
                 ^ hash<std::size_t>()(loc.line())
                 ^ hash<std::size_t>()(loc.column());
        }
    };
}
