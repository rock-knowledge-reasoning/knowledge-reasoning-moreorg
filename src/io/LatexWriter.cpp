#include "LatexWriter.hpp"
#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace moreorg {
namespace io {

std::string LatexWriter::toString(const facades::Robot& robot)
{
    std::stringstream ss;
    if(robot.getModelPool().empty())
    {
    } else
    {
        owlapi::model::IRI model = robot.getModelPool().begin()->first;

        ss << "\\begin{table}[h!t]" << std::endl;
        ss << "\\centering" << std::endl;
        ss << "\\caption{Properties of the agent type \\textit{"
           << model.getFragment() << "} }" << std::endl;
        ss << "\\label{tab:agent_type:" << model.getFragment() << "}"
           << std::endl;
        ss << "\\begin{tabular}{ll}" << std::endl;
        ss << "\\toprule" << std::endl;
        ss << "\\textbf{Property} & \\textbf{Value} \\\\ \\midrule"
           << std::endl;
        std::string linesep = "\\\\";
        ss << "mass (kg):                     & " << std::fixed
           << std::setprecision(2) << robot.getMass() << linesep << std::endl;
        ss << "supply voltage (V):            & " << std::fixed
           << std::setprecision(2) << robot.getSupplyVoltage() << linesep
           << std::endl;
        ss << "energy capacity (Wh):          & " << std::fixed
           << std::setprecision(2) << robot.getEnergyCapacity() << linesep
           << std::endl;
        ss << "nominal power consumption (W): & " << std::fixed
           << std::setprecision(2) << robot.getNominalPowerConsumption()
           << linesep << std::endl;
        ss << "min accelleration (m/s):       & " << std::fixed
           << std::setprecision(2) << robot.getMinAcceleration() << linesep
           << std::endl;
        ss << "max accelleration (m/s):       & " << std::fixed
           << std::setprecision(2) << robot.getMaxAcceleration() << linesep
           << std::endl;
        ss << "nominal accelleration (m/s):   & " << std::fixed
           << std::setprecision(2) << robot.getNominalAcceleration() << linesep
           << std::endl;
        ss << "min velocity (m/s):            & " << std::fixed
           << std::setprecision(2) << robot.getMinVelocity() << linesep
           << std::endl;
        ss << "max velocity (m/s):            & " << std::fixed
           << std::setprecision(2) << robot.getMaxVelocity() << linesep
           << std::endl;
        ss << "nominal velocity (m/s):        & " << std::fixed
           << std::setprecision(2) << robot.getNominalVelocity() << linesep
           << std::endl;
        ss << "transport consumption (units): & " << std::fixed
           << std::setprecision(2) << robot.getTransportDemand() << linesep
           << std::endl;
        ss << "transport capacity (units):    & " << std::fixed
           << std::setprecision(2) << robot.getTransportCapacity() << linesep
           << std::endl;
        ss << "mobility                       & ";
        try
        {
            if(robot.isMobile())
            {
                ss << "mobile ";
            } else
            {
                ss << "$\\lnot$ mobile ";
            }
        } catch(const std::runtime_error& e)
        {
            ss << "n/a ";
        }
        ss << "\\\\ \\bottomrule" << std::endl;
        ss << "\\end{tabular}" << std::endl;
        ss << "\\end{table}" << std::endl;
    }
    return ss.str();
}

std::string LatexWriter::toString(const std::vector<facades::Robot>& facades)
{
    std::map<std::string, std::string> rows;
    rows["titlerow"] = "\\textbf{Property}";
    rows["mass"] = "mass (kg)";
    rows["supply voltage"] = "supply voltage (V)";
    rows["energy capacity"] = "energy capacity (Wh)";
    rows["nominal power consumption"] = "nominal power consumption (W)";
    rows["min accelleration"] = "min accelleration (m/s)";
    rows["max accelleration"] = "max accelleration (m/s)";
    rows["nominal accelleration"] = "nominal accelleration (m/s)";
    rows["min velocity"] = "min velocity (m/s)";
    rows["max velocity"] = "min velocity (m/s)";
    rows["nominal velocity"] = "nominal velocity (m/s)";
    rows["transport consumption"] = "transport consumption (units)";
    rows["transport capacity"] = "transport capacity (units)";
    rows["mobility"] = "mobility";

    for(const facades::Robot& robot : facades)
    {
        typedef std::function<double()> DoublePropertyFunc;
        std::map<std::string, DoublePropertyFunc> dFunctions = {
            {"mass", std::bind(&facades::Robot::getMass, robot)},
            {"supply voltage",
             std::bind(&facades::Robot::getSupplyVoltage, robot)},
            {"energy capacity",
             std::bind(&facades::Robot::getEnergyCapacity, robot)},
            {"nominal power consumption",
             std::bind(&facades::Robot::getNominalPowerConsumption, robot)},
            {"min accelleration",
             std::bind(&facades::Robot::getMinAcceleration, robot)},
            {"max accelleration",
             std::bind(&facades::Robot::getMaxAcceleration, robot)},
            {"nominal accelleration",
             std::bind(&facades::Robot::getNominalAcceleration, robot)},
            {"min velocity", std::bind(&facades::Robot::getMinVelocity, robot)},
            {"max velocity", std::bind(&facades::Robot::getMaxVelocity, robot)},
            {"nominal velocity",
             std::bind(&facades::Robot::getNominalVelocity, robot)},
        };

        if(robot.getModelPool().empty())
        {
        } else
        {
            owlapi::model::IRI model = robot.getModelPool().begin()->first;
            rows["titlerow"] += " & \\textbf{" + model.getFragment() + "}";

            for(std::pair<std::string, DoublePropertyFunc> dFunc : dFunctions)
            {
                std::stringstream value;
                value << std::fixed << std::setprecision(2) << dFunc.second();
                rows[dFunc.first] += " & " + value.str();
            }

            {
                std::stringstream value;
                value << std::fixed << std::setprecision(2)
                      << robot.getTransportDemand();
                rows["transport consumption"] += " & " + value.str();
            }
            {
                std::stringstream value;
                value << std::fixed << std::setprecision(2)
                      << robot.getTransportCapacity();
                rows["transport capacity"] += " & " + value.str();
            }
            {
                std::stringstream value;
                try
                {
                    if(robot.isMobile())
                    {
                        value << "mobile ";
                    } else
                    {
                        value << "$\\lnot$ mobile ";
                    }
                } catch(const std::runtime_error& e)
                {
                    value << "n/a ";
                }
                rows["mobility"] += " & " + value.str();
            }
        }
    }

    std::stringstream ss;
    ss << "\n\\begin{table}[h!t]" << std::endl;
    ss << "\\centering" << std::endl;
    ss << "\\caption{Properties of agent types}" << std::endl;
    ss << "\\label{tab:appendix:agent_types}" << std::endl;
    std::string columns(facades.size(), 'l');
    ss << "\\begin{tabular}{l" + columns + "}" << std::endl;
    ss << "\\toprule" << std::endl;
    std::string linesep = "\\\\";
    ss << rows["titlerow"] << linesep << "\\midrule" << std::endl;
    ss << rows["mass"] << linesep << std::endl;
    ss << rows["supply voltage"] << linesep << std::endl;
    ss << rows["energy capacity"] << linesep << std::endl;
    ss << rows["nominal power consumption"] << linesep << std::endl;
    ss << rows["min accelleration"] << linesep << std::endl;
    ss << rows["max accelleration"] << linesep << std::endl;
    ss << rows["nominal accelleration"] << linesep << std::endl;
    ss << rows["min velocity"] << linesep << std::endl;
    ss << rows["max velocity"] << linesep << std::endl;
    ss << rows["nominal velocity"] << linesep << std::endl;
    ss << rows["transport consumption"] << linesep << std::endl;
    ss << rows["transport capacity"] << linesep << std::endl;
    ss << rows["mobility"] << linesep << std::endl;
    ss << "\\bottomrule" << std::endl;
    ss << "\\end{tabular}" << std::endl;
    ss << "\\end{table}" << std::endl;

    return ss.str();
}

} // end namespace io
} // end namespace moreorg
