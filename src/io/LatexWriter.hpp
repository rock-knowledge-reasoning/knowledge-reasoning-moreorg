#include "../facades/Robot.hpp"

namespace moreorg {
namespace io {

/**
 * \brief Export structures in latex
 * \details Export robot descriptions and inferred robot properties into latex
 * tables
 */
class LatexWriter
{
public:
    /**
     * Convert robot facade into string
     * \return stringified robot facade
     */
    static std::string toString(const facades::Robot& robot);

    /**
     * Convert robot facades into string
     * \return stringified robot facades
     */
    static std::string toString(const std::vector<facades::Robot>& facades);
};

} // end namespace io
} // end namespace moreorg
