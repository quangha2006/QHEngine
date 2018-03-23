//  ===================================================================================================================
//
//      DEBUG (internal, global) - xcode config
//
//  * See the comomn global xcode config file (in Internal) for a detailled explanation about this system.
//  * See the user debug project xcode config file (in ../glitchConfig/xcconifg) for a detailled explanation about
//    user customisable configuration.
//
//  ===================================================================================================================

// Architecture
ONLY_ACTIVE_ARCH = YES

// Optimization
GCC_OPTIMIZATION_LEVEL = 0

// Debug Symbols
GCC_GENERATE_DEBUGGING_SYMBOLS = YES
GCC_DEBUGGING_SYMBOLS          = full
DEBUG_INFORMATION_FORMAT       = dwarf-wit-dsym

// Preprocessor definition
CONFIG_GCC_PREPROCESSOR_DEFINITIONS = _DEBUG
