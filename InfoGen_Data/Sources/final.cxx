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

#include "final.hxx"
#include "IGConf.h"

using namespace std;

fmt::dynamic_format_arg_store<fmt::format_context> GeneralValues;
cse::ustring FinalString;

string MainTemplate;

void final(cse::PlanetarySystemPointer& System)
{
    cse::CSESysDebug("final", cse::CSEDebugger::INFO, "Merging blocks...");

    GeneralValues.push_back(fmt::arg("MainTitle", System->PObject->Name[0].ToStdString()));
    MainTemplate = LoadTemplateProfile("Main");

    FinalString = fmt::vformat(MainTemplate, GeneralValues);
}
