//  ===================================================================================================================
//
//      COMMON (internal, global) - xcode config
//
//  INFORMATION
//
//      Global files provide a fondation and basic configuration for xcode project. Since there is no "global" 
//      mechanics, copy all global file from a project to your own. Global configuration are the topmost 
//      configuration, so their configuration can easely be override.
//
//      Next are the project configuration; only use this layer if you have more than one target in your project.
//      Project configuration files are used with the project setting. There are 4 types of project configuration 
//      files: Common, Debug, Release and Platform. 
//
//      Common configuration apply to both debug and release. 
//
//      I choose to use two dinstinc files for debug/release configuration over the [variant=debug/release] syntax 
//      offered by Apple for clarity reasons since the IDE only display conditional SKD and ARCH, leaving an empty
//      box when VARIANT is used.
//
//      For platform (iphone device/simulator) I choose a shared file for both configuration since conditinal
//      setting works correctly for SDK
//
//      Finally you have target configuration file. Target configuration used the standard .xcconfig while internal
//      configuration file uses .h. The choice of .h is to prevent those configuration file to be exposed to the IDE
//      as a choice of "Based on" while keeping some syntax coloring when editing them.
//
//  ===================================================================================================================

// --------------------------------------------------------------------------------------------------------------------
// Code Signing
// --------------------------------------------------------------------------------------------------------------------

CODE_SIGN_IDENTITY = iPhone Developer						// Will try to get the appropriate identity by default

// --------------------------------------------------------------------------------------------------------------------
// Architectures
// --------------------------------------------------------------------------------------------------------------------

ARCHS = $(ARCHS_STANDARD_INCLUDING_64_BIT)					// actually, pointless to set this here, the generated project overwrites this
//SDKROOT = iphoneos											// Use latest iPhone SDK.
ONLY_ACTIVE_ARCH = NO										// Compile both all and choose the best 
															// depending on device

IPHONEOS_DEPLOYMENT_TARGET = 5.0
IPHONEOS_DEPLOYMENT_TARGET[arch=arm64] = 7.0

// --------------------------------------------------------------------------------------------------------------------
// Build Locations (Extended form provide a unique name for each build)
// --------------------------------------------------------------------------------------------------------------------

// Force values for SYSROOM and OBJROOT, this prevent the user from overriding this in his personnal setting. This was
// done to prevent errors. If you need to change this use the user config files instead.
SYMROOT = build
OBJROOT = build
CONFIGURATION_BUILD_DIR = $(BUILD_DIR)/$(CONFIGURATION)$(EFFECTIVE_PLATFORM_NAME)
CONFIGURATION_TEMP_DIR = $(PROJECT_TEMP_DIR)/$(CONFIGURATION)$(EFFECTIVE_PLATFORM_NAME)

// --------------------------------------------------------------------------------------------------------------------
// Compiler Version
// --------------------------------------------------------------------------------------------------------------------

GCC_VERSION = com.apple.compilers.llvm.clang.1_0			// Use LLVM

// --------------------------------------------------------------------------------------------------------------------
// Deployment
// --------------------------------------------------------------------------------------------------------------------

COPY_PHASE_STRIP = NO										// ??
STRIP_INSTALLED_PRODUCT = NO								// ??

// --------------------------------------------------------------------------------------------------------------------
// Linking
// --------------------------------------------------------------------------------------------------------------------

PREBINDING = NO												// ??

// --------------------------------------------------------------------------------------------------------------------
// Search Paths
// --------------------------------------------------------------------------------------------------------------------

HEADER_SEARCH_PATHS = $(USER_TARGET_PLATFORM_HEADER_SEARCH_PATHS) $(TARGET_PLATFORM_HEADER_SEARCH_PATHS) $(USER_TARGET_CONFIG_HEADER_SEARCH_PATHS) $(TARGET_CONFIG_HEADER_SEARCH_PATHS) $(USER_TARGET_COMMON_HEADER_SEARCH_PATHS) $(TARGET_COMMON_HEADER_SEARCH_PATHS) $(USER_PLATFORM_HEADER_SEARCH_PATHS) $(PLATFORM_HEADER_SEARCH_PATHS) $(USER_CONFIG_HEADER_SEARCH_PATHS) $(CONFIG_HEADER_SEARCH_PATHS) $(USER_COMMON_HEADER_SEARCH_PATHS) $(COMMON_HEADER_SEARCH_PATHS)

