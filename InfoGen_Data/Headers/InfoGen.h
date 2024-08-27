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

/**
 * @file InfoGen.h
 * @brief The main header file of InfoGen
 */

#pragma once

#ifndef _INFOGEN_
#define _INFOGEN_

#include "IGDef.h"
#include "IGConf.h"

#ifdef __cplusplus
_EXTERN_C
#endif

/**
 * @brief InfoGen Main function
 * @param argc - Option count
 * @param argv - Options
 */
IGEXPORT void IGCALL InfoGenMain(int argc, char const* const* argv);

#ifdef __cplusplus
_END_EXTERN_C
#endif

#endif
