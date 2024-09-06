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

#include "CSE/Physics/Illuminants.h"
#include "final.hxx"
#include "composite1.hxx"
#include "gbuffer_object.hxx"
#include "IGConf.h"

std::string BarycenterFormatString;
std::string BarycenterBinaryFormatString;
std::string StarFormatString;
std::string StarBinaryFormatString;

std::string ObjectTables;

void LoadObjectFormatStrings()
{
    cse::CSESysDebug("composite1", cse::CSEDebugger::INFO, "Loading object format strings...");

    BarycenterFormatString = LoadTemplateProfile("Barycenter");
    BarycenterBinaryFormatString = LoadTemplateProfile("MultipleSys");
    StarFormatString = LoadTemplateProfile("Star");
    StarBinaryFormatString = LoadTemplateProfile("MultiStar");

    cse::CSESysDebug("composite", cse::CSEDebugger::INFO, "DONE.");
}

void __DFS_AddTable(cse::PlanetarySystemPointer& System)
{
    if (ObjectCharacteristics.contains(System))
    {
        cse::CSESysDebug("composite1", cse::CSEDebugger::INFO,
            fmt::format("Adding table for object: {}", System->PObject->Name[0].ToStdString()));

        if (System->PObject->Type == "Barycenter")
        {
            if (System->PObject->Orbit.Binary)
            {
                ObjectTables.append(fmt::vformat(BarycenterBinaryFormatString, ObjectCharacteristics.at(System)));
            }
            else
            {
                ObjectTables.append(fmt::vformat(BarycenterFormatString, ObjectCharacteristics.at(System)));
            }
        }

        if (System->PObject->Type == "Star")
        {
            if (!cse::Illuminants::IsBlackHole(System->PObject->SpecClass))
            {
                if (System->PObject->Orbit.Binary)
                {
                    ObjectTables.append(fmt::vformat(StarBinaryFormatString, ObjectCharacteristics.at(System)));
                }
                else
                {
                    ObjectTables.append(fmt::vformat(StarFormatString, ObjectCharacteristics.at(System)));
                }
            }
        }

        ObjectTables.push_back('\n');
    }

    for (auto& i : System->PSubSystem)
    {
        __DFS_AddTable(i);
    }
}

void composite1(cse::PlanetarySystemPointer& System)
{
    LoadObjectFormatStrings();
    __DFS_AddTable(System);
    GeneralValues.push_back(fmt::arg("ObjectTables", ObjectTables));
}
