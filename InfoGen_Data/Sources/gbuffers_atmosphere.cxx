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

#include "gbuffers_atmosphere.hxx"
#include "CSE/Base/ConstLists.h"

std::map<cse::PlanetarySystemPointer, fmt::dynamic_format_arg_store<fmt::format_context>> AtmoData;

void __DFS_Proc_Atmosphere(cse::PlanetarySystemPointer& System)
{
    if ((System->PObject->Type == "Planet" || System->PObject->Type == "DwarfPlanet" || System->PObject->Type == "Moon")
        && !System->PObject->NoAtmosphere)
    {
        AtmoData.insert({System, fmt::dynamic_format_arg_store<fmt::format_context>()});
        AtmoData.at(System).push_back(fmt::arg("AtmoPressure", System->PObject->Atmosphere.Pressure));
        AtmoData.at(System).push_back(fmt::arg("AtmoPressureHpa", System->PObject->Atmosphere.Pressure / 100));
        AtmoData.at(System).push_back(fmt::arg("AtmoPressureKpa", System->PObject->Atmosphere.Pressure / 1000));
        AtmoData.at(System).push_back(fmt::arg("AtmoPressureBar", System->PObject->Atmosphere.Pressure / 100000));
        AtmoData.at(System).push_back(fmt::arg("AtmoPressureAtm", System->PObject->Atmosphere.Pressure / StandardAtm));
    }

    for (auto i : System->PSubSystem)
    {
        __DFS_Proc_Atmosphere(i);
    }
}

void gbuffers_atmosphere(cse::PlanetarySystemPointer& System)
{
    cse::CSESysDebug("gbuffer_atmosphere", cse::CSEDebugger::INFO, "Processing atmosphere...");
    __DFS_Proc_Atmosphere(System);
    cse::CSESysDebug("gbuffer_atmosphere", cse::CSEDebugger::INFO, "DONE.");
}
