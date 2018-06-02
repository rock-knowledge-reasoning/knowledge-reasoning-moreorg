#include "../facades/Robot.hpp"

namespace organization_model {
namespace io {

class LatexWriter
{
public:
    static std::string toString(const facades::Robot& robot);
};

} // end namespace io
} // end namespace organization_model
