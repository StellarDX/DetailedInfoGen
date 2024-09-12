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

#include "gbuffers_object_planet.hxx"
#include "CSE/Base/ConstLists.h"
#include "gbuffers_object.hxx"
#include "IGConf.h"

void CalculateTemperature(cse::PlanetarySystemPointer& System)
{
    cse::float64 TotalTemperature = 0;
    vec3 SystemPos = Coordinates.at(System).Position;
    for (auto i : Coordinates)
    {
        if (i.first->PObject->Type == "Star")
        {
            vec3 StarPos = i.second.Position;
            cse::float64 Dist = cse::linalg::distance(SystemPos, StarPos);
            cse::float64 CurrrentTemperature = cse::ObjectLiterals::EquilibriumTemperature(*(i.first->PObject), *(System->PObject), Dist);
            cse::CSESysDebug("gbuffer_object_planet", cse::CSEDebugger::INFO,
                fmt::format("Distance between {} and star {} is {}, LStar = {}, Temperature = {}",
                System->PObject->Name[0].ToStdString(),
                i.first->PObject->Name[0].ToStdString(),
                Dist / AU,
                i.first->PObject->LumBol,
                CurrrentTemperature));
            TotalTemperature += CurrrentTemperature;
        }
    }
    cse::CSESysDebug("gbuffer_object_planet", cse::CSEDebugger::INFO,
        fmt::format("Total temperature of {}: {}",
        System->PObject->Name[0].ToStdString(), TotalTemperature));
    System->PObject->Temperature = TotalTemperature;
}

bool IsRockyPlanet(cse::Object Obj)
{
    return Obj.Class != "GasGiant" && Obj.Class != "Jupiter" &&
           Obj.Class != "IceGiant" && Obj.Class != "Neptune" &&
           Obj.Class != "Chthonia" && Obj.Class != "HeliumGiant";
}

std::string __Sudarsky_Gas_Giant_Classification(cse::float64 Temperature)
{
    if (Temperature < 150)
    {
        return StaticStrings.at("GasGiantClassI").ToStdString();
    }
    if (Temperature > 150 && Temperature < 350)
    {
        return StaticStrings.at("GasGiantClassII").ToStdString();
    }
    if (Temperature > 350 && Temperature < 800)
    {
        return StaticStrings.at("GasGiantClassIII").ToStdString();
    }
    if (Temperature > 800 && Temperature < 1400)
    {
        return StaticStrings.at("GasGiantClassIV").ToStdString();
    }
    if (Temperature > 1400)
    {
        return StaticStrings.at("GasGiantClassV").ToStdString();
    }
}