// --------------------------------------------------------------------------------------------------------------------
// Code generation
// --------------------------------------------------------------------------------------------------------------------

GCC_AUTO_VECTORIZATION = NO									// ??
GCC_DYNAMIC_NO_PIC = YES									// ??
GCC_ENABLE_FIX_AND_CONTINUE = NO							// ??
GCC_ENABLE_KERNEL_DEVELOPMENT = NO							// ??
GCC_ENABLE_OBJC_GC = unsupported							// ??
GCC_ENABLE_SSE3_EXTENSIONS = NO								// ??
GCC_ENABLE_SUPPLEMENTAL_SSE3_INSTRUCTIONS = NO				// ??
GCC_ENABLE_SYMBOL_SEPARATION = YES							// ??
GCC_FAST_MATH = NO											// ??
GCC_FAST_OBJC_DISPATCH = YES								// ??
GCC_FEEDBACK_DIRECTED_OPTIMIZATION = Off					// ??
GCC_GENERATE_TEST_COVERAGE_FILES = NO						// ??
GCC_INLINES_ARE_PRIVATE_EXTERN = YES						// ??
GCC_INSTRUMENT_PROGRAM_FLOW_ARCS = NO						// ??
GCC_MODEL_PPC64 = NO										// ??
GCC_MODEL_TUNING = G4										// ??
GCC_NO_COMMON_BLOCKS = NO									// ??
GCC_OBJC_CALL_CXX_CDTORS = NO								// ??
GCC_REUSE_STRINGS = YES										// ??
GCC_STRICT_ALIASING = NO									// ??
GCC_SYMBOLS_PRIVATE_EXTERN = NO								// ??
GCC_THREADSAFE_STATICS = YES								// ??
GCC_THUMB_SUPPORT = NO										// No, no interworking overhead by default.
GCC_UNROLL_LOOPS = NO										// ??

// --------------------------------------------------------------------------------------------------------------------
// Language
// --------------------------------------------------------------------------------------------------------------------

GCC_ALTIVEC_EXTENSIONS = NO									// ??
GCC_CHAR_IS_UNSIGNED_CHAR = NO								// ??
GCC_CHECK_RETURN_VALUE_OF_OPERATOR_NEW = NO					// ??
GCC_CW_ASM_SYNTAX = YES										// ??
GCC_C_LANGUAGE_STANDARD = c99								// ??
GCC_ENABLE_ASM_KEYWORD = YES								// ??
GCC_ENABLE_BUILTIN_FUNCTIONS = YES							// ??
GCC_ENABLE_CPP_EXCEPTIONS = NO								// No, don't use exception (performance improvement)
GCC_ENABLE_CPP_RTTI = NO									// No, don't use rtti (memory improvement)
GCC_ENABLE_FLOATING_POINT_LIBRARY_CALLS = NO				// ??
GCC_ENABLE_OBJC_EXCEPTIONS = YES							// ??
GCC_ENABLE_PASCAL_STRINGS = YES								// ??
GCC_ENABLE_TRIGRAPHS = NO									// ??
GCC_PREFIX_HEADER =                  						// No precompiled headers.
GCC_INCREASE_PRECOMPILED_HEADER_SHARING = NO				// No precompiled headers sharing.
GCC_PRECOMPILE_PREFIX_HEADER = NO							// No, project needs to override this settings.
GCC_FORCE_CPU_SUBTYPE_ALL = NO								// ??
GCC_INCREASE_PRECOMPILED_HEADER_SHARING = NO				// ??
GCC_INPUT_FILETYPE = automatic								// ??
GCC_LINK_WITH_DYNAMIC_LIBRARIES = YES						// ??
GCC_ONE_BYTE_BOOL = YES										// ??
GCC_PFE_FILE_C_DIALECTS = c objective-c c++ objective-c++	// ??
GCC_SHORT_ENUMS = NO										// ??
GCC_USE_GCC3_PFE_SUPPORT = YES								// ??
GCC_USE_INDIRECT_FUNCTION_CALLS = NO						// ??
GCC_USE_REGISTER_FUNCTION_CALLS = NO						// ??
GCC_USE_STANDARD_INCLUDE_SEARCHING = YES					// ??

