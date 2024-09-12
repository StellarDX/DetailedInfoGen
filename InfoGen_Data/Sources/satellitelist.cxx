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

#include "satellitelist.hxx"
#include "gbuffers_object.hxx"
#include "IGConf.h"

std::string SatelliteTableMainTemplate;
std::string SatelliteTableItemTemplate;

void LoadSatelliteTableTemplates()
{
    SatelliteTableMainTemplate = LoadTemplateProfile("SateTable");
    SatelliteTableItemTemplate = LoadTemplateProfile("SateTableRow");
}

std::string GenerateSateTableItem(cse::PlanetarySystemPointer& System)
{
    return fmt::vformat(SatelliteTableItemTemplate, ObjectCharacteristics.at(System));
}

void SatelliteList(cse::PlanetarySystemPointer& System)
{
    if (System->PSubSystem.empty())
    {
        ObjectCharacteristics.at(System).push_back(fmt::arg("SatelliteTable", ""));
        return;
    }

    cse::CSESysDebug("satellitelist", cse::CSEDebugger::INFO,
        fmt::format("Adding satellite list for object: {}", System->PObject->Name[0].ToStdString()));

    LoadSatelliteTableTemplates();

    std::string SateTableItems;
    for (auto i : System->PSubSystem)
    {
        if (ObjectCharacteristics.contains(i))
        {
            SateTableItems += GenerateSateTableItem(i);
        }
    }
    auto SateTableItemArg = fmt::arg("SatelliteTableItems", SateTableItems);

    ObjectCharacteristics.at(System).push_back(fmt::arg("SatelliteTable",
        fmt::vformat(SatelliteTableMainTemplate, fmt::make_format_args(SateTableItemArg))));
}
