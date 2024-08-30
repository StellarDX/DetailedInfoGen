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

#pragma once

#ifndef _IG_GOBJECT_
#define _IG_GOBJECT_

#include <fmt/args.h>
#include <CSE/PlanSystem.h>
#include "CSE/Physics/Orbit.h"

extern std::map<cse::PlanetarySystemPointer, cse::Orbit::OrbitStateType> Coordinates;

extern std::vector<fmt::dynamic_format_arg_store<fmt::format_context>> MultipleStarCatalog;
extern std::map<cse::PlanetarySystemPointer, fmt::dynamic_format_arg_store<fmt::format_context>> ObjectCharacteristics;

void gbuffer_object(cse::PlanetarySystemPointer& System);

#endif
