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

#include <iostream>
#include <filesystem>
#include <random>
#include <fstream>
#include <boost/program_options.hpp>
#include <CSE/Base.h>
#include <CSE/Parser.h>
#include <CSE/Object.h>
#include <CSE/PlanSystem.h>
#include <fmt/args.h>

#include "Sources/gbuffer_object.hxx"
#include "Sources/gbuffer_object_barycenter.hxx"
#include "Sources/gbuffer_system.hxx"
#include "Sources/composite.hxx"
#include "Sources/composite1.hxx"
#include "Sources/final.hxx"

#include "InfoGen.h"

using namespace std;
using namespace cse;
using namespace std::filesystem;
using namespace boost::program_options;

options_description   Options(_TITLE("Options"));
variables_map         OptionsVariables;

path                  InputFile;
path                  OutputPath = "./IGOutputData";
bool                  MakeDir;
bool                  FixOrbitPlane = 0;

path                  Template = "Markdown";
map<ustring, path>    OutputTemplates;
path                  TemplatePrefix;
vector<path>          InstallDirectories;
HMODULE               ExternalGenerator = nullptr;

path                  Target;
float64               OrbitCalculatorEpoch;

map<ustring, ustring> StaticStrings
{
    // Contents
    {"ContentStar",        "Stars"},
    {"ContentPlanet",      "Planets"},
    {"ContentDwarfPlanet", "Known dwarf planets"},

    // Binary natures
    {"CPMSystem",          "Common proper motion system"},
    {"VisualSystem",       "Visual or interferometric system"},
    {"AstrometricSystem",  "Astrometric system"},
    {"SB1System",          "Single-lined spectroscopic system"},
    {"SB2System",          "Double-lined spectroscopic system"},
    {"EclipsingBinary",    "Eclipsing binary"},
    {"OtherBarycenters",   "Double or multiple objects"},

    // Stellar Classifications
    {"OTypeStar",          "O-type main-sequence star"},
    {"BTypeStar",          "B-type main-sequence star"},
    {"ATypeStar",          "A-type main-sequence star"},
    {"FTypeStar",          "F-type main-sequence star"},
    {"GTypeStar",          "G-type main-sequence star"},
    {"KTypeStar",          "K-type main-sequence star"},
    {"MTypeStar",          "Red dwarf"},
    {"OTypeSubdwarf",      "Subdwarf O star"},
    {"BTypeSubdwarf",      "Subdwarf B star"},
    {"Subdwarf",           "Subdwarf"},
    {"WolfRayetStar",      "Wolf-Rayet star"},
    {"Subgiant",           "Subgiant"},
    {"BlueGiant",          "Blue giant"},
    {"RedGiant",           "Red giant"},
    {"YellowGiant",        "Yellow giant"},
    {"BrightGiant",        "Bright giant"},
    {"BSG",                "Blue supergiant"},
    {"RSG",                "Red supergiant"},
    {"YSG",                "Yellow supergiant"},
    {"Hypergiant",         "Hypergiant"},
    {"CarbonStar",         "Carbon star"},
    {"STypeStar",          "S-type star"},
    {"CNStar",             "CN star"},
    {"CHStar",             "CH star"},
    {"WhiteDwarf",         "White dwarf"},
    {"NeutronStar",        "Pulsar"},
    {"BlackHole",          "Black hole"},
    {"Substellar",         "Substellar Object"},

    // Planetary Classifications
    {"SuperJupiter",       "Super-Jupiter"},      // An astronomical object more massive than the planet Jupiter.
    {"GasGiant",           "Gas giant"},          // A massive planet composed primarily of hydrogen and helium.
    {"SuperNeptune",       "Super-Neptune"},      // A planet that is more massive than the planet Neptune. These planets are generally described as being around 5–7 times as large as Earth with estimated masses of 20–80 ME
    {"IceGiant",           "Ice giant"},          // A giant planet composed mainly of 'ices'—volatile substances heavier than hydrogen and helium, such as water, methane, and ammonia—as opposed to 'gas' (hydrogen and helium).
    {"SubNeptune",         "Sub-Neptune"},        // a planet with smaller radius than Neptune even though it may have a larger mass
    {"MiniNeptune",        "Gas dwarf"},          // A low-mass planet composed primarily of hydrogen and helium.
    {"MegaEarth",          "Mega-Earth"},         // Proposed neologism for a massive terrestrial exoplanet that is at least ten times the mass of Earth
    {"SuperEarth",         "Super-Earth"},        // An extrasolar planet with a mass higher than Earth's, but substantially below the mass of the Solar System's smaller gas giants Uranus and Neptune, which are 14.5 and 17.1 Earth masses respectively.
    {"SubEarth",           "Sub-Earth"},          // A classification of planets "substantially less massive" than Earth and Venus.
    {"EccentricJupiter",   "Eccentric Jupiter"},  // A gas giant that orbits its star in an eccentric orbit.
    {"HotJupiter",         "Hot Jupiter"},        // A class of extrasolar planets whose characteristics are similar to Jupiter, but that have high surface temperatures because they orbit very close—between approximately 0.015 and 0.5 AU (2.2×106 and 74.8×106 km)—to their parent stars, whereas Jupiter orbits its parent star (the Sun) at 5.2 AU (780×106 km), causing low surface temperatures.
    {"HotNeptune",         "Hoptune"},            // An extrasolar planet in an orbit close to its star (normally less than one astronomical unit away), with a mass similar to that of Uranus or Neptune.
    {"Carbonia",           "Carbon planet"},      // A theoretical terrestrial planet that composed primarily of graphite, diamond or moissanite could form if protoplanetary discs are carbon-rich and oxygen-poor.
    {"Chthonia",           "Helium planet"},      // A theoretical planet that may form via mass loss from a low-mass white dwarf. Helium planets are predicted to have roughly the same diameter as hydrogen–helium planets of the same mass.
    {"Hycean",             "Hycean planet"},      // A hypothetical type of habitable planet described as a hot, water-covered planet with a hydrogen-rich atmosphere.
    {"IceWorld",           "Ice planet"},         // A theoretical planet with an icy surface and consists of a global cryosphere.
    {"Ferria",             "Iron planet"},        // A theoretical planet that consists primarily of an iron-rich core with little or no mantle.
    {"LavaPlanet",         "Lava planet"},        // A theoretical terrestrial planet with a surface mostly or entirely covered by molten lava.
    {"Oceania",            "Ocean planet"},       // A theoretical planet which has a substantial fraction of its mass made of water.
    {"SilicatePlanet",     "Silicate planet"},    // A terrestrial planet that is composed primarily of silicate rocks. All four inner planets in the Solar System are silicon-based.
    {"Terra",              "Terrestrial planet"}, // Also known as a telluric planet or rocky planet. A planet that is composed primarily of carbonaceous or silicate rocks or metals.
    {"GasGiantClassI",     "Class I: Ammonia clouds"},
    {"GasGiantClassII",    "Class II: Water clouds"},
    {"GasGiantClassIII",   "Class III: Cloudless"},
    {"GasGiantClassIV",    "Class IV: Alkali metals"},
    {"GasGiantClassV",     "Class V: Silicate clouds"},
};

