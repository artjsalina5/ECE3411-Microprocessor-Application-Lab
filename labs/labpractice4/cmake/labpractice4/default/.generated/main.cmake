# cmake files support debug production
include("${CMAKE_CURRENT_LIST_DIR}/rule.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/file.cmake")

set(labpractice4_default_library_list )

# Handle files with suffix (s|as|asm|AS|ASM|As|aS|Asm), for group default-XC8
if(labpractice4_default_default_XC8_FILE_TYPE_assemble)
add_library(labpractice4_default_default_XC8_assemble OBJECT ${labpractice4_default_default_XC8_FILE_TYPE_assemble})
    labpractice4_default_default_XC8_assemble_rule(labpractice4_default_default_XC8_assemble)
    list(APPEND labpractice4_default_library_list "$<TARGET_OBJECTS:labpractice4_default_default_XC8_assemble>")
endif()

# Handle files with suffix S, for group default-XC8
if(labpractice4_default_default_XC8_FILE_TYPE_assemblePreprocess)
add_library(labpractice4_default_default_XC8_assemblePreprocess OBJECT ${labpractice4_default_default_XC8_FILE_TYPE_assemblePreprocess})
    labpractice4_default_default_XC8_assemblePreprocess_rule(labpractice4_default_default_XC8_assemblePreprocess)
    list(APPEND labpractice4_default_library_list "$<TARGET_OBJECTS:labpractice4_default_default_XC8_assemblePreprocess>")
endif()

# Handle files with suffix [cC], for group default-XC8
if(labpractice4_default_default_XC8_FILE_TYPE_compile)
add_library(labpractice4_default_default_XC8_compile OBJECT ${labpractice4_default_default_XC8_FILE_TYPE_compile})
    labpractice4_default_default_XC8_compile_rule(labpractice4_default_default_XC8_compile)
    list(APPEND labpractice4_default_library_list "$<TARGET_OBJECTS:labpractice4_default_default_XC8_compile>")
endif()


add_executable(labpractice4_default_image_1UM4c_SE ${labpractice4_default_library_list})

set_target_properties(labpractice4_default_image_1UM4c_SE PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${labpractice4_default_output_dir})
set_target_properties(labpractice4_default_image_1UM4c_SE PROPERTIES OUTPUT_NAME "default")
set_target_properties(labpractice4_default_image_1UM4c_SE PROPERTIES SUFFIX ".elf")
         

target_link_libraries(labpractice4_default_image_1UM4c_SE PRIVATE ${labpractice4_default_default_XC8_FILE_TYPE_link})


# Add the link options from the rule file.
labpractice4_default_link_rule(labpractice4_default_image_1UM4c_SE)




