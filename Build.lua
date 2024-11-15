-- premake5.lua
ProjectName = "CatmullRomAssignement"
AppName = "App"

workspace (ProjectName)
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject (AppName)

   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

OutputDir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

group ""
include (AppName .. "/Build-" .. AppName ..".lua")