LPCSTR Usage = _TITLE("Usage\n")
_TXT(R"(
  InfoGen [options] <source-file>
  InfoGen [options] -S <source-file> -B <path-to-output>

Specify a source file to (re-)generate planetary system report with
variable templates.
)");

#if defined (WIN32) | defined (WIN64)
class InfoGenTextCoder : public __StelCXX_UString_Codec
{
    // __StelCXX_UString_Codec interface
public:
    uint16_t MCodePage;

    uint16_t CodePage() override {return MCodePage;}

    ustring Decode(string str) const override
    {
        int len = MultiByteToWideChar(MCodePage, 0, str.c_str(), -1, nullptr, 0);
        wchar_t* wstr = new wchar_t[len];
        memset(wstr, 0, len);
        MultiByteToWideChar(MCodePage, 0, str.c_str(), -1, wstr, len);
        ustring ustr(wstr);
        delete[] wstr;
        return ustr;
    }

    std::string Encode(ustring str) const override
    {
        int len = WideCharToMultiByte(MCodePage, 0, str.c_str(), -1, nullptr, 0, nullptr, nullptr);
        char* cstr = new char[len + 1];
        memset(cstr, 0, len + 1);
        WideCharToMultiByte(MCodePage, 0, str.c_str(), -1, cstr, len, nullptr, nullptr);
        string rstr(cstr);
        delete[] cstr;
        return rstr;
    }
}InputCoder, OutputCoder;
#else
using InfoGenTextCoder = __StelCXX_UString_Codec_65001;
InfoGenTextCoder InputCoder, OutputCoder;
#endif

