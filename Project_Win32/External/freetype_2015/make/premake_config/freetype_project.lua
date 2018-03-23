
-- Solution
freetype_solution = function()
if  _ACTION=="vs2008" then
	solution "freetype_2008"
elseif _ACTION=="vs2010" then
	solution "freetype_2010"
elseif _ACTION=="vs2012" then
	solution "freetype_2012"
elseif _ACTION=="vs2013" then
	solution "freetype_2013"
elseif _ACTION=="vs2015" then
	solution "freetype_2015"
else
	solution "freetype"
end
	
	if _ACTION=="xcode3" or _ACTION=="vs2012wp8" or _ACTION=="vs2012w8" or _ACTION=="vs2013w8" or _ACTION=="vs2013wrt" then
		configurations { "Debug",
						 "Release"
						}
	else
		configurations { 
						 "Debug - Multithreaded",
						 "Debug - Multithreaded DLL",
						 "Release - Multithreaded",
						 "Release - Multithreaded DLL"
						 }
	end
	
	location ( getFreeTypeLocation("../glitch/source/freetype/builds/"))
	
	-- Platforms
	if _ACTION=="xcode3" then
        if _OPTIONS["iphone"] then
            platforms { "iPhone" }
        elseif _OPTIONS["tvos"] then
            platforms { "tvOS" }
        else
            platforms { "x32" }
        end
	elseif _ACTION=="vs2012" or _ACTION=="vs2013" or _ACTION=="vs2015" then
        platforms { "x32", "x64" }
	elseif _ACTION=="vs2012wp8" then
		platforms { "x32", "ARM" }
	elseif _ACTION=="vs2012w8" or _ACTION=="vs2013w8" or _ACTION=="vs2013wrt" then
		platforms { "x32", "x64", "ARM" }
	end
	
	if _ACTION=="vs2013wrt" then
		folder "freetype"
		folderUuid "A43F54E8-B804-4F2C-B517-0963941F6070"
		
		freetype_project_wrt("Windows")
		freetype_project_wrt("WindowsPhone")
		freetype_shared_items_project_wrt()
	else
		freetype_project()
	end
end

freetype_project = function()

if  _ACTION=="vs2008" then
	project "freetype_2008"
elseif _ACTION=="vs2010" then
	project "freetype_2010"
elseif _ACTION=="vs2012" or _ACTION=="vs2012w8" then
	project "freetype_2012"
elseif _ACTION=="vs2013"  or _ACTION=="vs2013w8" then
	project "freetype_2013"
elseif _ACTION=="vs2015"  or _ACTION=="vs2015w8" then
	project "freetype_2015"
else
	project "freetype"
