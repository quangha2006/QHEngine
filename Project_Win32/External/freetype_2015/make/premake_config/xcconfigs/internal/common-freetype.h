//  ===================================================================================================================
//
//      COMMON (internal, libglitch) - Freetype xcode config
//
//  * See the comomn global xcode config file (in Internal) for a detailled explanation about this system.
//  * See the user debug project xcode config file (in ../glitchConfig/xcconifg) for a detailled explanation about
//    user customisable configuration.
//
//  ===================================================================================================================

// Target name
PRODUCT_NAME = freetype

ARCHS = $(ARCHS_STANDARD_INCLUDING_64_BIT)

// Search Paths
COMMON_HEADER_SEARCH_PATHS = "$(SRCROOT)/../../include"

// Preprocessor Definitions
COMMON_GCC_PREPROCESSOR_DEFINITIONS=DARWIN_NO_CARBON FT2_BUILD_LIBRARY _LIB
