# gmod-imgui

![GitHub repo size](https://img.shields.io/github/repo-size/GrayWolf64/gmod-imgui)

This is still Work-In-Progress.

Other Tools Used: 
1. [Trailing Whitespace Visualizer](https://marketplace.visualstudio.com/items?itemName=MadsKristensen.TrailingWhitespaceVisualizer).
2. [Intel® oneAPI DPC++/C++ Compiler](https://www.intel.com/content/www/us/en/developer/tools/oneapi/dpc-compiler.html) (Not using it at the present because it causes GMod (x86-x64 branch) to fail to load the binary module when built x64 Release).

Use MS Visual Studio 2022 (MS C++ Compiler) to build x64 Release. Then put the binary into garrysmod/lua/bin, then require it ("imgui"). So a demo window will show up once you have entered a sp game.

Third-Party Project(s) Used:
1. [Dear ImGui](https://github.com/ocornut/imgui)
2. [Detours](https://github.com/microsoft/detours)
3. [DXHooker](https://github.com/MoxPoto/DXHooker)