// --------------------------------------------------------------------------------------------------------------------
// Preprocessor Definitions
// --------------------------------------------------------------------------------------------------------------------

GCC_PREPROCESSOR_DEFINITIONS[arch=armv7]        = $(USER_TARGET_ARM7_GCC_PREPROCESSOR_DEFINITIONS) $(ARM7_TARGET_GCC_PREPROCESSOR_DEFINITIONS) $(USER_TARGET_PLATFORM_GCC_PREPROCESSOR_DEFINITIONS) $(TARGET_PLATFORM_GCC_PREPROCESSOR_DEFINITIONS) $(USER_TARGET_CONFIG_GCC_PREPROCESSOR_DEFINITIONS) $(TARGET_CONFIG_GCC_PREPROCESSOR_DEFINITIONS) $(USER_TARGET_COMMON_GCC_PREPROCESSOR_DEFINITIONS) $(TARGET_COMMON_GCC_PREPROCESSOR_DEFINITIONS) $(USER_ARM7_GCC_PREPROCESSOR_DEFINITIONS) $(ARM7_GCC_PREPROCESSOR_DEFINITIONS) $(USER_PLATFORM_GCC_PREPROCESSOR_DEFINITIONS) $(PLATFORM_GCC_PREPROCESSOR_DEFINITIONS) $(USER_CONFIG_GCC_PREPROCESSOR_DEFINITIONS) $(CONFIG_GCC_PREPROCESSOR_DEFINITIONS) $(USER_COMMON_GCC_PREPROCESSOR_DEFINITIONS) $(COMMON_GCC_PREPROCESSOR_DEFINITIONS)
GCC_PREPROCESSOR_DEFINITIONS[arch=armv7s]        = $(USER_TARGET_ARM7S_GCC_PREPROCESSOR_DEFINITIONS) $(ARM7S_TARGET_GCC_PREPROCESSOR_DEFINITIONS) $(USER_TARGET_PLATFORM_GCC_PREPROCESSOR_DEFINITIONS) $(TARGET_PLATFORM_GCC_PREPROCESSOR_DEFINITIONS) $(USER_TARGET_CONFIG_GCC_PREPROCESSOR_DEFINITIONS) $(TARGET_CONFIG_GCC_PREPROCESSOR_DEFINITIONS) $(USER_TARGET_COMMON_GCC_PREPROCESSOR_DEFINITIONS) $(TARGET_COMMON_GCC_PREPROCESSOR_DEFINITIONS) $(USER_ARM7S_GCC_PREPROCESSOR_DEFINITIONS) $(ARM7S_GCC_PREPROCESSOR_DEFINITIONS) $(USER_PLATFORM_GCC_PREPROCESSOR_DEFINITIONS) $(PLATFORM_GCC_PREPROCESSOR_DEFINITIONS) $(USER_CONFIG_GCC_PREPROCESSOR_DEFINITIONS) $(CONFIG_GCC_PREPROCESSOR_DEFINITIONS) $(USER_COMMON_GCC_PREPROCESSOR_DEFINITIONS) $(COMMON_GCC_PREPROCESSOR_DEFINITIONS)
GCC_PREPROCESSOR_DEFINITIONS[arch=arm64]        = $(USER_TARGET_ARM64_GCC_PREPROCESSOR_DEFINITIONS) $(ARM64_TARGET_GCC_PREPROCESSOR_DEFINITIONS) $(USER_TARGET_PLATFORM_GCC_PREPROCESSOR_DEFINITIONS) $(TARGET_PLATFORM_GCC_PREPROCESSOR_DEFINITIONS) $(USER_TARGET_CONFIG_GCC_PREPROCESSOR_DEFINITIONS) $(TARGET_CONFIG_GCC_PREPROCESSOR_DEFINITIONS) $(USER_TARGET_COMMON_GCC_PREPROCESSOR_DEFINITIONS) $(TARGET_COMMON_GCC_PREPROCESSOR_DEFINITIONS) $(USER_ARM64_GCC_PREPROCESSOR_DEFINITIONS) $(ARM64_GCC_PREPROCESSOR_DEFINITIONS) $(USER_PLATFORM_GCC_PREPROCESSOR_DEFINITIONS) $(PLATFORM_GCC_PREPROCESSOR_DEFINITIONS) $(USER_CONFIG_GCC_PREPROCESSOR_DEFINITIONS) $(CONFIG_GCC_PREPROCESSOR_DEFINITIONS) $(USER_COMMON_GCC_PREPROCESSOR_DEFINITIONS) $(COMMON_GCC_PREPROCESSOR_DEFINITIONS)
GCC_PREPROCESSOR_DEFINITIONS[sdk=iphonesimulator*]			   = $(USER_TARGET_PLATFORM_GCC_PREPROCESSOR_DEFINITIONS) $(TARGET_PLATFORM_GCC_PREPROCESSOR_DEFINITIONS) $(USER_TARGET_CONFIG_GCC_PREPROCESSOR_DEFINITIONS) $(TARGET_CONFIG_GCC_PREPROCESSOR_DEFINITIONS) $(USER_TARGET_COMMON_GCC_PREPROCESSOR_DEFINITIONS) $(TARGET_COMMON_GCC_PREPROCESSOR_DEFINITIONS) $(USER_PLATFORM_GCC_PREPROCESSOR_DEFINITIONS) $(PLATFORM_GCC_PREPROCESSOR_DEFINITIONS) $(USER_CONFIG_GCC_PREPROCESSOR_DEFINITIONS) $(CONFIG_GCC_PREPROCESSOR_DEFINITIONS) $(USER_COMMON_GCC_PREPROCESSOR_DEFINITIONS) $(COMMON_GCC_PREPROCESSOR_DEFINITIONS)