void InfoGenOptionInit()
{
    // 只能说Boost那个中文支持简直史一样
    Options.add_options()
    ("input,S", value<std::string>()->value_name("<source-file>"), _TXT("Specify input file")
        _NOTE(R"((Note: The "source file" here refers to the complete system file exported by SE, the export method is select a system and go to it, then return to the main menu and select Tools -> Export System Script, the exported system is the complete system.))"))
    ("output,B", value<std::string>()->value_name("<path-to-output>"), _TXT("Specify output directory"))
    ("input-codepage", value<uint16_t>()->value_name("<encod>")
        ->default_value(65001), _TXT("The encoding of the input file (code page), e.g. --input-codepage=65001 means using UTF-8 encoding. "
        _WARN("This feature is only available on Windows.")))
    ("output-codepage", value<uint16_t>()->value_name("<encod>")
        ->default_value(65001), _TXT("Encoding of the output file (code page). "
        _WARN("This feature is only available on Windows.")))
    ("mkdir", _TXT("Automatically creates the output directory if it doesn't exist"))
    ("seed", value<uint32_t>()->value_name("<seed>")
        ->default_value(random_device()()), _TXT("Seed of random number generator, randomly generated if not specified, ranging in 0-4294967295"))
    ("epoch", value<float64>()->value_name("<JD>")
        ->default_value(GetJDFromSystem()), _TXT("Epoch of the orbit tracker, defaults value is current system time"))
    ("fix-orbit-plane,f", _TXT("Correction of orbital plane, " _WARN("TODO")))
    ("generator,G", value<std::string>()->value_name("<generator>")
        ->default_value("Markdown"), _TXT("Specify generator, default is Markdown"))
    ("list-variables", _TXT("Show variables"))
    ("verbose,v", _TXT("Show log"))
    ("help,h", _TXT("Show this page"));
}

