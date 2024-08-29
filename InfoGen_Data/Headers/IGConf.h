/*

    Copyright (c) StellarDX Astronomy. All rights reserved.

    Module Name:

        IGConf.h

    Abstract:

        Global configurable variables of InfoGen

*/

#pragma once

#ifndef _INFOGEN_CONF
#define _INFOGEN_CONF

#include <filesystem>
#include <boost/program_options.hpp>
#include <fmt/args.h>
#include "CSE/Base/CSEBase.h"

extern boost::program_options::options_description   Options;
extern boost::program_options::variables_map         OptionsVariables;

extern std::filesystem::path                         InputFile;
extern std::filesystem::path                         OutputPath;
extern bool                                          MakeDir;
extern cse::float64                                  OrbitCalculatorEpoch;
extern bool                                          FixOrbitPlane;

extern std::filesystem::path                         Template;
extern std::map<cse::ustring, std::filesystem::path> OutputTemplates;
extern std::filesystem::path                         TemplatePrefix;
extern std::vector<std::filesystem::path>            InstallDirectories;

extern std::map<cse::ustring, cse::ustring>          StaticStrings;

extern std::map<cse::ustring, std::filesystem::path> OutputTemplates;

std::string LoadTemplateProfile(std::string Index);

#endif
