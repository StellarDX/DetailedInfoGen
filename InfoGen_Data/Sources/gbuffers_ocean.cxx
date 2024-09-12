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

#include "gbuffers_ocean.hxx"

std::map<cse::PlanetarySystemPointer, fmt::dynamic_format_arg_store<fmt::format_context>> OceanData;

void __DFS_Proc_Ocean(cse::PlanetarySystemPointer& System)
{
    if ((System->PObject->Type == "Planet" || System->PObject->Type == "DwarfPlanet" || System->PObject->Type == "Moon")
        && !System->PObject->NoOcean)
    {
        OceanData.insert({System, fmt::dynamic_format_arg_store<fmt::format_context>()});
        OceanData.at(System).push_back(fmt::arg("OceanDepth", System->PObject->Ocean.Height));
        OceanData.at(System).push_back(fmt::arg("OceanDepthKm", System->PObject->Ocean.Height / 1000));
    }

    for (auto i : System->PSubSystem)
    {
        __DFS_Proc_Ocean(i);
    }
}

void gbuffers_ocean(cse::PlanetarySystemPointer& System)
{
    cse::CSESysDebug("gbuffer_ocean", cse::CSEDebugger::INFO, "Processing ocean...");
    __DFS_Proc_Ocean(System);
    cse::CSESysDebug("gbuffer_ocean", cse::CSEDebugger::INFO, "DONE.");
}
