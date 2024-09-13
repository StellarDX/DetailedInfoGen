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
#include "CSE/Physics/Illuminants.h"
#include "gbuffers_object.hxx"
#include "gbuffers_object_star.hxx"
#include "IGConf.h"

std::string GetStarType(cse::PlanetarySystemPointer& System)
{
    cse::Illuminants::StellarClassification Classification(System->PObject->SpecClass);
    if (cse::Illuminants::IsMainSequence(Classification))
    {
        if (cse::Illuminants::IsOType(Classification))
        {
            return StaticStrings.at("OTypeStar").ToStdString();
        }
        if (cse::Illuminants::IsBType(Classification))
        {
            return StaticStrings.at("BTypeStar").ToStdString();
        }
        if (cse::Illuminants::IsAType(Classification))
        {
            return StaticStrings.at("ATypeStar").ToStdString();
        }
        if (cse::Illuminants::IsFType(Classification))
        {
            return StaticStrings.at("FTypeStar").ToStdString();
        }
        if (cse::Illuminants::IsGType(Classification))
        {
            return StaticStrings.at("GTypeStar").ToStdString();
        }
        if (cse::Illuminants::IsKType(Classification))
        {
            return StaticStrings.at("KTypeStar").ToStdString();
        }
        if (cse::Illuminants::IsMType(Classification))
        {
            return StaticStrings.at("MTypeStar").ToStdString();
        }
    }
    if (cse::Illuminants::IsSubDwarf(Classification))
    {
        if (cse::Illuminants::IsOType(Classification))
        {
            return StaticStrings.at("OTypeSubdwarf").ToStdString();
        }
        if (cse::Illuminants::IsBType(Classification))
        {
            return StaticStrings.at("BTypeSubdwarf").ToStdString();
        }
        return StaticStrings.at("Subdwarf").ToStdString();
    }
    if (cse::Illuminants::IsWolfRayet(Classification))
    {
        return StaticStrings.at("WolfRayetStar").ToStdString();
    }
    if (cse::Illuminants::IsSubGiant(Classification))
    {
        return StaticStrings.at("Subgiant").ToStdString();
    }
    if (cse::Illuminants::IsBrightGiant(Classification))
    {
        return StaticStrings.at("BrightGiant").ToStdString();
    }
    if (cse::Illuminants::IsSuperGiant(Classification))
    {
        if (cse::Illuminants::IsOType(Classification) ||
            cse::Illuminants::IsBType(Classification))
        {
            return StaticStrings.at("BSG").ToStdString();
        }
        if (cse::Illuminants::IsKType(Classification) ||
            cse::Illuminants::IsMType(Classification))
        {
            return StaticStrings.at("RSG").ToStdString();
        }
        if (cse::Illuminants::IsAType(Classification) ||
            cse::Illuminants::IsFType(Classification) ||
            cse::Illuminants::IsGType(Classification))
        {
            return StaticStrings.at("YSG").ToStdString();
        }
    }
    if (cse::Illuminants::IsHyperGiant(Classification))
    {
        return StaticStrings.at("HyperGiant").ToStdString();
    }
    if (cse::Illuminants::IsNormalGiant(Classification))
    {
        if (cse::Illuminants::IsOType(Classification) ||
            cse::Illuminants::IsBType(Classification))
        {
            return StaticStrings.at("BlueGiant").ToStdString();
        }
        if (cse::Illuminants::IsKType(Classification) ||
            cse::Illuminants::IsMType(Classification))
        {
            return StaticStrings.at("RedGiant").ToStdString();
        }
        if (cse::Illuminants::IsAType(Classification) ||
            cse::Illuminants::IsFType(Classification) ||
            cse::Illuminants::IsGType(Classification))
        {
            return StaticStrings.at("YellowGiant").ToStdString();
        }
    }
    if (cse::Illuminants::IsCarbonStar(Classification))
    {
        if (Classification | Classification.N)
        {
            return StaticStrings.at("CNStar").ToStdString();
        }
        if (Classification | Classification.CH)
        {
            return StaticStrings.at("CHStar").ToStdString();
        }
        return StaticStrings.at("CarbonStar").ToStdString();
    }
    if (cse::Illuminants::IsSTypeStar(Classification))
    {
        return StaticStrings.at("STypeStar").ToStdString();
    }
    if (cse::Illuminants::IsWhiteDwarf(Classification))
    {
        return StaticStrings.at("WhiteDwarf").ToStdString();
    }
    if (cse::Illuminants::IsNeutronStar(Classification))
    {
        return StaticStrings.at("NeutronStar").ToStdString();
    }
    if (cse::Illuminants::IsBlackHole(Classification))
    {
        if (!System->PObject->KerrSpin && !System->PObject->KerrCharge)
        {
            return StaticStrings.at("SchwarzschildBH").ToStdString();
        }
        if (System->PObject->KerrSpin && !System->PObject->KerrCharge)
        {
            return StaticStrings.at("KerrBH").ToStdString();
        }
        if (!System->PObject->KerrSpin && System->PObject->KerrCharge)
        {
            return StaticStrings.at("RNBH").ToStdString();
        }
        if (System->PObject->KerrSpin && System->PObject->KerrCharge)
        {
            return StaticStrings.at("KNBH").ToStdString();
        }
    }
    if (cse::Illuminants::IsBrownDwarf(Classification))
    {
        return StaticStrings.at("Substellar").ToStdString();
    }
    return "Star";
}

