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

#define _USE_CSE_DEFINES

#include "gbuffer_object.hxx"
#include <CSE/Base.h>
#include <CSE/Physics/Orbit.h>

#include "InfoGen.h"

#define NO_DATA_NUM -5.72957777e+39

std::map<cse::PlanetarySystemPointer, cse::Orbit::OrbitStateType> Coordinates;

std::map<cse::PlanetarySystemPointer, fmt::dynamic_format_arg_store<fmt::format_context>> ObjectCharacteristics;

bool IsMajorObject(cse::Object Obj)
{
    return Obj.Type == "Star" || Obj.Type == "Planet" || Obj.Type == "DwarfPlanet" ||
           Obj.Type == "Moon" || Obj.Type == "Barycenter";
}

cse::Orbit::OrbitStateType GetCurrentStateFromKeplerianOrbitElements(cse::float64 ParentMass, cse::Orbit::KeplerianOrbitElems Elem)
{
    cse::Orbit::__Real_Time_Satellite_Tracker_And_Predictor Tracker(Elem, ParentMass);
    Tracker.SetDateJD(OrbitCalculatorEpoch);
    return Tracker.CurrentState();
}

void FixEquatorRefPlane(cse::PlanetarySystemPointer& System)
{
    //cse::CSESysDebug("gbuffer_object", cse::CSEDebugger::INFO, "Fixing equator reference plane...");
    cse::CSESysDebug("gbuffer_object", cse::CSEDebugger::WARNING, _WARN("TODO: Equator reference plane Fixing will be added in future versions."));
    //cse::CSESysDebug("gbuffer_object", cse::CSEDebugger::INFO, "DONE.");
}

void CalculatePosition(cse::PlanetarySystemPointer& System)
{
    // USing BFS
    cse::CSESysDebug("gbuffer_object", cse::CSEDebugger::INFO, "Calculating positions...");
    std::queue<cse::PlanetarySystemPointer> Buffer;
    std::queue<vec3> PosBuffer;
    Buffer.push(System);
    Coordinates.insert({System, {.Position = vec3(0, 0, 0), .Velocity = vec3(0, 0, 0)}});
    PosBuffer.push({0, 0, 0});
    cse::CSESysDebug("gbuffer_object", cse::CSEDebugger::INFO, fmt::format(R"({} "{}" at pos ({}, {}, {}))",
        System->PObject->Type.ToStdString(), System->PObject->Name[0].ToStdString(), 0, 0, 0));
    while (!Buffer.empty())
    {
        auto CurrentElem = Buffer.front();
        auto CurrentParentPos = PosBuffer.front();
        Buffer.pop();
        PosBuffer.pop();
        for (auto i : CurrentElem->PSubSystem)
        {
            if (IsMajorObject(*(i->PObject)))
            {
                Buffer.push(i);
                auto State = GetCurrentStateFromKeplerianOrbitElements(CurrentElem->PObject->Mass, i->PObject->Orbit);
                State.Position += CurrentParentPos;
                Coordinates.insert({i, State});
                cse::CSESysDebug("gbuffer_object", cse::CSEDebugger::INFO, fmt::format(R"({} "{}" at pos ({}, {}, {}))",
                    i->PObject->Type.ToStdString(), i->PObject->Name[0].ToStdString(),
                    State.Position.x / AU, State.Position.y / AU, State.Position.z / AU));
                PosBuffer.push(State.Position);
            }

        }
    }
    cse::CSESysDebug("gbuffer_object", cse::CSEDebugger::INFO, "DONE.");
}

bool IsBinaryObject(cse::Object::OrbitParams Orbit1, cse::Object::OrbitParams Orbit2)
{
    return
    (
        cse::abs(Orbit1.Period - Orbit2.Period) < 1e-2 &&
        Orbit1.Eccentricity == Orbit2.Eccentricity &&
        Orbit1.Inclination == Orbit2.Inclination &&
        Orbit1.AscendingNode == Orbit2.AscendingNode &&
        cse::abs(Orbit1.ArgOfPericenter - Orbit2.ArgOfPericenter) - 180 < 1e-5 &&
        Orbit1.MeanAnomaly == Orbit2.MeanAnomaly
    );
}

void TransferOrbitalElems(cse::PlanetarySystemPointer& System)
{
    if (System->PObject->Type == "Barycenter")
    {
        cse::PlanetarySystemPointer Primary, Companion;

        for (size_t i = 0; i < System->PSubSystem.size(); ++i)
        {
            for (size_t j = i + 1; j < System->PSubSystem.size(); ++j)
            {
                if (IsBinaryObject(System->PSubSystem[i]->PObject->Orbit, System->PSubSystem[j]->PObject->Orbit))
                {
                    Primary = System->PSubSystem[i];
                    Companion = System->PSubSystem[j];
                }
            }
        }

        if (Primary != nullptr && Companion != nullptr)
        {
            cse::CSESysDebug("gbuffer_object", cse::CSEDebugger::INFO,
                fmt::format(R"(Transfering: Primary: "{}", Companion: "{}")",
                Primary->PObject->Name[0].ToStdString(),
                Companion->PObject->Name[0].ToStdString()));

            if (cse::ObjectLiterals::SemiMajorAxis(*Primary->PObject) >
                cse::ObjectLiterals::SemiMajorAxis(*Companion->PObject))
            {
                Primary.swap(Companion);
            }

            Companion->PObject->Orbit.Binary = true;
            Companion->PObject->Orbit.PericenterDist += Primary->PObject->Orbit.PericenterDist;
            Companion->PObject->ParentBody = Primary->PObject->Name[0];

            if (Primary->PObject->Type == "Star" || Primary->PObject->Type == "Barycenter")
            {
                Primary->PObject->Orbit = cse::Object::OrbitParams();
            }
            else
            {
                Primary->PObject->Orbit = System->PObject->Orbit;
                Primary->PObject->ParentBody = System->PObject->ParentBody;
                System->PObject->Orbit = cse::Object::OrbitParams();
            }
        }

        if (Primary->PObject->Rotation.TidalLocked && IS_NO_DATA_DBL(Primary->PObject->Rotation.RotationPeriod))
        {
            Primary->PObject->Rotation.RotationPeriod = Companion->PObject->Orbit.Period;
            Primary->PObject->Rotation.TidalLocked = false;
        }

        if (Companion->PObject->Rotation.TidalLocked && IS_NO_DATA_DBL(Companion->PObject->Rotation.RotationPeriod))
        {
            Companion->PObject->Rotation.RotationPeriod = Companion->PObject->Orbit.Period;
        }
    }

    for (auto i : System->PSubSystem)
    {
        TransferOrbitalElems(i);
    }
}

void gbuffer_object(cse::PlanetarySystemPointer& System)
{
    if (FixOrbitPlane)
    {
        FixEquatorRefPlane(System);
    }
    CalculatePosition(System);

    cse::CSESysDebug("gbuffer_object", cse::CSEDebugger::INFO, "Transfering orbit data...");
    TransferOrbitalElems(System);
    cse::CSESysDebug("gbuffer_object", cse::CSEDebugger::INFO, "DONE.");
}
