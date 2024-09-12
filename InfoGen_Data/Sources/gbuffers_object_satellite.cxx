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

#include <CSE/Base/ConstLists.h>
#include "gbuffers_object.hxx"
#include "gbuffers_object_planet.hxx"
#include "gbuffers_object_satellite.hxx"

extern std::stack<cse::PlanetarySystemPointer> __ParentStack;
cse::PlanetarySystemPointer GetParentBody(cse::PlanetarySystemPointer& System)
{
    if (__ParentStack.empty()) {return nullptr;}
    decltype(__ParentStack) ParentStack = __ParentStack;
    ParentStack.pop();
    if (ParentStack.empty()) {return nullptr;}
    return ParentStack.top();
}

cse::float64 __Synodic_Period(cse::float64 P1, cse::float64 I1, cse::float64 P2, cse::float64 I2)
{
    cse::float64 CompanionPeriod, PrimaryPeriod;
    if (cse::Quadrant(I2) >= 2 && cse::Quadrant(I2) < 6 && P2 > 0)
    {
        CompanionPeriod = -P2;
    }
    else {CompanionPeriod = P2;}

    if (cse::Quadrant(I1) >= 2 && cse::Quadrant(I1) < 6 && P1 > 0)
    {
        PrimaryPeriod = -P1;
    }
    else {PrimaryPeriod = P1;}

    return (PrimaryPeriod * CompanionPeriod) / (PrimaryPeriod - CompanionPeriod);
}

cse::float64 CalculateSynodicOrbitalPeriod(cse::PlanetarySystemPointer& Parent, cse::PlanetarySystemPointer& Companion)
{
    return __Synodic_Period(Parent->PObject->Orbit.Period, Parent->PObject->Orbit.Inclination,
        Companion->PObject->Orbit.Period, Companion->PObject->Orbit.Inclination);
}

cse::float64 CalculateSatelliteSynodicDay(cse::PlanetarySystemPointer& Parent, cse::PlanetarySystemPointer& Companion)
{
    return __Synodic_Period(Parent->PObject->Orbit.Period, Parent->PObject->Orbit.Inclination,
        Companion->PObject->Rotation.RotationPeriod, Companion->PObject->Rotation.Obliquity);
}

void gbuffers_object_satellite(cse::PlanetarySystemPointer& System)
{
    auto ParentBody = GetParentBody(System);
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
    cse::float64 SynOrbPeriod = CalculateSynodicOrbitalPeriod(ParentBody, System);
    ObjectCharacteristics.at(System).push_back(fmt::arg("SynOrbitPeriod", SynOrbPeriod));
    ObjectCharacteristics.at(System).push_back(fmt::arg("SynOrbitPeriodDays", SynOrbPeriod / SynodicDay));
    ObjectCharacteristics.at(System).push_back(fmt::arg("SynOrbitPeriodYears", SynOrbPeriod / JulianYear));
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
    cse::float64 SynodicPeriod = CalculateSatelliteSynodicDay(ParentBody, System);
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