void InfoGenVariableList()
{
    cout << _TITLE("Variable List\n")
_NOTE(R"(  [Name]               [Range]         [Type]                [Note])")
_TXT(R"(
  MainTitle            [Main]          (Raw string)          System name
  Content              [Main]          (Preprocessed string) Content
  General              [Main]          (Preprocessed string) System general information table
  ContentTitle         [ContentFormat] (Raw string)          Content Title
  ContentItem          [ContentFormat] (Raw string)          Content Item
  ContentStarsTitle    [Contents]      (Preprocessed string) Star content title
  ContentStars         [Contents]      (Preprocessed string) Star content item list
  ContentPlanetsTitle  [Contents]      (Preprocessed string) Planet content title
  ContentPlanets       [Contents]      (Preprocessed string) Planet content item list
  ContentDPlanetsTitle [Contents]      (Preprocessed string) Dwarf planet content title
  ContentDwarfPlanets  [Contents]      (Preprocessed string) Dwarf planet content item list
  SystemAge            [General]       (Real number)         System age
  StarCount            [General]       (Integer number)      Number of stars
  PlanetCount          [General]       (Integer number)      Number of planets
  DwarfPlanetCount     [General]       (Integer number)      Number of dwarf planets
  SatelliteCount       [General]       (Integer number)      Number of satellites
  MinorPlanetCount     [General]       (Integer number)      Number of minor planets
  CometCount           [General]       (Integer number)      Number of comets
  CombinedSpectralType [General]       (Raw string)          Combined spectral type
  MSC                  [Main]          (Preprocessed string) Multiple star table
  MSCItems             [MSC]           (Preprocessed string) Items of MSC table
  MSCPrimary           [MSCItems]      (Raw string)          System primary component
  MSCSecondary         [MSCItems]      (Raw string)          System secondary component
  MSCParent            [MSCItems]      (Raw string)          Parent body
  MSCType              [MSCItems]      (Raw string)          Binary type
  MSCPer               [MSCItems]      (Real number)         Period
  MSCPerUnit           [MSCItems]      (Raw string)          Unit of period
  MSCSep               [MSCItems]      (Real number)         Separation
  MSCSepUnit           [MSCItems]      (Raw string)          Unit of separation
  MSCSp1               [MSCItems]      (Raw string)          Primary spectral type
  MSCSp2               [MSCItems]      (Raw string)          Secondary spectral type
  MSCMass1             [MSCItems]      (Real number)         Primary mass in Solar mass
  MSCMass2             [MSCItems]      (Real number)         Secondary mass in Solar mass
  ObjectTables         [Main]          (Preprocessed string) Object tables
  Name                 [Object]        (Raw string)          Object name
  Type                 [Object]        (Raw string)          Object type
  OrbitEpoch           [Object]        (Real number)         Epoch in JD
  OrbitRefSystem       [Object]        (Raw string)          Orbit reference system
  OrbitPrimary         [Object]        (Raw string)          Primary object, only used in star systems
  OrbitCompanion       [Object]        (Raw string)          Companion object, only used in star systems
  OrbitPeriod          [Object]        (Real number)         Orbital Period, default unit is seconds, Days and Years are also available.
  OrbitAphelion        [Object]        (Real number)         Aphelion, default unit is metres, Km and AU are also available.
  OrbitPerihelion      [Object]        (Real number)         Perihelion, default unit is metres, Km and AU are also available.
  OrbitSemiMajorAxis   [Object]        (Real number)         Orbital Semi-major axis, default unit is metres, Km and AU are also available.
  OrbitEccentricity    [Object]        (Real number)         Orbital Eccentricity
  OrbitInclination     [Object]        (Real number)         Orbital Inclination
  OrbitAscendingNode   [Object]        (Real number)         Orbital AscendingNode
  OrbitArgOfPericenter [Object]        (Real number)         Orbital ArgOfPericenter
  OrbitMeanAnomaly     [Object]        (Real number)         Orbital MeanAnomaly
  AbsMagn              [Star]          (Real number)         Absolute Bolometric Magnitude
  SpecType             [Star]          (Raw string)          Stellar Classification
  Metallicity          [Star]          (Real number)         Metallicity
  Obliquity            [Object]        (Real number)         Rotational Obliquity
  RotationPeriod       [Object]        (Real number)         Sidereal Rotation Period, default unit is seconds, Hours and Days are also available.
  RotationVelocity     [Object]        (Real number)         Rotational Velocity in m/s
  EquatorialRadius     [Object]        (Real number)         Equatorial Radius, default unit is metres, Km, Earth, Jup and Sun are also available.
  PolarRadius          [Object]        (Real number)         Polar Radius, default unit is metres, Km, Earth, Jup and Sun are also available.
  MeanRadius           [Object]        (Real number)         Mean Radius, default unit is metres, Km, Earth, Jup and Sun are also available.
  DimensionsX          [Object]        (Real number)         X-axis Dimension in metres
  DimensionsY          [Object]        (Real number)         Y-axis Dimension in metres
  DimensionsZ          [Object]        (Real number)         Z-axis Dimension in metres
  Flattening           [Object]        (Real number)         Flattening
  ECircumference       [Object]        (Real number)         Equatorial Circumference
  MCircumference       [Object]        (Real number)         Meridional Circumference
  SurfaceArea          [Object]        (Real number)         Surface Area in m^2
  Volume               [Object]        (Real number)         Volume in m^3
  Mass                 [Object]        (Real number)         Mass, default unit is Kg, Earth, Jup and Sun are also available.
  AverageDensity       [Object]        (Real number)         Average Density in Kg/m^3
  Age                  [Object]        (Real number)         Age in years
  SurfaceGravity       [Object]        (Real number)         Surface Gravity in m/s^2
  EscapeVelocity       [Object]        (Real number)         Escape Velocity in m/s
  SynRotationPeriod    [Planet]        (Real number)         Synodic Rotation Period, default unit is seconds, Hours and Days are also available.
  AlbedoBond           [Planet]        (Real number)         Bond Albedo
  AlbedoGeom           [Planet]        (Real number)         Geometric Albedo
  Temperature          [Object]        (Real number)         Temperature, default unit is Kelvin, Celsius is also available.
  Luminosity           [Star]          (Real number)         Visual Luminosity, default unit is Watts, Sun is also available.
  LumBol               [Star]          (Real number)         Bolometric Luminosity, default unit is Watts, Sun is also available.
  SatelliteTable       [Planet]        (Preprocessed string) Satellite table
  SatelliteTableItems  [SateTable]     (Preprocessed string) Satellite table items
  SatellitePageSize    [SateTable]     (Integer number)      Satellite table page size
  SatelliteTotalPages  [SateTable]     (Integer number)      Satellite table total pages

Type "{Variable}" to insert variables into template files.
Some of real-number type variables support different units, use "TypeUnit" to switch unit. For example, variable "OrbitPeriod"'s default unit is second, if you want to switch unit to days, type "OrbitPeriodDays".
)");
}