std::string GetPlanetType(cse::PlanetarySystemPointer& System)
{
    if (!System->PObject->NoOcean && System->PObject->Ocean.Height > 30000)
    {
        if (System->PObject->Atmosphere.Composition.contains("H2") &&
            System->PObject->Atmosphere.Composition.at("H2") > 30)
        {
            return StaticStrings.at("Hycean").ToStdString();
        }
        else {return StaticStrings.at("Oceania").ToStdString();}
    }

    if (IsRockyPlanet(*System->PObject) && System->PObject->Temperature > 1000 &&
        (System->PObject->Surface.lavaCoverTidal || System->PObject->Surface.lavaCoverSun || System->PObject->Surface.lavaCoverYoung))
    {
        return StaticStrings.at("LavaPlanet").ToStdString();
    }

    if (System->PObject->Class == "Ferria")
    {
        return StaticStrings.at("Ferria").ToStdString();
    }
    if (System->PObject->Class == "Terra")
    {
        cse::float64 MeanRad = cse::ObjectLiterals::MeanRadius(*System->PObject);
        if (MeanRad < 0.8 * EarthRadius)
        {
            return StaticStrings.at("SubEarth").ToStdString();
        }
        if (MeanRad > 0.8 * EarthRadius && MeanRad < 1.2 * EarthRadius)
        {
            return StaticStrings.at("Terra").ToStdString();
        }
        if (MeanRad > 1.2 * EarthRadius && MeanRad < 2.2 * EarthRadius)
        {
            return StaticStrings.at("SuperEarth").ToStdString();
        }
        if (MeanRad > 2.2 * EarthRadius)
        {
            return StaticStrings.at("MegaEarth").ToStdString();
        }
        return StaticStrings.at("SilicatePlanet").ToStdString();
    }
    if (System->PObject->Class == "Carbonia")
    {
        return StaticStrings.at("Carbonia").ToStdString();
    }
    if (System->PObject->Class == "Aquaria")
    {
        return StaticStrings.at("IceWorld").ToStdString();
    }

    if (System->PObject->Class == "Neptune" || System->PObject->Class == "IceGiant")
    {
        if (System->PObject->Temperature > 600)
        {
            return StaticStrings.at("HotNeptune").ToStdString();
        }
        cse::float64 MeanRad = cse::ObjectLiterals::MeanRadius(*System->PObject);
        if (MeanRad < 2 * EarthRadius)
        {
            return StaticStrings.at("MiniNeptune").ToStdString();
        }
        if (MeanRad > 2 * EarthRadius && MeanRad < 3 * EarthRadius)
        {
            return StaticStrings.at("SubNeptune").ToStdString();
        }
        if (MeanRad > 3 * EarthRadius && MeanRad < 5 * EarthRadius)
        {
            return StaticStrings.at("IceGiant").ToStdString();
        }
        if (MeanRad > 5 * EarthRadius)
        {
            return StaticStrings.at("SuperNeptune").ToStdString();
        }
    }

    if (System->PObject->Class == "Jupiter" || System->PObject->Class == "GasGiant")
    {
        std::string SudarskyClass = __Sudarsky_Gas_Giant_Classification(System->PObject->Temperature);
        if (System->PObject->Orbit.Eccentricity >= 0.2)
        {
            return SudarskyClass + ' ' + StaticStrings.at("EccentricJupiter").ToStdString();
        }
        if (System->PObject->Temperature >= 700)
        {
            return SudarskyClass + ' ' + StaticStrings.at("HotJupiter").ToStdString();
        }
        if (System->PObject->Mass < 3 * JupiterMass)
        {
            return SudarskyClass + ' ' + StaticStrings.at("SuperJupiter").ToStdString();
        }
        return SudarskyClass + ' ' + StaticStrings.at("GasGiant").ToStdString();
    }

    if (System->PObject->Class == "Chthonia" || System->PObject->Class == "HeliumGiant")
    {
        return StaticStrings.at("Chthonia").ToStdString();
    }
}

