project "legba"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    targetdir ("%{wks.location}/bin/"..outputdir.."/%{prj.name}")
    objdir ("%{wks.location}/bin-int/"..outputdir.."/%{prj.name}")

    files {
        "src/**.h",
        "src/**.cpp"
    }

    includedirs {
        "src"
    }

    postbuildcommands {
        "{COPYDIR} %[rsc] %[%{cfg.targetdir}/rsc]"
    }

    filter "configurations:Debug"
        symbols "On"

    filter "configurations:Release"
        symbols "Off"
        optimize "Full"
