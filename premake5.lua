workspace "legba"
    startproject "legba"
    architecture "x86_64"
    configurations {"Debug", "Release"}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "legba"