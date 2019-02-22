#include <lex/source_location.hpp>

#include <ostream>

namespace mhc4
{

source_location::source_location(const std::string& filepath,
                                 std::size_t line, std::size_t column)
    : filepath_(&filepath), line_(line), column_(column)
{  }

bool source_location::operator==(const source_location& src_loc) const noexcept
{
    return filepath_ == src_loc.filepath_
        && line_     == src_loc.line_
        && column_   == src_loc.column_;
}

bool source_location::operator!=(const source_location& src_loc) const noexcept
{
    return !(filepath_ == src_loc.filepath_
          && line_     == src_loc.line_
          && column_   == src_loc.column_);
}

const std::string& source_location::filepath() const
{
    return *filepath_;
}

std::size_t source_location::line() const
{
    return line_;
}

std::size_t source_location::column() const
{
    return column_;
}

std::ostream& operator<<(std::ostream& os, const source_location& loc)
{
    os << loc.filepath() << ":"
       << loc.line()     << ":"
       << loc.column()   << ":";
    return os;
}

}
