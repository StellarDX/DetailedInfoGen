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

#include "CSE/Base/ConstLists.h"
#include "gbuffer_object.hxx"
#include "gbuffer_object_barycenter.hxx"
#include "IGConf.h"

std::string GetBinaryNature(cse::PlanetarySystemPointer& System)
{
    try
    {
        std::string BinaryNature = BinaryNatures.at(System);
        if (BinaryNature == "EB") {return StaticStrings.at("EclipsingBinary").ToStdString();}
        if (BinaryNature == "SB1") {return StaticStrings.at("SB1System").ToStdString();}
        if (BinaryNature == "SB2") {return StaticStrings.at("SB2System").ToStdString();}
        if (BinaryNature == "A") {return StaticStrings.at("AstrometricSystem").ToStdString();}
        if (BinaryNature == "C") {return StaticStrings.at("CPMSystem").ToStdString();}
        if (BinaryNature == "V") {return StaticStrings.at("VisualSystem").ToStdString();}
    }
    catch (...)
    {
        return StaticStrings.at("OtherBarycenters").ToStdString();
    }
}

void gbuffer_object_barycenter(cse::PlanetarySystemPointer& System)
{
    ObjectCharacteristics.insert({System, fmt::dynamic_format_arg_store<fmt::format_context>()});
    ObjectCharacteristics.at(System).push_back(fmt::arg("Name", System->PObject->Name[0].ToStdString()));
    ObjectCharacteristics.at(System).push_back(fmt::arg("Type", GetBinaryNature(System)));
    ObjectCharacteristics.at(System).push_back(fmt::arg("OrbitEpoch", System->PObject->Orbit.Epoch));
    ObjectCharacteristics.at(System).push_back(fmt::arg("OrbitRefSystem", System->PObject->Orbit.RefPlane.ToStdString()));
    ObjectCharacteristics.at(System).push_back(fmt::arg("OrbitPrimary", System->PObject->ParentBody.ToStdString()));
    ObjectCharacteristics.at(System).push_back(fmt::arg("OrbitCompanion", System->PObject->Name[0].ToStdString()));
    ObjectCharacteristics.at(System).push_back(fmt::arg("OrbitPeriod", System->PObject->Orbit.Period));
    ObjectCharacteristics.at(System).push_back(fmt::arg("OrbitPeriodDays", System->PObject->Orbit.Period / SynodicDay));
    ObjectCharacteristics.at(System).push_back(fmt::arg("OrbitPeriodYears", System->PObject->Orbit.Period / JulianYear));
    ObjectCharacteristics.at(System).push_back(fmt::arg("OrbitSemiMajorAxis", cse::ObjectLiterals::SemiMajorAxis(*System->PObject)));
    ObjectCharacteristics.at(System).push_back(fmt::arg("OrbitSemiMajorAxisKm", cse::ObjectLiterals::SemiMajorAxis(*System->PObject) / 1000.));
    ObjectCharacteristics.at(System).push_back(fmt::arg("OrbitSemiMajorAxisAU", cse::ObjectLiterals::SemiMajorAxis(*System->PObject) / AU));
    ObjectCharacteristics.at(System).push_back(fmt::arg("OrbitEccentricity", System->PObject->Orbit.Eccentricity));
    ObjectCharacteristics.at(System).push_back(fmt::arg("OrbitInclination", System->PObject->Orbit.Inclination));
    ObjectCharacteristics.at(System).push_back(fmt::arg("OrbitAscendingNode", System->PObject->Orbit.AscendingNode));
    ObjectCharacteristics.at(System).push_back(fmt::arg("OrbitArgOfPericenter", System->PObject->Orbit.ArgOfPericenter));
    ObjectCharacteristics.at(System).push_back(fmt::arg("OrbitMeanAnomaly", System->PObject->Orbit.MeanAnomaly));
}
