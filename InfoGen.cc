#include <format>
#include <iostream>
#include <CSE/Base.h>

// Windows API必须最后包含
#include "InfoGen_Data/Headers/IGDef.h"

using namespace std;
using namespace cse;

LPCSTR Copyright = R"(
SpaceEngine 0.990 详细信息生成器
版权所有 (C) StellarDX Astronomy.
这是自由软件；请参考源代码的版权声明。本软件不提供任何保证，甚至不会包括
可售性或适用于任何特定目的的保证。
编译器版本：)";

int main(int argc, char const* const* argv)
{
    IGFORCEU8;
    //cse::CSESysDebug.__LogLevel = 2; // Log everything

    cout << Copyright << COMPILER_VERSION << "\n\n";
    // Load Main function
    cse::CSESysDebug("Main", cse::CSEDebugger::INFO, "Loading engine...");
    HMODULE InfoGen = LoadLibrary("./InfoGen_Data/InfoGen.so");
    if (!InfoGen)
    {
        cse::CSESysDebug("Main", cse::CSEDebugger::FATAL,
            _FATAL("Unable to load engine, shared objects are missing?"));
        exit(0xBAD50);
    }
    cse::CSESysDebug("Main", cse::CSEDebugger::INFO,
        format("InfoGen HInstance is located at 0x{:X}", uint64(InfoGen)));

    MainFuncType MainFunc = MainFuncType(GetProcAddress(InfoGen, TEXT("InfoGenMain")));
    if (!MainFunc)
    {
        cse::CSESysDebug("Main", cse::CSEDebugger::FATAL,
            _FATAL("Unable to locate main function."));
        FreeLibrary(InfoGen);
        exit(0xBAADF00D);
    }
    cse::CSESysDebug("Main", cse::CSEDebugger::INFO, "Starting process...");

    MainFunc(argc, argv);

    cse::CSESysDebug("Main", cse::CSEDebugger::INFO, "InfoGen end of work, destorying engine...");
    FreeLibrary(InfoGen);

    return 0;
}
