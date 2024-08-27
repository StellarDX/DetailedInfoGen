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

#ifndef _IG_GSYSTEM_
#define _IG_GSYSTEM_

#include <fmt/args.h>
#include <CSE/PlanSystem.h>

/**
 * @brief Lists of stars, planets and dwarf-planets, used to generate Contents
 */
extern cse::ustringlist PopulationStars, PopulationPlanets, PopulationDwarfPlanets;
extern cse::uint64 NSatellites, NMinorPlanets, NComets;

/**
 * @brief Storage of fmt-args, used to fill tables.
 */
extern fmt::dynamic_format_arg_store<fmt::format_context> TotalResultValues;

/**
 * @brief System Age
 */
extern cse::float64 SystemAge;

/**
 * @brief Combined Spectral Type
 */
extern cse::ustring CombinedSpectralType;

/**
 * @brief Main function
 */
void gbuffer_system(cse::PlanetarySystemPointer& System);

#endif
