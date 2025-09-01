# cmake files support debug production
include("${CMAKE_CURRENT_LIST_DIR}/rule.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/file.cmake")

set(lab01_default_library_list )

# Handle files with suffix (s|as|asm|AS|ASM|As|aS|Asm), for group toolchain XC8 3.00
if(lab01_default_toolchain_XC8_3_00_FILE_TYPE_assemble)
add_library(lab01_default_toolchain_XC8_3_00_assemble OBJECT ${lab01_default_toolchain_XC8_3_00_FILE_TYPE_assemble})
    lab01_default_toolchain_XC8_3_00_assemble_rule(lab01_default_toolchain_XC8_3_00_assemble)
    list(APPEND lab01_default_library_list "$<TARGET_OBJECTS:lab01_default_toolchain_XC8_3_00_assemble>")
endif()

# Handle files with suffix S, for group toolchain XC8 3.00
if(lab01_default_toolchain_XC8_3_00_FILE_TYPE_assemblePreprocess)
add_library(lab01_default_toolchain_XC8_3_00_assemblePreprocess OBJECT ${lab01_default_toolchain_XC8_3_00_FILE_TYPE_assemblePreprocess})
    lab01_default_toolchain_XC8_3_00_assemblePreprocess_rule(lab01_default_toolchain_XC8_3_00_assemblePreprocess)
    list(APPEND lab01_default_library_list "$<TARGET_OBJECTS:lab01_default_toolchain_XC8_3_00_assemblePreprocess>")
endif()

# Handle files with suffix [cC], for group toolchain XC8 3.00
if(lab01_default_toolchain_XC8_3_00_FILE_TYPE_compile)
add_library(lab01_default_toolchain_XC8_3_00_compile OBJECT ${lab01_default_toolchain_XC8_3_00_FILE_TYPE_compile})
    lab01_default_toolchain_XC8_3_00_compile_rule(lab01_default_toolchain_XC8_3_00_compile)
    list(APPEND lab01_default_library_list "$<TARGET_OBJECTS:lab01_default_toolchain_XC8_3_00_compile>")
endif()


add_executable(lab01_default_image_LV6_YV5Q ${lab01_default_library_list})

set_target_properties(lab01_default_image_LV6_YV5Q PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${lab01_default_output_dir})
set_target_properties(lab01_default_image_LV6_YV5Q PROPERTIES OUTPUT_NAME "default")
set_target_properties(lab01_default_image_LV6_YV5Q PROPERTIES SUFFIX ".elf")
         

target_link_libraries(lab01_default_image_LV6_YV5Q PRIVATE ${lab01_default_toolchain_XC8_3_00_FILE_TYPE_link})


# Add the link options from the rule file.
lab01_default_link_rule(lab01_default_image_LV6_YV5Q)