// --------------------------------------------------------------------------------------------------------------------
// Warnings
// --------------------------------------------------------------------------------------------------------------------

GCC_TREAT_IMPLICIT_FUNCTION_DECLARATIONS_AS_ERRORS = YES	// ??
GCC_TREAT_NONCONFORMANT_CODE_ERRORS_AS_WARNINGS = NO		// No, our goal is a 0 error, 0 warning project.
GCC_TREAT_WARNINGS_AS_ERRORS = YES							// Yes, but you can override this in your user config.
GCC_WARN_64_TO_32_BIT_CONVERSION = NO						// ??
GCC_WARN_ABOUT_DEPRECATED_FUNCTIONS = YES					// ??
GCC_WARN_ABOUT_INVALID_OFFSETOF_MACRO = YES					// ??
GCC_WARN_ABOUT_MISSING_NEWLINE = NO							// No, legacy standard (ISO C 1999 5.1.1.2).
GCC_WARN_ABOUT_MISSING_PROTOTYPES = NO						// ??
GCC_WARN_ABOUT_POINTER_SIGNEDNESS = YES						// ??
GCC_WARN_ABOUT_RETURN_TYPE = YES							// ??
GCC_WARN_ALLOW_INCOMPLETE_PROTOCOL = YES					// ??
GCC_WARN_CHECK_SWITCH_STATEMENTS = NO						// ??
GCC_WARN_EFFECTIVE_CPLUSPLUS_VIOLATIONS = NO				// ??
GCC_WARN_FOUR_CHARACTER_CONSTANTS = NO						// ??
GCC_WARN_HIDDEN_VIRTUAL_FUNCTIONS = NO						// ??
GCC_WARN_INHIBIT_ALL_WARNINGS = NO							// No, we want as many warnings as possible.
GCC_WARN_INITIALIZER_NOT_FULLY_BRACKETED = NO				// ??
GCC_WARN_MISSING_PARENTHESES = NO							// ??
GCC_WARN_NON_VIRTUAL_DESTRUCTOR = YES						// Yes, you'll be stoned if you turn this off.
GCC_WARN_PEDANTIC = NO										// ??
GCC_WARN_PROTOTYPE_CONVERSION = NO							// ??
GCC_WARN_SHADOW = NO										// ??
GCC_WARN_SIGN_COMPARE = YES									// ??
GCC_WARN_TYPECHECK_CALLS_TO_PRINTF = YES					// ??
GCC_WARN_UNINITIALIZED_AUTOS = NO							// ??
GCC_WARN_UNKNOWN_PRAGMAS = NO								// ??
GCC_WARN_UNUSED_FUNCTION = NO								// ??
GCC_WARN_UNUSED_LABEL = YES									// ??
GCC_WARN_UNUSED_PARAMETER = NO								// ??
GCC_WARN_UNUSED_VALUE = NO									// ??
GCC_WARN_UNUSED_VARIABLE = NO								// ??