end

	kind "StaticLib"
	language "C++"
	
	location ( getFreeTypeLocation("../glitch/source/freetype/builds/"))
	local platform = _ACTION
	
	freetype_project_get_common_files()
	
	if _ACTION == "xcode3" then -- Xcode3 specific stuff
	
		if _OPTIONS["iphone"] then
				platform = platform .. "_iphone"
		elseif _OPTIONS["tvos"] then
				platform = platform .. "_tvos"
		end
		   
		-- Config files
		files { "../glitch/source/freetype/make/premake_config/**.h",
			"../glitch/source/freetype/make/premake_config/**.xcconfig"}
		excludes { "../glitch/source/freetype/make/premake_config/propertysheets/**"}
		if  _OPTIONS["iphone"] or _OPTIONS["tvos"] then
			excludes { "../glitch/source/freetype/make/premake_config/xcconfigsosx/**"}
		else
			excludes { "../glitch/source/freetype/make/premake_config/xcconfigs/**"}
		end
		
		targetdir ("../glitchConfig/bin/freetype/premake_" .. platform  .. "/$(CONFIGURATION)")
		objdir ("../glitchConfig/build/freetype/premake_" .. platform)
				   
		-- Configurations
		-- Debug
		configuration "Debug"
			xcodeconfigreferenceproject "debug-project.xcconfig"
			xcodeconfigreferencetarget "debug-freetype.xcconfig"
			
			
		-- Release
		configuration "Release"	
			xcodeconfigreferenceproject "release-project.xcconfig"
			xcodeconfigreferencetarget "release-freetype.xcconfig"
	
	else -- Vs specific stuff
	
		uuid "ADD22788-E9F2-8745-A2DC-4078E9A91BA7"
				
		if _ACTION == "vs2010" then 
			defines { "%(PreprocessorDefinitions)" }
		end
				
		if _ACTION == "vs2008" then
			targetdir ("../glitchConfig/bin/freetype/$(SolutionName)/premake_" .. platform .. "/$(ConfigurationName)")
		elseif _ACTION == "vs2010" then
			targetdir ("../glitchConfig/bin/freetype/$(SolutionName)/premake_" .. platform .. "/$(Configuration)")
		elseif _ACTION == "vs2012" or _ACTION == "vs2013" or _ACTION == "vs2015" then
			targetdir ("../glitchConfig/bin/freetype/$(SolutionName)/premake_" .. platform .. "/$(PlatformName)/$(Configuration)")
		elseif _ACTION == "vs2012wp8" or _ACTION=="vs2012w8" or _ACTION=="vs2013w8" then
			targetdir ("../glitchConfig/bin/freetype/$(SolutionName)/premake_" .. platform .. "/$(PlatformName)/$(Configuration)")
		end
		
		objdir ("../glitchConfig/build/freetype/$(SolutionName)/premake_" .. platform)
		
		-- Configurations
		
		if _ACTION == "vs2012wp8" or _ACTION=="vs2012w8" or _ACTION=="vs2013w8" then
			configuration "Debug"
				vsconfigreference "../glitch/source/freetype/make/premake_config/propertysheets/freetype - debug - wp8"
			
			configuration "Release"
				flags("Optimize")
				vsconfigreference "../glitch/source/freetype/make/premake_config/propertysheets/freetype - release - wp8"
		else
			-- Debug
			configuration "Debug - Multithreaded"
				vsconfigreference "../glitch/source/freetype/make/premake_config/propertysheets/freetype - debug multithreaded"
			
			configuration "Debug - Multithreaded DLL"
				vsconfigreference "../glitch/source/freetype/make/premake_config/propertysheets/freetype - debug multithreaded dll"
				
			-- Release
			configuration "Release - Multithreaded"	
				vsconfigreference "../glitch/source/freetype/make/premake_config/propertysheets/freetype - release multithreaded"
				
			configuration "Release - Multithreaded DLL"	
				vsconfigreference "../glitch/source/freetype/make/premake_config/propertysheets/freetype - release multithreaded dll"
		end
	end
end

freetype_project_wrt = function(_type)
	freetype_project_wrt_common_settings(_type, _type == "Windows" and "ADD22788-E9F2-8745-A2DC-4078E9A91BA7" or "40116485-B723-4B2F-B4DA-F35C054F6BA5")
	
	targetname "freetype"
	sharedItems ({"freetype." .. "Shared"})	
	apptype(_type)
	flags { "Unicode" }
	local platform = _ACTION
	targetdir ("../glitchConfig/bin/freetype/$(SolutionName)/premake_" .. platform .. "/$(TargetOsAndVersion)/$(PlatformName)/$(Configuration)")
	objdir ("../glitchConfig/build/freetype/$(SolutionName)/premake_" .. platform)
	
	configuration "Debug"
		vsconfigreference "../glitch/source/freetype/make/premake_config/propertysheets/freetype - debug - wrt"

	configuration "Release"
		flags("Optimize")
		vsconfigreference "../glitch/source/freetype/make/premake_config/propertysheets/freetype - release - wrt"
end					

freetype_shared_items_project_wrt = function()
	freetype_project_wrt_common_settings("Shared", "BF7C89AD-2819-43B8-9CF5-5FCF38898BF4")
	isItems(true)
	sharedUuid "952BE0BA-9440-4444-AF69-5770C9348E20"
	freetype_project_get_common_files()
