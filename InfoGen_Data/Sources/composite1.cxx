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
#include "gbuffers_atmosphere.hxx"
#include "gbuffers_object.hxx"
#include "IGConf.h"
#include "gbuffers_ocean.hxx"
#include "satellitelist.hxx"

std::string BarycenterFormatString;
std::string BarycenterBinaryFormatString;
std::string StarFormatString;
std::string StarBinaryFormatString;
std::string BHFormatString;
std::string BHBinaryFormatString;
std::string PlanetFormatString;
std::string PlanetBinaryFormatString;
std::string SatelliteFormatString;

std::string AtmosphereFormatString;
std::string AtmoCompFormatString;
std::string OceanFormatString;
std::string OceanCompFormatString;

std::string ObjectTables;

void LoadObjectFormatStrings()
{
    cse::CSESysDebug("composite1", cse::CSEDebugger::INFO, "Loading object format strings...");

    BarycenterFormatString = LoadTemplateProfile("Barycenter");
    BarycenterBinaryFormatString = LoadTemplateProfile("MultipleSys");
    StarFormatString = LoadTemplateProfile("Star");
    StarBinaryFormatString = LoadTemplateProfile("MultiStar");
    BHFormatString = LoadTemplateProfile("BlackHole");
    BHBinaryFormatString = LoadTemplateProfile("BHCompanion");
    PlanetFormatString = LoadTemplateProfile("Planet");
    PlanetBinaryFormatString = LoadTemplateProfile("MultiPlanet");
    AtmosphereFormatString = LoadTemplateProfile("Atmosphere");
    AtmoCompFormatString = LoadTemplateProfile("AtmoComp");
    OceanFormatString = LoadTemplateProfile("Ocean");
    OceanCompFormatString = LoadTemplateProfile("OceanComp");
    SatelliteFormatString = LoadTemplateProfile("Satellite");

    cse::CSESysDebug("composite", cse::CSEDebugger::INFO, "DONE.");
}

void CreateAtmosphereData(cse::PlanetarySystemPointer& System)
{
    if (AtmoData.contains(System))
    {
        cse::CSESysDebug("composite1", cse::CSEDebugger::INFO,
            fmt::format("Creating atmosphere data for {}", System->PObject->Name[0].ToStdString()));
        std::string AtmoCompositions;
        for (auto i : System->PObject->Atmosphere.Composition)
        {
            auto CompName = fmt::arg("CompName", i.first.ToStdString());
            auto CompVal = fmt::arg("CompValue", i.second);
            AtmoCompositions += fmt::vformat(AtmoCompFormatString,
                fmt::make_format_args(CompName, CompVal));
        }
        AtmoData.at(System).push_back(fmt::arg("AtmoCompositions", AtmoCompositions));
        ObjectCharacteristics.at(System).push_back(fmt::arg("Atmosphere",
            fmt::vformat(AtmosphereFormatString, AtmoData.at(System))));
    }
    else
    {
        ObjectCharacteristics.at(System).push_back(fmt::arg("Atmosphere", ""));
    }
}

void CreateOceanData(cse::PlanetarySystemPointer& System)
{
    if (OceanData.contains(System))
    {
        cse::CSESysDebug("composite1", cse::CSEDebugger::INFO,
            fmt::format("Creating ocean data for {}", System->PObject->Name[0].ToStdString()));
        std::string OceanCompositions;
        for (auto i : System->PObject->Ocean.Composition)
        {
            auto CompName = fmt::arg("CompName", i.first.ToStdString());
            auto CompVal = fmt::arg("CompValue", i.second);
            OceanCompositions += fmt::vformat(OceanCompFormatString,
                fmt::make_format_args(CompName, CompVal));
        }
        OceanData.at(System).push_back(fmt::arg("OceanCompositions", OceanCompositions));
        ObjectCharacteristics.at(System).push_back(fmt::arg("Ocean",
            fmt::vformat(OceanFormatString, OceanData.at(System))));
    }
    else
    {
        ObjectCharacteristics.at(System).push_back(fmt::arg("Ocean", ""));
    }
}

void __DFS_AddTable(cse::PlanetarySystemPointer& System)
{
    if (ObjectCharacteristics.contains(System))
    {
        cse::CSESysDebug("composite1", cse::CSEDebugger::INFO,
            fmt::format("Adding table for object: {}", System->PObject->Name[0].ToStdString()));

        if (System->PObject->Type == "Barycenter")
        {
            if (System->PObject->Class != "Sun") {SatelliteList(System);}
            else
            {
                ObjectCharacteristics.at(System).push_back(fmt::arg("SatelliteTable", ""));
            }

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
            else
            {
                if (System->PObject->Orbit.Binary)
                {
                    ObjectTables.append(fmt::vformat(BHBinaryFormatString, ObjectCharacteristics.at(System)));
                }
                else
                {
                    ObjectTables.append(fmt::vformat(BHFormatString, ObjectCharacteristics.at(System)));
                }
            }
        }

        if (System->PObject->Type == "Planet" || System->PObject->Type == "DwarfPlanet")
        {
            if (!System->PSubSystem.empty()) {SatelliteList(System);}
            else
            {
                ObjectCharacteristics.at(System).push_back(fmt::arg("SatelliteTable", ""));
            }
            CreateAtmosphereData(System);
            CreateOceanData(System);
            if (System->PObject->Orbit.Binary)
            {
                ObjectTables.append(fmt::vformat(PlanetBinaryFormatString, ObjectCharacteristics.at(System)));
            }
            else
            {
                ObjectTables.append(fmt::vformat(PlanetFormatString, ObjectCharacteristics.at(System)));
            }
        }

        if (System->PObject->Type == "Moon")
        {
            CreateAtmosphereData(System);
            CreateOceanData(System);
            ObjectTables.append(fmt::vformat(SatelliteFormatString, ObjectCharacteristics.at(System)));
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