void InfoGenOptionParse(int argc, char const* const* argv)
{
    try
    {
        positional_options_description SourceDir;
        SourceDir.add("input", -1);
        basic_parsed_options Args = command_line_parser(argc, argv)
            .options(Options)
            .positional(SourceDir)
            .run();
        store(Args, OptionsVariables);
    }
    catch (const exception& e)
    {
        CSESysDebug("InfoGen", CSEDebugger::WARNING,
            fmt::format(_WARN_COLOR "{}" "\033[39m", e.what()));
        throw runtime_error(e.what());
    }
}

void InfoGenChkDir()
{
    CSESysDebug("InfoGen", CSEDebugger::INFO, "Checking directories...");

    if (OptionsVariables.count("input"))
    {
        string File = OptionsVariables.at("input").as<string>();
        CSESysDebug("InfoGen", CSEDebugger::INFO,
            fmt::format("input = {}", File));
        try {InputFile = canonical(File);}
        catch (const exception& e)
        {
            CSESysDebug("InfoGen", CSEDebugger::FATAL,
                fmt::format(_FATAL("找不到 {}: 没有那个文件\n{}"), File, e.what()));
            throw runtime_error(e.what());
        }
    }
    else
    {
        CSESysDebug("InfoGen", CSEDebugger::FATAL, _FATAL("fatal error: no input files"));
        throw runtime_error("no input files");
    }

    if (OptionsVariables.count("output"))
    {
        string ErrorStr;
        string Path = OptionsVariables.at("output").as<string>();
        CSESysDebug("InfoGen", CSEDebugger::INFO,
            fmt::format("output = {}", Path));
        try
        {
            OutputPath = canonical(Path);
            if (!is_directory(OutputPath)) {ErrorStr = fmt::format("{}不是目录", Path);}
        }
        catch (const exception& e)
        {
            ErrorStr = e.what();
        }

        if (!ErrorStr.empty())
        {
            CSESysDebug("InfoGen", CSEDebugger::ERROR,
                fmt::format(_ERROR("找不到 {}: 没有那个目录\n{}"), Path, ErrorStr));
            OutputPath = canonical("./IGOutputData");
            CSESysDebug("InfoGen", CSEDebugger::ERROR,
                fmt::format(_ERROR("输出目录已重定向到{}"), OutputPath.string()));
        }
    }
    else
    {
        OutputPath = canonical("./IGOutputData");
        CSESysDebug("InfoGen", CSEDebugger::INFO,
            fmt::format("output = {}", OutputPath.string()));
    }
}