end

freetype_project_wrt_common_settings = function(_type, _uuid)
	folder "freetype"
	
	project ("freetype." .. _type)
	language "C++"
	rootnamespace "freetype"
	location ( getFreeTypeLocation("../glitch/source/freetype/builds/") .. "/" .. _type )
	kind "StaticLib"
	
	uuid(_uuid)
end
	
freetype_project_get_common_files = function()
	files { "../glitch/source/freetype/src/base/ftbbox.c",
			"../glitch/source/freetype/src/base/ftmm.c",
			"../glitch/source/freetype/src/base/ftpfr.c",
			"../glitch/source/freetype/src/base/ftsynth.c",
			"../glitch/source/freetype/src/base/fttype1.c",
			"../glitch/source/freetype/src/base/ftwinfnt.c",
			"../glitch/source/freetype/src/base/ftbase.c",
			"../glitch/source/freetype/src/base/ftbitmap.c",
			"../glitch/source/freetype/src/base/ftgasp.c",
			"../glitch/source/freetype/src/base/ftglyph.c",
			"../glitch/source/freetype/src/base/ftinit.c",
			"../glitch/source/freetype/src/base/ftstroke.c",
			"../glitch/source/freetype/src/base/ftsystem.c"}
	
	files { "../glitch/source/freetype/src/pcf/pcf.c",
			"../glitch/source/freetype/src/pfr/pfr.c",
			"../glitch/source/freetype/src/psaux/psaux.c",
			"../glitch/source/freetype/src/pshinter/pshinter.c",
			"../glitch/source/freetype/src/psnames/psmodule.c",
			"../glitch/source/freetype/src/raster/raster.c",
			"../glitch/source/freetype/src/sfnt/sfnt.c",
			"../glitch/source/freetype/src/truetype/truetype.c",
			"../glitch/source/freetype/src/type1/type1.c",
			"../glitch/source/freetype/src/cid/type1cid.c",
			"../glitch/source/freetype/src/type42/type42.c",
			"../glitch/source/freetype/src/winfonts/winfnt.c",
			"../glitch/source/freetype/src/autofit/autofit.c",
			"../glitch/source/freetype/src/bdf/bdf.c",
			"../glitch/source/freetype/src/cff/cff.c",
			"../glitch/source/freetype/src/gzip/ftgzip.c",
			"../glitch/source/freetype/src/lzw/ftlzw.c",
			"../glitch/source/freetype/src/smooth/smooth.c",
			"../glitch/source/freetype/src/cache/ftcache.c",
			"../glitch/source/freetype/builds/win32/ftdebug.c"}
	
	files { "../glitch/source/freetype/include/ft2build.h",
			"../glitch/source/freetype/include/freetype/config/ftconfig.h",
			"../glitch/source/freetype/include/freetype/config/ftheader.h",
			"../glitch/source/freetype/include/freetype/config/ftmodule.h",
			"../glitch/source/freetype/include/freetype/config/ftoption.h",
			"../glitch/source/freetype/include/freetype/config/ftstdlib.h"}
end

getFreeTypeLocation = function(basePath)
	outputPath = basePath
	
	-- Options
	if _ACTION=="xcode3" then
		if  _OPTIONS["iphone"] then
			outputPath = outputPath .. "iphone"
		elseif  _OPTIONS["tvos"] then
			outputPath = outputPath .. "tvos"
		else
			outputPath = outputPath .. "mac"
		end
	elseif _ACTION=="vs2012wp8" then
		outputPath = outputPath .. "wp8"
	elseif _ACTION=="vs2012w8" or _ACTION=="vs2013w8" then
		outputPath = outputPath .. "w8"
	elseif _ACTION=="vs2013wrt" then
		outputPath = outputPath .. "wrt/VS2013"
	else
		outputPath = outputPath  .. "win32/visualc"
	end

	return outputPath
end
