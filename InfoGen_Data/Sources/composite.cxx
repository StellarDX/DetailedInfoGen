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
#include "composite.hxx"
#include "gbuffer_system.hxx"

#include "IGConf.h"

using namespace std;

fmt::dynamic_format_arg_store<fmt::format_context> ContentValues;
fmt::dynamic_format_arg_store<fmt::format_context> SystemBasicInfoValues;

string ContentTitleFormatString;
string ContentItemFormatString;
string FullContentFormatString;
string GeneralTableFormatString;

void LoadContentFormatStrings()
{
    cse::CSESysDebug("composite", cse::CSEDebugger::INFO, "Loading Content format strings...");

    string TemplateFile = LoadTemplateProfile("ContentFormat");
    if (!TemplateFile.empty())
    {
        istringstream is(TemplateFile);
        getline(is, ContentTitleFormatString);
        getline(is, ContentItemFormatString);
    }

    FullContentFormatString = LoadTemplateProfile("Contents");

    cse::CSESysDebug("composite", cse::CSEDebugger::INFO, "DONE.");
}

void CreateContent()
{
    cse::CSESysDebug("composite", cse::CSEDebugger::INFO, "Creating content...");

    if (!PopulationStars.empty())
    {
        auto TStr = fmt::arg("ContentTitle", StaticStrings.at("ContentStar").ToStdString());
        ContentValues.push_back(fmt::arg("ContentStarsTitle",
            fmt::vformat(ContentTitleFormatString, fmt::make_format_args(TStr))));
        string Content;
        for (auto i : PopulationStars)
        {
            cse::CSESysDebug("composite", cse::CSEDebugger::INFO, "Creating content item: " + i.ToStdString());
            auto IStr = fmt::arg("ContentItem", i.ToStdString());
            Content += vformat(ContentItemFormatString + "\n", fmt::make_format_args(IStr));
        }
        ContentValues.push_back(fmt::arg("ContentStars", Content));
    }
    else
    {
        ContentValues.push_back(fmt::arg("ContentStarsTitle", ""));
        ContentValues.push_back(fmt::arg("ContentStars", ""));
    }

    if (!PopulationPlanets.empty())
    {
        auto TStr = fmt::arg("ContentTitle", StaticStrings.at("ContentPlanet").ToStdString());
        ContentValues.push_back(fmt::arg("ContentPlanetsTitle",
            fmt::vformat(ContentTitleFormatString, fmt::make_format_args(TStr))));
        string Content;
        for (auto i : PopulationPlanets)
        {
            cse::CSESysDebug("composite", cse::CSEDebugger::INFO, "Creating content item: " + i.ToStdString());
            auto IStr = fmt::arg("ContentItem", i.ToStdString());
            Content += vformat(ContentItemFormatString + "\n", fmt::make_format_args(IStr));
        }
        ContentValues.push_back(fmt::arg("ContentPlanets", Content));
    }
    else
    {
        ContentValues.push_back(fmt::arg("ContentPlanetsTitle", ""));
        ContentValues.push_back(fmt::arg("ContentPlanets", ""));
    }

    if (!PopulationDwarfPlanets.empty())
    {
        auto TStr = fmt::arg("ContentTitle", StaticStrings.at("ContentDwarfPlanet").ToStdString());
        ContentValues.push_back(fmt::arg("ContentDPlanetsTitle",
            fmt::vformat(ContentTitleFormatString, fmt::make_format_args(TStr))));
        string Content;
        for (auto i : PopulationDwarfPlanets)
        {
            cse::CSESysDebug("composite", cse::CSEDebugger::INFO, "Creating content item: " + i.ToStdString());
            auto IStr = fmt::arg("ContentItem", i.ToStdString());
            Content += vformat(ContentItemFormatString + "\n", fmt::make_format_args(IStr));
        }
        ContentValues.push_back(fmt::arg("ContentDwarfPlanets", Content));
    }
    else
    {
        ContentValues.push_back(fmt::arg("ContentDPlanetsTitle", ""));
        ContentValues.push_back(fmt::arg("ContentDwarfPlanets", ""));
    }

    cse::CSESysDebug("composite", cse::CSEDebugger::INFO, "Merging content...");
    GeneralValues.push_back(fmt::arg("Contents",
        fmt::vformat(FullContentFormatString, ContentValues)));

    cse::CSESysDebug("composite", cse::CSEDebugger::INFO, "DONE.");
}

void CreateGeneralTable()
{
    cse::CSESysDebug("composite", cse::CSEDebugger::INFO, "Creating general table...");

    SystemBasicInfoValues.push_back(fmt::arg("SystemAge", SystemAge));
    SystemBasicInfoValues.push_back(fmt::arg("StarCount", PopulationStars.size()));
    SystemBasicInfoValues.push_back(fmt::arg("PlanetCount", PopulationPlanets.size()));
    SystemBasicInfoValues.push_back(fmt::arg("DwarfPlanetCount", PopulationDwarfPlanets.size()));
    SystemBasicInfoValues.push_back(fmt::arg("SatelliteCount", NSatellites));
    SystemBasicInfoValues.push_back(fmt::arg("MinorPlanetCount", NMinorPlanets));
    SystemBasicInfoValues.push_back(fmt::arg("CometCount", NComets));
    SystemBasicInfoValues.push_back(fmt::arg("CombinedSpectralType", CombinedSpectralType.ToStdString()));

    GeneralTableFormatString = LoadTemplateProfile("General");
    GeneralValues.push_back(fmt::arg("General",
        fmt::vformat(GeneralTableFormatString, SystemBasicInfoValues)));
}

void composite(cse::PlanetarySystemPointer& System)
{
    LoadContentFormatStrings();
    CreateContent();
    CreateGeneralTable();
}
