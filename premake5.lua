-- premake5.lua
workspace "SoftwareRenedrer"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "SoftwareRenedrer"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
include "Walnut/WalnutExternal.lua"

include "SoftwareRenedrer"