void gbuffers_object_star(cse::PlanetarySystemPointer& System)
{
    ObjectCharacteristics.insert({System, fmt::dynamic_format_arg_store<fmt::format_context>()});
    ObjectCharacteristics.at(System).push_back(fmt::arg("Name", System->PObject->Name[0].ToStdString()));
    ObjectCharacteristics.at(System).push_back(fmt::arg("Type", GetStarType(System)));

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

    if (cse::Illuminants::IsBlackHole(System->PObject->SpecClass))
    {
        ObjectCharacteristics.at(System).push_back(fmt::arg("Mass", System->PObject->Mass));
        ObjectCharacteristics.at(System).push_back(fmt::arg("MassEarth", System->PObject->Mass / EarthMass));
        ObjectCharacteristics.at(System).push_back(fmt::arg("MassJup", System->PObject->Mass / JupiterMass));
        ObjectCharacteristics.at(System).push_back(fmt::arg("MassSun", System->PObject->Mass / SolarMass));
        cse::float64 SchwarzschildRadius = (2. * GravConstant * System->PObject->Mass) / (cse::float64(SpeedOfLight) * SpeedOfLight);
        ObjectCharacteristics.at(System).push_back(fmt::arg("SchwarzschildRadius", SchwarzschildRadius));
        ObjectCharacteristics.at(System).push_back(fmt::arg("SchwarzschildRadiusKm", SchwarzschildRadius / 1000));
        ObjectCharacteristics.at(System).push_back(fmt::arg("SchwarzschildRadiusEarth", SchwarzschildRadius / EarthRadius));
        ObjectCharacteristics.at(System).push_back(fmt::arg("SchwarzschildRadiusJup", SchwarzschildRadius / JupiterRadius));
        ObjectCharacteristics.at(System).push_back(fmt::arg("SchwarzschildRadiusSun", SchwarzschildRadius / SolarRadius));
        ObjectCharacteristics.at(System).push_back(fmt::arg("KerrSpin", System->PObject->KerrSpin));
        ObjectCharacteristics.at(System).push_back(fmt::arg("KerrCharge", System->PObject->KerrCharge));
    }
    else
    {
        ObjectCharacteristics.at(System).push_back(fmt::arg("AbsMagn", cse::Illuminants::GetAbsMagnBolFromLumBol(System->PObject->LumBol)));
        ObjectCharacteristics.at(System).push_back(fmt::arg("SpecType", System->PObject->SpecClass.ToStdString()));
        ObjectCharacteristics.at(System).push_back(fmt::arg("Metallicity", System->PObject->FeH));
        ObjectCharacteristics.at(System).push_back(fmt::arg("Obliquity", System->PObject->Rotation.Obliquity));
        ObjectCharacteristics.at(System).push_back(fmt::arg("RotationPeriod", System->PObject->Rotation.RotationPeriod));
        ObjectCharacteristics.at(System).push_back(fmt::arg("RotationPeriodHours", System->PObject->Rotation.RotationPeriod / 3600));
        ObjectCharacteristics.at(System).push_back(fmt::arg("RotationPeriodDays", System->PObject->Rotation.RotationPeriod / SynodicDay));
        ObjectCharacteristics.at(System).push_back(fmt::arg("RotationVelocity", cse::ObjectLiterals::EquatorialRotationVelocity(*System->PObject)));
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
        ObjectCharacteristics.at(System).push_back(fmt::arg("Age", System->PObject->Age));
        ObjectCharacteristics.at(System).push_back(fmt::arg("SurfaceGravity", cse::ObjectLiterals::SurfaceGravity(*System->PObject)));
        ObjectCharacteristics.at(System).push_back(fmt::arg("EscapeVelocity", cse::ObjectLiterals::EscapeVelocity(*System->PObject)));
        ObjectCharacteristics.at(System).push_back(fmt::arg("Temperature", System->PObject->Temperature));
        ObjectCharacteristics.at(System).push_back(fmt::arg("TemperatureCelsius", System->PObject->Temperature - 273.15));
        ObjectCharacteristics.at(System).push_back(fmt::arg("Luminosity", System->PObject->Luminosity));
        ObjectCharacteristics.at(System).push_back(fmt::arg("LuminositySun", System->PObject->Luminosity / SolarLum));
        ObjectCharacteristics.at(System).push_back(fmt::arg("LumBol", System->PObject->LumBol));
        ObjectCharacteristics.at(System).push_back(fmt::arg("LumBolSun", System->PObject->LumBol / SolarLum));
    }
}
