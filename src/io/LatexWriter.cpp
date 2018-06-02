#include "LatexWriter.hpp"
#include <sstream>
#include <iomanip>
#include <iostream>

namespace organization_model {
namespace io {

std::string LatexWriter::toString(const facades::Robot& robot)
{
    std::stringstream ss;
    if(robot.getModelPool().empty())
    {
    } else {
        owlapi::model::IRI model = robot.getModelPool().begin()->first;

        ss << "\\begin{table}[h!t]" << std::endl;
        ss << "\\centering" << std::endl;
        ss << "\\caption{Properties of the agent type \\textit{" << model.getFragment() << "} }" << std::endl;
        ss << "\\label{tab:agent_type:" << model.getFragment() << "}" << std::endl;
        ss << "\\begin{tabular}{|l|l|}" << std::endl;
        ss << "\\hline" << std::endl;
        ss << "\\textbf{Property} & \\textbf{Value} \\\\ \\hline" << std::endl;
        std::string linesep = "\\\\ \\midrule";
        ss << "mass (kg):                     & " << std::fixed << std::setprecision(2) << robot.getMass()                    << linesep << std::endl;
        ss << "supply voltage (V):            & " << std::fixed << std::setprecision(2) << robot.getSupplyVoltage()           << linesep << std::endl;
        ss << "energy capacity (Wh):          & " << std::fixed << std::setprecision(2) << robot.getEnergyCapacity()          << linesep << std::endl;
        ss << "nominal power consumption (W): & " << std::fixed << std::setprecision(2) << robot.getNominalPowerConsumption() << linesep << std::endl;
        ss << "min accelleration (m/s):       & " << std::fixed << std::setprecision(2) << robot.getMinAcceleration()         << linesep << std::endl;
        ss << "max accelleration (m/s):       & " << std::fixed << std::setprecision(2) << robot.getMaxAcceleration()         << linesep << std::endl;
        ss << "nominal accelleration (m/s):   & " << std::fixed << std::setprecision(2) << robot.getNominalAcceleration()     << linesep << std::endl;
        ss << "min velocity (m/s):            & " << std::fixed << std::setprecision(2) << robot.getMinVelocity()             << linesep << std::endl;
        ss << "max velocity (m/s):            & " << std::fixed << std::setprecision(2) << robot.getMaxVelocity()             << linesep << std::endl;
        ss << "nominal velocity (m/s):        & " << std::fixed << std::setprecision(2) << robot.getNominalVelocity()         << linesep << std::endl;
        ss << "transport consumption (units): & " << std::fixed << std::setprecision(2) << robot.getTransportDemand()         << linesep << std::endl;
        ss << "transport capacity (units):    & " << std::fixed << std::setprecision(2) << robot.getTransportCapacity()       << linesep << std::endl;
        ss << "mobility                       & ";
        try {
            if(robot.isMobile())
            {
                ss << "mobile ";
            } else {
                ss << "$lnot$ mobile ";
            }
        } catch(const std::runtime_error& e)
        {
            ss << "n/a ";
        }
        ss << "\\\\ \\hline \\hline" << std::endl;
        ss << "\\end{tabular}" << std::endl;
        ss << "\\end{table}" << std::endl;
    }
    return ss.str();
}

} // end namespace io
} // end namespace organization_model