void InfoGenOptionLoad()
{
    CSESysDebug("InfoGen", CSEDebugger::INFO, "Parsing options...");

    #if defined (WIN32) | defined (WIN64)
    if (OptionsVariables.count("input-codepage"))
    {
        InputCoder.MCodePage = OptionsVariables.at("input-codepage").as<uint16_t>();
        CSESysDebug("InfoGen", CSEDebugger::INFO,
            fmt::format("input-codepage = {}", InputCoder.MCodePage));
    }

    if (OptionsVariables.count("output-codepage"))
    {
        OutputCoder.MCodePage = OptionsVariables.at("output-codepage").as<uint16_t>();
        CSESysDebug("InfoGen", CSEDebugger::INFO,
            fmt::format("output-codepage = {}", OutputCoder.MCodePage));
    }
    #endif

    if (OptionsVariables.count("seed"))
    {
        random.seed(OptionsVariables.at("seed").as<uint32_t>());
        CSESysDebug("InfoGen", CSEDebugger::INFO,
            fmt::format("seed = {}", random.seed()));
    }

    // Precision can be specified in tags in templates
    /*if (OptionsVariables.count("precision"))
    {
        Precision = OptionsVariables.at("precision").as<uint16_t>();
        CSESysDebug("InfoGen", CSEDebugger::INFO,
            fmt::format("precision = {}", Precision));
    }*/

    if (OptionsVariables.count("epoch"))
    {
        OrbitCalculatorEpoch = OptionsVariables.at("epoch").as<float64>();
        CSESysDebug("InfoGen", CSEDebugger::INFO,
            fmt::format("epoch = {}", OrbitCalculatorEpoch));
    }

    if (OptionsVariables.count("fix-orbit-plane"))
    {
        FixOrbitPlane = 1;
    }

    if (OptionsVariables.count("generator"))
    {
        Template = OptionsVariables.at("generator").as<std::string>();
    }
}

void LoadStaticStrings(const scenario::SharedTablePointer& Table)
{
    auto it = Table->Get().begin();
    auto end = Table->Get().end();
    for (; it != end; ++it)
    {
        ustring Value;
        it->Value.front().GetQualified(&Value);
        StaticStrings[it->Key] = Value;
    }
}

void LoadTemplate()
{
    CSESysDebug("InfoGen", CSEDebugger::INFO, "Loading output Template...");
    scenario::SharedTablePointer TemplateConfigTable;
    try
    {
        string TemplateStr = Template.string();
        Template = canonical(fmt::vformat(_DATA_DIR("WebResources/{0}/{0}.cfg"), fmt::make_format_args(TemplateStr)));
        CSESysDebug("InfoGen", CSEDebugger::INFO, "Config file: " + Template.string());
        TemplatePrefix = Template.parent_path();
        TemplateConfigTable = ParseFile(Template);
    }
    catch (const exception& e)
    {
        CSESysDebug("SCStream", CSEDebugger::FATAL,
            fmt::format(_FATAL("{}"), e.what()));
        throw runtime_error(e.what());
    }

    ustring TargetFmtStr;
    __scstream_table_helpers::__Get_Value_From_Table(&TargetFmtStr, TemplateConfigTable, L"Target", ustring("a.md"));
    Target = TargetFmtStr.ToStdWString();

    auto ProfileList = __scstream_table_helpers::__Find_Table_From_List(TemplateConfigTable, L"Profiles");
    if (ProfileList != TemplateConfigTable->Get().end())
    {
        for (auto i : ProfileList->SubTable->Get())
        {
            ustring Value;
            i.Value.front().GetQualified(&Value);
            OutputTemplates.insert({i.Key, Value.ToStdWString()});
        }
    }

    auto InstallDirList = __scstream_table_helpers::__Find_Table_From_List(TemplateConfigTable, L"Install");
    if (InstallDirList != TemplateConfigTable->Get().end() && !InstallDirList->Value.empty())
    {
        for (auto i : InstallDirList->Value.front().Value)
        {
            InstallDirectories.push_back(i.ToStdString());
        }
    }

    auto StaticStringTable = __scstream_table_helpers::__Find_Table_From_List(TemplateConfigTable, L"StaticStrings");
    if (StaticStringTable != TemplateConfigTable->Get().end())
    {
        LoadStaticStrings(StaticStringTable->SubTable);
    }

    auto CustomGenerator = __scstream_table_helpers::__Find_Table_From_List(TemplateConfigTable, L"CustomGenerator");
    if (CustomGenerator != TemplateConfigTable->Get().end())
    {
        ustring Value;
        CustomGenerator->Value.front().GetQualified(&Value);
        try
        {
            path SOPath = canonical(TemplatePrefix + "/" + Value.ToStdString());
            ExternalGenerator = LoadLibrary(SOPath.string().c_str());
        }
        catch (const exception& e)
        {
            CSESysDebug("SCStream", CSEDebugger::FATAL,
                fmt::format(_FATAL("{}"), e.what()));
            throw runtime_error(e.what());
        }
    }

    CSESysDebug("InfoGen", CSEDebugger::INFO, "DONE.");
}