void gbuffers_object_planet(cse::PlanetarySystemPointer& System)
{
    ObjectCharacteristics.insert({System, fmt::dynamic_format_arg_store<fmt::format_context>()});
    ObjectCharacteristics.at(System).push_back(fmt::arg("Name", System->PObject->Name[0].ToStdString()));

    CalculateTemperature(System);
    ObjectCharacteristics.at(System).push_back(fmt::arg("Type", GetPlanetType(System)));

    ObjectCharacteristics.at(System).push_back(fmt::arg("OrbitEpoch", System->PObject->Orbit.Epoch));
    ObjectCharacteristics.at(System).push_back(fmt::arg("OrbitRefSystem", System->PObject->Orbit.RefPlane.ToStdString()));
    ObjectCharacteristics.at(System).push_back(fmt::arg("OrbitPrimary", System->PObject->ParentBody.ToStdString()));
    ObjectCharacteristics.at(System).push_back(fmt::arg("OrbitCompanion", System->PObject->Name[0].ToStdString()));
    ObjectCharacteristics.at(System).push_back(fmt::arg("OrbitPeriod", System->PObject->Orbit.Period));
    ObjectCharacteristics.at(System).push_back(fmt::arg("OrbitPeriodDays", System->PObject->Orbit.Period / SynodicDay));
    ObjectCharacteristics.at(System).push_back(fmt::arg("OrbitPeriodYears", System->PObject->Orbit.Period / JulianYear));
    cse::float64 Aphelion = cse::ObjectLiterals::Aphelion(*System->PObject);
    ObjectCharacteristics.at(System).push_back(fmt::arg("OrbitAphelion", Aphelion));
    ObjectCharacteristics.at(System).push_back(fmt::arg("OrbitAphelionKm", Aphelion / 1000.));
    ObjectCharacteristics.at(System).push_back(fmt::arg("OrbitAphelionAU", Aphelion / AU));
    cse::float64 Perihelion = cse::ObjectLiterals::Perihelion(*System->PObject);
    ObjectCharacteristics.at(System).push_back(fmt::arg("OrbitPerihelion", Perihelion));
    ObjectCharacteristics.at(System).push_back(fmt::arg("OrbitPerihelionKm", Perihelion / 1000.));
    ObjectCharacteristics.at(System).push_back(fmt::arg("OrbitPerihelionAU", Perihelion / AU));
    cse::float64 SemiMajorAxis = cse::ObjectLiterals::SemiMajorAxis(*System->PObject);
    ObjectCharacteristics.at(System).push_back(fmt::arg("OrbitSemiMajorAxis", SemiMajorAxis));
    ObjectCharacteristics.at(System).push_back(fmt::arg("OrbitSemiMajorAxisKm", SemiMajorAxis / 1000.));
    ObjectCharacteristics.at(System).push_back(fmt::arg("OrbitSemiMajorAxisAU", SemiMajorAxis / AU));
    ObjectCharacteristics.at(System).push_back(fmt::arg("OrbitEccentricity", System->PObject->Orbit.Eccentricity));
    ObjectCharacteristics.at(System).push_back(fmt::arg("OrbitInclination", System->PObject->Orbit.Inclination));
    ObjectCharacteristics.at(System).push_back(fmt::arg("OrbitAscendingNode", System->PObject->Orbit.AscendingNode));
    ObjectCharacteristics.at(System).push_back(fmt::arg("OrbitArgOfPericenter", System->PObject->Orbit.ArgOfPericenter));
    ObjectCharacteristics.at(System).push_back(fmt::arg("OrbitMeanAnomaly", System->PObject->Orbit.MeanAnomaly));

    cse::float64 EqRadius = cse::ObjectLiterals::EquatorialRadius(*System->PObject);
    ObjectCharacteristics.at(System).push_back(fmt::arg("EquatorialRadius", EqRadius));
    ObjectCharacteristics.at(System).push_back(fmt::arg("EquatorialRadiusKm", EqRadius / 1000));
    ObjectCharacteristics.at(System).push_back(fmt::arg("EquatorialRadiusEarth", EqRadius / EarthRadius));
    ObjectCharacteristics.at(System).push_back(fmt::arg("EquatorialRadiusJup", EqRadius / JupiterRadius));
    ObjectCharacteristics.at(System).push_back(fmt::arg("EquatorialRadiusSun", EqRadius / SolarRadius));
    cse::float64 PoRadius = cse::ObjectLiterals::PolarRadius(*System->PObject);
    ObjectCharacteristics.at(System).push_back(fmt::arg("PolarRadius", PoRadius));
    ObjectCharacteristics.at(System).push_back(fmt::arg("PolarRadiusKm", PoRadius / 1000));
    ObjectCharacteristics.at(System).push_back(fmt::arg("PolarRadiusEarth", PoRadius / EarthRadius));
    ObjectCharacteristics.at(System).push_back(fmt::arg("PolarRadiusJup", PoRadius / JupiterRadius));
    ObjectCharacteristics.at(System).push_back(fmt::arg("PolarRadiusSun", PoRadius / SolarRadius));
    cse::float64 MeRadius = cse::ObjectLiterals::MeanRadius(*System->PObject);
    ObjectCharacteristics.at(System).push_back(fmt::arg("MeanRadius", MeRadius));
    ObjectCharacteristics.at(System).push_back(fmt::arg("MeanRadiusKm", MeRadius / 1000));
    ObjectCharacteristics.at(System).push_back(fmt::arg("MeanRadiusEarth", MeRadius / EarthRadius));
    ObjectCharacteristics.at(System).push_back(fmt::arg("MeanRadiusJup", MeRadius / JupiterRadius));
    ObjectCharacteristics.at(System).push_back(fmt::arg("MeanRadiusSun", MeRadius / SolarRadius));
    ObjectCharacteristics.at(System).push_back(fmt::arg("DimensionsX", System->PObject->Dimensions.x));
    ObjectCharacteristics.at(System).push_back(fmt::arg("DimensionsY", System->PObject->Dimensions.y));
    ObjectCharacteristics.at(System).push_back(fmt::arg("DimensionsZ", System->PObject->Dimensions.z));
    ObjectCharacteristics.at(System).push_back(fmt::arg("Flattening", cse::ObjectLiterals::Flattening(*System->PObject).y));
    ObjectCharacteristics.at(System).push_back(fmt::arg("ECircumference", cse::ObjectLiterals::EquatorialCircumference(*System->PObject)));
    ObjectCharacteristics.at(System).push_back(fmt::arg("MCircumference", cse::ObjectLiterals::MeridionalCircumference(*System->PObject)));
    ObjectCharacteristics.at(System).push_back(fmt::arg("SurfaceArea", cse::ObjectLiterals::SurfaceArea(*System->PObject)));
    ObjectCharacteristics.at(System).push_back(fmt::arg("Volume", cse::ObjectLiterals::Volume(*System->PObject)));
    ObjectCharacteristics.at(System).push_back(fmt::arg("Mass", System->PObject->Mass));
    ObjectCharacteristics.at(System).push_back(fmt::arg("MassEarth", System->PObject->Mass / EarthMass));
    ObjectCharacteristics.at(System).push_back(fmt::arg("MassJup", System->PObject->Mass / JupiterMass));
    ObjectCharacteristics.at(System).push_back(fmt::arg("MassSun", System->PObject->Mass / SolarMass));
    ObjectCharacteristics.at(System).push_back(fmt::arg("AverageDensity", cse::ObjectLiterals::MeanDensity(*System->PObject)));
    ObjectCharacteristics.at(System).push_back(fmt::arg("SurfaceGravity", cse::ObjectLiterals::SurfaceGravity(*System->PObject)));
    ObjectCharacteristics.at(System).push_back(fmt::arg("EscapeVelocity", cse::ObjectLiterals::EscapeVelocity(*System->PObject)));
    cse::float64 SynodicPeriod = cse::ObjectLiterals::SynodicRotationPeriod(*System->PObject);
    ObjectCharacteristics.at(System).push_back(fmt::arg("SynRotationPeriod", SynodicPeriod));
    ObjectCharacteristics.at(System).push_back(fmt::arg("SynRotationPeriodHours", SynodicPeriod / 3600));
    ObjectCharacteristics.at(System).push_back(fmt::arg("SynRotationPeriodDays", SynodicPeriod / SynodicDay));
    ObjectCharacteristics.at(System).push_back(fmt::arg("RotationPeriod", System->PObject->Rotation.RotationPeriod));
    ObjectCharacteristics.at(System).push_back(fmt::arg("RotationPeriodHours", System->PObject->Rotation.RotationPeriod / 3600));
    ObjectCharacteristics.at(System).push_back(fmt::arg("RotationPeriodDays", System->PObject->Rotation.RotationPeriod / SynodicDay));
    ObjectCharacteristics.at(System).push_back(fmt::arg("RotationVelocity", cse::ObjectLiterals::EquatorialRotationVelocity(*System->PObject)));
    ObjectCharacteristics.at(System).push_back(fmt::arg("Obliquity", System->PObject->Rotation.Obliquity));
    ObjectCharacteristics.at(System).push_back(fmt::arg("AlbedoBond", System->PObject->AlbedoBond));
    ObjectCharacteristics.at(System).push_back(fmt::arg("AlbedoGeom", System->PObject->AlbedoGeom));
    ObjectCharacteristics.at(System).push_back(fmt::arg("Temperature", System->PObject->Temperature));
    ObjectCharacteristics.at(System).push_back(fmt::arg("TemperatureCelsius", System->PObject->Temperature - 273.15));
}
