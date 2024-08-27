/****************************************************************************
 * Copyright (C) by StellarDX Astronomy                                     *
 *                                                                          *
 * This file is part of Detailed information generator.                     *
 *                                                                          *
 *   This program is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation; either version 2 of the License, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *
 *   This program is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU General Public License for more details.                           *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program; If not, see <https://www.gnu.org/licenses/>.  *
 ****************************************************************************/

#include "gbuffer_system.hxx"
#include "CSE/PlanSystem.h"

cse::ustringlist PopulationStars, PopulationPlanets, PopulationDwarfPlanets;
cse::uint64 NSatellites = 0, NMinorPlanets = 0, NComets = 0;
fmt::dynamic_format_arg_store<fmt::format_context> TotalResultValues;
cse::float64 SystemAge = -1;
cse::ustringlist StarSpectralTypes;
cse::ustring CombinedSpectralType;

void ExportPopulations(cse::PlanetarySystemPointer& System, cse::uint64* NSatellites, cse::uint64* NMinorPlanets, cse::uint64* NComets)
{
    if ("Star" == System->PObject->Type)
    {
        PopulationStars.push_back(System->PObject->Name[0]);
        StarSpectralTypes.push_back(System->PObject->SpecClass);
    }
    else if ("Planet" == System->PObject->Type)
        {PopulationPlanets.push_back(System->PObject->Name[0]);}
    else if ("DwarfPlanet" == System->PObject->Type)
        {PopulationDwarfPlanets.push_back(System->PObject->Name[0]);}
    else if ("Moon" == System->PObject->Type || "DwarfMoon" == System->PObject->Type)
        {++(*NSatellites);}
    else if ("Asteroid" == System->PObject->Type)
        {++(*NMinorPlanets);}
    else if ("Comet" == System->PObject->Type)
        {++(*NComets);}

    if (!IS_NO_DATA_DBL(System->PObject->Age))
    {
        SystemAge = cse::max(SystemAge, System->PObject->Age);
    }

    for (auto& i : System->PSubSystem)
    {
        ExportPopulations(i, NSatellites, NMinorPlanets, NComets);
    }
}

void gbuffer_system(cse::PlanetarySystemPointer& System)
{
    cse::CSESysDebug("gbuffer_system", cse::CSEDebugger::INFO, "Loading population data...");

    cse::uint64 NStars, NPlanets, NDwarfPlanets;

    ExportPopulations(System, &NSatellites, &NMinorPlanets, &NComets);

    NStars = PopulationStars.size();
    NPlanets = PopulationPlanets.size();
    NDwarfPlanets = PopulationDwarfPlanets.size();

    cse::CSESysDebug("gbuffer_system", cse::CSEDebugger::INFO,
        fmt::format("Star count: {}", NStars));
    cse::CSESysDebug("gbuffer_system", cse::CSEDebugger::INFO,
        fmt::format("Planet count: {}", NPlanets));
    cse::CSESysDebug("gbuffer_system", cse::CSEDebugger::INFO,
        fmt::format("Dwarf Planet count: {}", NDwarfPlanets));
    cse::CSESysDebug("gbuffer_system", cse::CSEDebugger::INFO,
        fmt::format("Satallite count: {}", NSatellites));
    cse::CSESysDebug("gbuffer_system", cse::CSEDebugger::INFO,
        fmt::format("Minor Planet count: {}", NMinorPlanets));
    cse::CSESysDebug("gbuffer_system", cse::CSEDebugger::INFO,
        fmt::format("Comet count: {}", NComets));

    TotalResultValues.push_back(fmt::arg("StarCount", NStars));
    TotalResultValues.push_back(fmt::arg("PlanetCount", NPlanets));
    TotalResultValues.push_back(fmt::arg("DwarfPlanetCount", NDwarfPlanets));
    TotalResultValues.push_back(fmt::arg("SatelliteCount", NSatellites));
    TotalResultValues.push_back(fmt::arg("MinorPlanetCount", NMinorPlanets));
    TotalResultValues.push_back(fmt::arg("CometCount", NComets));

    CombinedSpectralType = StarSpectralTypes.front();
    for (size_t i = 1; i < StarSpectralTypes.size(); ++i)
    {
        CombinedSpectralType += " + " + StarSpectralTypes[i];
    }
}
