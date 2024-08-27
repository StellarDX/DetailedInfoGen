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

#include "Sources/composite.hxx"
#include "Sources/final.hxx"
#include "Sources/gbuffer_system.hxx"

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

path                  Template = "Markdown";
map<ustring, path>    OutputTemplates;
path                  TemplatePrefix;
vector<path>          InstallDirectories;

path                  Target;

map<ustring, ustring> StaticStrings
{
    {"ContentStar",        "Stars"},
    {"ContentPlanet",      "Planets"},
    {"ContentDwarfPlanet", "Known dwarf planets"},
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
    ("input,S", value<std::string>()->value_name("<source-file>"), _TXT("指定输入的源文件 ")
_NOTE(R"((注:这里的"源
文件"指SE导出的完整系统
文件，导出方法是先选中一
个系统并前往，返回主菜单
选择工具->导出系统脚本，
导出系统即为完整系统。))"))
    ("output,B", value<std::string>()->value_name("<path-to-output>"), _TXT("指定输出目录"))
    ("input-codepage", value<uint16_t>()->value_name("<encod>")
        ->default_value(65001), _TXT("输入文件的编码\n(代码页)，如UTF-8就写\n--input-codepage=65001，\n"
        _WARN("此功能仅Windows下\n可用")))
    ("output-codepage", value<uint16_t>()->value_name("<encod>")
        ->default_value(65001), _TXT("输出文件的编码\n(代码页)"
        _WARN("此功能仅Windows下可用")))
    ("mkdir", _TXT("如果输出目录不存在就自动创建"))
    ("seed", value<uint32_t>()->value_name("<seed>")
        ->default_value(random_device()()), _TXT("随机数生成器的种子，如果不指定则随机生成，范围是0-4294967295"))
    ("show-variables", _TXT("显示变量"))
    ("verbose,v", _TXT("显示日志"))
    ("help,h", _TXT("显示帮助"));
}

void InfoGenVariableList()
{
    cout << _TITLE("Variable List")
_TXT(R"(
  [Name]               [Source]    [Type]                [Note]
  MainTitle            [final]     (Raw string)          System name
  Content              [final]     (Preprocessed string) Content
  ContentTitle         [composite] (Raw string)          Content Title
  ContentItem          [composite] (Raw string)          Content Item
  ContentStarsTitle    [composite] (Preprocessed string) Star content title
  ContentStars         [composite] (Preprocessed string) Star content item list
  ContentPlanetsTitle  [composite] (Preprocessed string) Planet content title
  ContentPlanets       [composite] (Preprocessed string) Planet content item list
  ContentDPlanetsTitle [composite] (Preprocessed string) Dwarf planet content title
  ContentDwarfPlanets  [composite] (Preprocessed string) Dwarf planet content item list
  General              [final]     (Preprocessed string) System general information table
  SystemAge            [composite] (Real number)         System age
  StarCount            [composite] (Integer number)      Number of stars
  PlanetCount          [composite] (Integer number)      Number of planets
  DwarfPlanetCount     [composite] (Integer number)      Number of dwarf planets
  SatelliteCount       [composite] (Integer number)      Number of satellites
  MinorPlanetCount     [composite] (Integer number)      Number of minor planets
  CometCount           [composite] (Integer number)      Number of comets
  CombinedSpectralType [composite] (Raw string)          Combined spectral type
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

    CSESysDebug("InfoGen", CSEDebugger::INFO, "DONE.");
}

string LoadTemplateProfile(string Index)
{
    string Path = OutputTemplates.at(Index).string();
    ifstream fin(canonical(TemplatePrefix + "/" + Path));
    stringstream Buf;
    Buf << fin.rdbuf();
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
    if (OptionsVariables.count("show-variables"))
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

    gbuffer_system(System);
    composite(System);
    final(System);

    auto TitleArg = fmt::arg("SystemName", System->PObject->Name[0].ToStdString());
    Target = fmt::vformat(Target.string(), fmt::make_format_args(TitleArg));

    InstallTarget();

    CSESysDebug("InfoGen", CSEDebugger::INFO, "Generating done.");

    _MAIN_FUNC_END
}

#ifdef __cplusplus
_END_EXTERN_C
#endif
