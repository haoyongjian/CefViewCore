file(GLOB_RECURSE CefViewCore_SHARED_SRC_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/Shared/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Shared/*.h"
)
source_group(
  TREE "${CMAKE_CURRENT_SOURCE_DIR}/Shared"
  PREFIX Shared
  FILES ${CefViewCore_SHARED_SRC_FILES}
)

# ## CefViewCore
# ###############################################################################################
# header files
file(GLOB_RECURSE CefViewCore_INCLUDE_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/../include/*.h"
)
source_group(
  TREE "${CMAKE_CURRENT_SOURCE_DIR}/../include"
  PREFIX Include
  FILES ${CefViewCore_INCLUDE_FILES}
)

# soruce code for all platforms
file(GLOB_RECURSE CefViewCore_SRC_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/CefView/CefBrowserApp/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/CefView/CefBrowserApp/*.cpp"
)
source_group(
  TREE "${CMAKE_CURRENT_SOURCE_DIR}/CefView/CefBrowserApp"
  PREFIX Source
  FILES ${CefViewCore_SRC_FILES}
)

add_library(CefViewCore STATIC
  ${CefViewCore_SHARED_SRC_FILES}
  ${CefViewCore_INCLUDE_FILES}
  ${CefViewCore_SRC_FILES}
)

target_include_directories(CefViewCore
  PUBLIC
  ${CefViewCore_INCLUDE_PATH}
  PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/Shared"
)

# ADD_LOGICAL_TARGET(libcef_lib
# "${CEF_LIB_DEBUG}"
# "${CEF_LIB_RELEASE}"
# )
#add_library(libcef_lib ${CEF_LIBTYPE} IMPORTED GLOBAL)
if(NOT TARGET libcef_lib)
    # Manually specify libcef.so path for LoongArch
    set(LIBCEF_SO_PATH "/home/hyj/cef-wps-loongarch64/libcef.so")

    if(NOT EXISTS "${LIBCEF_SO_PATH}")
        message(FATAL_ERROR "libcef.so not found at ${LIBCEF_SO_PATH}")
    endif()

    add_library(libcef_lib SHARED IMPORTED)
    set_target_properties(libcef_lib PROPERTIES
        IMPORTED_LOCATION "${LIBCEF_SO_PATH}"
	IMPORTED_LOCATION_RELEASE "${LIBCEF_SO_PATH}"   # Release
        IMPORTED_LOCATION_DEBUG "${LIBCEF_SO_PATH}"     # Debug（即使相同）
        INTERFACE_INCLUDE_DIRECTORIES "/home/hyj/cef-wps-loongarch64/include"
        INTERFACE_LINK_LIBRARIES ""
    )
endif()
#set_target_properties(libcef_lib
#  PROPERTIES
#  IMPORTED_LOCATION "${CEF_LIB_RELEASE}"
#  IMPORTED_LOCATION_DEBUG "${CEF_LIB_DEBUG}"
#  IMPORTED_LOCATION_RELEASE "${CEF_LIB_RELEASE}"
#)

if(OS_WINDOWS)
  SET_LIBRARY_TARGET_PROPERTIES(CefViewCore)

  add_dependencies(CefViewCore
    libcef_lib
    libcef_dll_wrapper
  )

  set(CefViewCore_LIBS
    libcef_lib
    libcef_dll_wrapper
    ${CEF_STANDARD_LIBS}
  )

  if(USE_SANDBOX AND(CEF_VERSION_MAJOR LESS 138))
    list(APPEND CefViewCore_LIBS cef_sandbox_lib)
  endif()

  target_link_libraries(CefViewCore
    PUBLIC
    ${CefViewCore_LIBS}
  )
endif() # OS_WINDOWS

if(OS_LINUX)
	#SET_LIBRARY_TARGET_PROPERTIES(CefViewCore)

  add_dependencies(CefViewCore
    libcef_lib
    #libcef_dll_wrapper
  )

  set(CefViewCore_LIBS
    libcef_lib
    #libcef_dll_wrapper
    ${CEF_STANDARD_LIBS}
  )

  target_link_libraries(CefViewCore
    PUBLIC
    ${CefViewCore_LIBS}
  )
endif() # OS_LINUX

if(OS_MACOS)
  file(GLOB_RECURSE CefViewCore_PUBLIC_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/../include/*.h"
  )

  target_compile_options(CefViewCore
    PRIVATE
    "-fobjc-arc"
  )

  SET_LIBRARY_TARGET_PROPERTIES(CefViewCore)

  set_target_properties(CefViewCore
    PROPERTIES
    FRAMEWORK TRUE
    PUBLIC_HEADER "${CefViewCore_PUBLIC_HEADERS}"
    CLANG_ENABLE_OBJC_ARC "YES"
    APPEND_STRING PROPERTY COMPILE_FLAGS "-fobjc-arc"
    XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "gnu++11" # -std=gnu++11
    XCODE_ATTRIBUTE_CLANG_LINK_OBJC_RUNTIME "NO" # -fno-objc-link-runtime
    XCODE_ATTRIBUTE_COPY_PHASE_STRIP "NO"
    XCODE_ATTRIBUTE_DEAD_CODE_STRIPPING[variant=Release] "YES" # -Wl,-dead_strip
    XCODE_ATTRIBUTE_GCC_C_LANGUAGE_STANDARD "c99" # -std=c99
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.cefview.cefview"
  )

  add_dependencies(CefViewCore
    libcef_dll_wrapper
  )

  set(CefViewCore_LIBS
    libcef_dll_wrapper
    ${CEF_STANDARD_LIBS}
  )

  if(USE_SANDBOX AND(CEF_VERSION_MAJOR LESS 138))
    list(APPEND CefViewCore_LIBS cef_sandbox_lib)
  endif()

  target_link_libraries(CefViewCore
    PUBLIC
    ${CefViewCore_LIBS}
  )
endif() # OS_MACOS

add_library(CefViewCore::CefViewCore ALIAS CefViewCore)

# ## CefViewWing
# ###############################################################################################
# soruce code for all platforms
file(GLOB_RECURSE CefViewWing_SRC_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/App/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/App/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/Bridge/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/Bridge/*.cpp"
)
source_group(
  TREE "${CMAKE_CURRENT_SOURCE_DIR}/CefWing"
  PREFIX Source
  FILES ${CefViewWing_SRC_FILES}
)

if(OS_WINDOWS)
  file(GLOB_RECURSE CefViewWing_PLATFORM_SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/win/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/win/*.cpp"
  )
  source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/win"
    PREFIX Source
    FILES ${CefViewWing_PLATFORM_SRC_FILES}
  )

  if(EXISTS ${CEFVIEW_WING_ICON})
    # Copy icon file to binary dir
    configure_file(
      "${CEFVIEW_WING_ICON}"
      "${CMAKE_CURRENT_BINARY_DIR}/app.ico"
      COPYONLY
    )

    # Config resource file
    configure_file(
      "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/win/resource.rc.in"
      "${CMAKE_CURRENT_BINARY_DIR}/resource.rc"
      COPYONLY
    )
    set(CefViewWing_WIN_RESOURCE_FILE "${CMAKE_CURRENT_BINARY_DIR}/resource.rc")
  else()
    set(CefViewWing_WIN_RESOURCE_FILE "")
  endif()

  # Create Helper executable target.
  add_executable(CefViewWing WIN32
    ${CefViewCore_SHARED_SRC_FILES}
    ${CefViewWing_SRC_FILES}
    ${CefViewWing_PLATFORM_SRC_FILES}
    ${CefViewWing_WIN_RESOURCE_FILE}
  )

  SET_EXECUTABLE_TARGET_PROPERTIES(CefViewWing)
  set_target_properties(CefViewWing
    PROPERTIES
    OUTPUT_NAME ${CEFVIEW_WING_NAME}
  )

  target_link_options(CefViewWing
    PRIVATE
    "/MANIFEST"
  )

  target_include_directories(CefViewWing
    PUBLIC
    ${CefViewCore_INCLUDE_PATH}
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/Shared"
  )

  add_dependencies(CefViewWing
    libcef_lib
    libcef_dll_wrapper
  )

  set(_helper_libs
    d3d11.lib
    glu32.lib
    imm32.lib
    opengl32.lib

    libcef_lib
    libcef_dll_wrapper
    ${CEF_STANDARD_LIBS}
    ${CEF_SANDBOX_STANDARD_LIBS}
  )

  if(USE_SANDBOX AND(CEF_VERSION_MAJOR LESS 138))
    list(APPEND _helper_libs cef_sandbox_lib)
  endif()

  target_link_libraries(CefViewWing
    ${_helper_libs}
  )

  # Add the Helper as a dependency of the main executable target.
  add_dependencies(CefViewCore CefViewWing)

  add_custom_command(TARGET CefViewWing
    POST_BUILD

    # embed the manifest file
    COMMAND mt.exe
    -manifest "\"${CMAKE_CURRENT_SOURCE_DIR}\\CefWing\\win\\CefViewWing.manifest\""
    -outputresource:\"$<TARGET_FILE:CefViewWing>\"

    # copy cef binary files
    # 1 copy the cef resources to output dir
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CEF_RESOURCE_DIR}"
    "$<TARGET_FILE_DIR:CefViewWing>"

    # copy all cef binary files to output dir
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CEF_BINARY_DIR}"
    "$<TARGET_FILE_DIR:CefViewWing>"

    # move libcef.lib to lib output
    COMMAND ${CMAKE_COMMAND} -E rename
    "$<TARGET_FILE_DIR:CefViewWing>/libcef.lib"
    "$<TARGET_LINKER_FILE_DIR:libcef_dll_wrapper>/libcef.lib"
  )

  if(USE_SANDBOX AND CEF_VERSION_MAJOR LESS 138)
    add_custom_command(
      TARGET CefViewWing
      POST_BUILD

      # move cef_sandbox.lib to lib output
      COMMAND ${CMAKE_COMMAND} -E rename
      "$<TARGET_FILE_DIR:CefViewWing>/cef_sandbox.lib"
      "$<TARGET_LINKER_FILE_DIR:libcef_dll_wrapper>/cef_sandbox.lib"
    )
  endif()
endif() # OS_WINDOWS

if(OS_LINUX)
  file(GLOB_RECURSE CefViewWing_PLATFORM_SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/linux/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/linux/*.cpp"
  )
  source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/linux"
    PREFIX Source
    FILES ${CefViewWing_PLATFORM_SRC_FILES}
  )

  # Create Helper executable target.
  add_executable(CefViewWing
    ${CefViewCore_SHARED_SRC_FILES}
    ${CefViewWing_SRC_FILES}
    ${CefViewWing_PLATFORM_SRC_FILES}
  )

  #SET_EXECUTABLE_TARGET_PROPERTIES(CefViewWing)

  set_target_properties(CefViewWing
    PROPERTIES
    INSTALL_RPATH "$ORIGIN"
    BUILD_WITH_INSTALL_RPATH TRUE
    OUTPUT_NAME ${CEFVIEW_WING_NAME}
  )

  target_include_directories(CefViewWing
    PUBLIC
    ${CefViewCore_INCLUDE_PATH}
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/Shared"
  )

  add_dependencies(CefViewWing
    libcef_lib
    #libcef_dll_wrapper
  )

  set(_helper_libs
    libcef_lib
    #libcef_dll_wrapper
    ${CEF_STANDARD_LIBS}
  )

  target_link_libraries(CefViewWing
    ${_helper_libs}
  )

  # Add the Helper as a dependency of the main executable target.
  add_dependencies(CefViewCore CefViewWing)

  # copy cef binary files
  add_custom_command(TARGET CefViewWing
    POST_BUILD

    # copy cef binary files
    # .1 copy the cef resources to output dir
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CEF_RESOURCE_DIR}"
    "$<TARGET_FILE_DIR:CefViewWing>"

    # copy all cef binary files to output dir
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CEF_BINARY_DIR}"
    "$<TARGET_FILE_DIR:CefViewWing>"
  )

  #SET_LINUX_SUID_PERMISSIONS(CefViewWing "$<TARGET_FILE_DIR:CefViewWing>/chrome-sandbox")
endif() # OS_LINUX

if(OS_MACOS)
  file(GLOB_RECURSE CefViewWing_PLATFORM_SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/mac/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/mac/*.m"
    "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/mac/*.mm"
    "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/mac/*.cpp"
  )
  source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/mac"
    PREFIX Source
    FILES ${CefViewWing_PLATFORM_SRC_FILES}
  )

  set(CefViewWing_LIBS
    libcef_dll_wrapper
    ${CEF_STANDARD_LIBS}
  )

  if(USE_SANDBOX AND(CEF_VERSION_MAJOR LESS 138))
    list(APPEND CefViewWing_LIBS cef_sandbox_lib)
  endif()

  # Create the multiple Helper app bundle targets.
  foreach(_suffix_list ${CEF_HELPER_APP_SUFFIXES})
    # Convert to a list and extract the suffix values.
    string(REPLACE ":" ";" _suffix_list ${_suffix_list})
    list(GET _suffix_list 0 _name_suffix)
    list(GET _suffix_list 1 _target_suffix)
    list(GET _suffix_list 2 _plist_suffix)

    # Define Helper target and output names.
    set(_helper_target "CefViewWing${_target_suffix}")
    set(_helper_output_name "${CEFVIEW_WING_NAME}${_name_suffix}")

    # Create Helper executable target.
    add_executable(${_helper_target} MACOSX_BUNDLE
      ${CefViewCore_SHARED_SRC_FILES}
      ${CefViewWing_SRC_FILES}
      ${CefViewWing_PLATFORM_SRC_FILES}
    )
    SET_EXECUTABLE_TARGET_PROPERTIES(${_helper_target})

    set(_helper_info_plist "${CMAKE_CURRENT_BINARY_DIR}/helper-Info${_target_suffix}.plist")
    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/mac/info.plist" _plist_contents)
    file(WRITE ${_helper_info_plist} ${_plist_contents})

    set_target_properties(${_helper_target}
      PROPERTIES
      CLANG_ENABLE_OBJC_ARC "YES"
      OUTPUT_NAME "${_helper_output_name}"
      MACOSX_BUNDLE_INFO_PLIST "${_helper_info_plist}"
      XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.cefview.${CEFVIEW_WING_NAME}${_plist_suffix}"
      XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME "YES"
      XCODE_ATTRIBUTE_CODE_SIGN_ENTITLEMENTS "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/mac/CefViewWing.entitlements"
    )

    target_include_directories(${_helper_target}
      PUBLIC
      ${CefViewCore_INCLUDE_PATH}
      PRIVATE
      "${CMAKE_CURRENT_SOURCE_DIR}/Shared"
    )

    add_dependencies(${_helper_target}
      libcef_dll_wrapper
    )

    target_link_libraries(${_helper_target}
      ${CefViewWing_LIBS}
    )

    # Add the Helper as a dependency of the main executable target.
    add_dependencies(CefViewCore ${_helper_target})
  endforeach()

  # copy cef binary files to the output folder
  add_custom_command(TARGET CefViewWing
    POST_BUILD

    # copy the cef framework to output directory
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CEF_BINARY_DIR}/Chromium Embedded Framework.framework"
    "$<TARGET_BUNDLE_DIR:CefViewWing>/../Chromium Embedded Framework.framework"
    VERBATIM
  )
endif() # OS_MACOS

# LoongArch64 special handling
if(CMAKE_SYSTEM_PROCESSOR MATCHES "loongarch|loongarch64")
    add_definitions(-D__loongarch64__)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/CefWing/Adapter)

    # Force include remap header
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -include CefSymbolRemap.h")

    # Redefine CEF API calls
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
        -DCefV8Value::CreateNull=CEF_CALL(void\\*,\ \"_ZN10CefV8Value13CreateNullEv\") \
        -DCefV8Value::CreateUndefined=CEF_CALL(void\\*,\ \"_ZN10CefV8Value15CreateUndefinedEv\") \
        -DCefV8Value::CreateBool=CEF_CALL(void\\*,\ \"_ZN10CefV8Value13CreateBoolEb\", v) \
        -DCefV8Value::CreateInt=CEF_CALL(void\\*,\ \"_ZN10CefV8Value12CreateIntEi\", v) \
        -DCefV8Value::CreateDouble=CEF_CALL(void\\*,\ \"_ZN10CefV8Value14CreateDoubleEd\", v) \
        -DCefV8Value::CreateString=CEF_CALL(void\\*,\ \"_ZN10CefV8Value13CreateStringERK13CefStringBaseI20CefStringTraitsUTF16E\", str) \
        -DCefV8Value::CreateArray=CEF_CALL(void\\*,\ \"_ZN10CefV8Value13CreateArrayEi\", len) \
        -DCefV8Value::CreateObject=CEF_CALL(void\\*,\ \"_ZN10CefV8Value13CreateObjectE13scoped_refptrI13CefV8AccessorES3_I20CefV8InterceptorE\", a, b) \
        -DCefV8Value::CreateFunction=CEF_CALL(void\\*,\ \"_ZN10CefV8Value13CreateFunctionERK13CefStringBaseI20CefStringTraitsUTF16E13scoped_refptrI13CefV8HandlerE\", name, handler) \
        -DCefProcessMessage::Create=CEF_CALL(void\\*,\ \"_ZN16CefProcessMessage13CreateERK13CefStringBaseI20CefStringTraitsUTF16E\", name) \
        -DCefCommandLine::CreateCommandLine=CEF_CALL(void\\*,\ \"_ZN14CefCommandLine17CreateCommandLineEv\") \
        -DCefValue::Create=CEF_CALL(void\\*,\ \"_ZN9CefValue6CreateEv\") \
        -DCefListValue::Create=CEF_CALL(void\\*,\ \"_ZN12CefListValue6CreateEv\") \
        -DCefDictionaryValue::Create=CEF_CALL(void\\*,\ \"_ZN18CefDictionaryValue6CreateEv\") \
        -DCefMessageRouterConfig::CefMessageRouterConfig=CEF_CALL(void,\ \"_ZN21CefMessageRouterConfigC1Ev\", this) \
        -DCefMessageRouterRendererSide::Create=CEF_CALL(void\\*,\ \"_ZN33CefMessageRouterRendererSide6CreateERK21CefMessageRouterConfig\", config) \
        -DCefV8Context::GetCurrentContext=CEF_CALL(void\\*,\ \"_ZN13CefV8Context18GetCurrentContextEv\") \
        -DCefExecuteProcess=CEF_CALL(int,\ \"_ZN11CefExecuteProcessERK11CefMainArgs13scoped_refptrI5CefAppEPv\", args, app, something) \
        -Dbase::cef_subtle::RefCountedThreadSafeBase::Release=CEF_CALL(int,\ \"_ZNK4base11cef_subtle25RefCountedThreadSafeBase7ReleaseEv\", this) \
        -Dbase::cef_subtle::RefCountedThreadSafeBase::AddRef=CEF_CALL(void,\ \"_ZNK4base11cef_subtle25RefCountedThreadSafeBase6AddRefEv\", this) \
    "
endif()

# add alias for helper process target
add_executable(CefViewCore::CefViewWing ALIAS CefViewWing)