string LoadTemplateProfile(string Index)
{
    string Path;
    try {Path = OutputTemplates.at(Index).string();}
    catch (...) {return "";}
    ifstream fin(canonical(TemplatePrefix + "/" + Path));
    stringstream Buf;
    Buf << fin.rdbuf();
    fin.close();
    return Buf.str();
}

PlanetarySystemPointer LoadSystem(path Input)
{
    CSESysDebug("InfoGen", CSEDebugger::INFO, "Loading Planetary System...");
    ifstream fin(Input);
    ISCStream sc(fin);
    sc.SetDecoder(InputCoder);
    scenario::SharedTablePointer SCTable;
    try {SCTable = sc.Parse();}
    catch (const exception& e)
    {
        CSESysDebug("SCStream", CSEDebugger::FATAL,
            fmt::format(_FATAL("SC语法错误：{}"), e.what()));
        throw runtime_error(e.what());
    }

    vector<Object> ObjectList;
    for (const auto& i : SCTable->Get())
    {
        ObjectList.push_back(GetObjectFromKeyValue(i));
    }
    CSESysDebug("InfoGen", CSEDebugger::INFO, fmt::format("{} Objects loaded", ObjectList.size()));

    return MakeSystem(ObjectList);
}

void InstallTarget()
{
    CSESysDebug("InfoGen", CSEDebugger::INFO, "Installing target...");

    CSESysDebug("InfoGen", CSEDebugger::INFO, "Installing: " + Target.string());
    ofstream of(OutputPath + "/" + Target);
    of << FinalString.Encode(OutputCoder);
    of.close();

    for (auto i : InstallDirectories)
    {
        CSESysDebug("InfoGen", CSEDebugger::INFO, "Installing: " + i.string());
        std::filesystem::copy(TemplatePrefix + "/" + i, OutputPath + + "/" + i,
            std::filesystem::copy_options::recursive);
    }
}

#ifdef __cplusplus
_EXTERN_C
#endif

void DefaultGeneratorMain(PlanetarySystemPointer& System)
{
    gbuffer_system(System);
    gbuffer_object(System);
    composite(System);
    composite1(System);
    final(System);
}

void (*GeneratorMain)(cse::PlanetarySystemPointer& System) = DefaultGeneratorMain;

#define _MAIN_FUNC_BEGIN try {
#define _MAIN_FUNC_END } catch (const exception& e) \
{\
    CSESysDebug("Main", CSEDebugger::FATAL,\
        fmt::format(_FATAL("Unexpected fatal error: {}"), e.what()));\
    return;\
}

IGEXPORT void IGCALL InfoGenMain(int argc, char const* const* argv)
{
    scenario::CSECatDebug.__LogLevel = 1;

    if (argc == 1)
    {
        cout << Usage << '\n';
        return;
    }

    InfoGenOptionInit();

    _MAIN_FUNC_BEGIN

    InfoGenOptionParse(argc, argv);
    if (OptionsVariables.count("help"))
    {
        cout << Options << '\n';
        return;
    }
    if (OptionsVariables.count("list-variables"))
    {
        InfoGenVariableList();
        return;
    }
    if (OptionsVariables.count("verbose"))
    {
        CSESysDebug.__LogLevel = 2;
    }

    InfoGenChkDir();
    InfoGenOptionLoad();
    LoadTemplate();

    PlanetarySystemPointer System = LoadSystem(InputFile);

    if (ExternalGenerator)
    {
        GeneratorMain = decltype(GeneratorMain)(GetProcAddress(ExternalGenerator, TEXT("GenMain")));
    }
    else {GeneratorMain = DefaultGeneratorMain;}

    GeneratorMain(System);

    if (ExternalGenerator)
    {
        FreeLibrary(ExternalGenerator);
    }

    auto TitleArg = fmt::arg("SystemName", System->PObject->Name[0].ToStdString());
    Target = fmt::vformat(Target.string(), fmt::make_format_args(TitleArg));

    InstallTarget();

    CSESysDebug("InfoGen", CSEDebugger::INFO, "Generating done.");

    _MAIN_FUNC_END
}

#ifdef __cplusplus
_END_EXTERN_C
#endif
