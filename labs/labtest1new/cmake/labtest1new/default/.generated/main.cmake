# cmake files support debug production
include("${CMAKE_CURRENT_LIST_DIR}/rule.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/file.cmake")

set(labtest1new_default_library_list )

# Handle files with suffix (s|as|asm|AS|ASM|As|aS|Asm), for group default-XC8
if(labtest1new_default_default_XC8_FILE_TYPE_assemble)
add_library(labtest1new_default_default_XC8_assemble OBJECT ${labtest1new_default_default_XC8_FILE_TYPE_assemble})
    labtest1new_default_default_XC8_assemble_rule(labtest1new_default_default_XC8_assemble)
    list(APPEND labtest1new_default_library_list "$<TARGET_OBJECTS:labtest1new_default_default_XC8_assemble>")
endif()

# Handle files with suffix S, for group default-XC8
if(labtest1new_default_default_XC8_FILE_TYPE_assemblePreprocess)
add_library(labtest1new_default_default_XC8_assemblePreprocess OBJECT ${labtest1new_default_default_XC8_FILE_TYPE_assemblePreprocess})
    labtest1new_default_default_XC8_assemblePreprocess_rule(labtest1new_default_default_XC8_assemblePreprocess)
    list(APPEND labtest1new_default_library_list "$<TARGET_OBJECTS:labtest1new_default_default_XC8_assemblePreprocess>")
endif()

# Handle files with suffix [cC], for group default-XC8
if(labtest1new_default_default_XC8_FILE_TYPE_compile)
add_library(labtest1new_default_default_XC8_compile OBJECT ${labtest1new_default_default_XC8_FILE_TYPE_compile})
    labtest1new_default_default_XC8_compile_rule(labtest1new_default_default_XC8_compile)
    list(APPEND labtest1new_default_library_list "$<TARGET_OBJECTS:labtest1new_default_default_XC8_compile>")
endif()


add_executable(labtest1new_default_image_ed3VVJZv ${labtest1new_default_library_list})

set_target_properties(labtest1new_default_image_ed3VVJZv PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${labtest1new_default_output_dir})
set_target_properties(labtest1new_default_image_ed3VVJZv PROPERTIES OUTPUT_NAME "default")
set_target_properties(labtest1new_default_image_ed3VVJZv PROPERTIES SUFFIX ".elf")
         

target_link_libraries(labtest1new_default_image_ed3VVJZv PRIVATE ${labtest1new_default_default_XC8_FILE_TYPE_link})


# Add the link options from the rule file.
labtest1new_default_link_rule(labtest1new_default_image_ed3VVJZv)




