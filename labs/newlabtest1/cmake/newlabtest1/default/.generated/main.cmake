# cmake files support debug production
include("${CMAKE_CURRENT_LIST_DIR}/rule.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/file.cmake")

set(newlabtest1_default_library_list )

# Handle files with suffix (s|as|asm|AS|ASM|As|aS|Asm), for group default-XC8
if(newlabtest1_default_default_XC8_FILE_TYPE_assemble)
add_library(newlabtest1_default_default_XC8_assemble OBJECT ${newlabtest1_default_default_XC8_FILE_TYPE_assemble})
    newlabtest1_default_default_XC8_assemble_rule(newlabtest1_default_default_XC8_assemble)
    list(APPEND newlabtest1_default_library_list "$<TARGET_OBJECTS:newlabtest1_default_default_XC8_assemble>")
endif()

# Handle files with suffix S, for group default-XC8
if(newlabtest1_default_default_XC8_FILE_TYPE_assemblePreprocess)
add_library(newlabtest1_default_default_XC8_assemblePreprocess OBJECT ${newlabtest1_default_default_XC8_FILE_TYPE_assemblePreprocess})
    newlabtest1_default_default_XC8_assemblePreprocess_rule(newlabtest1_default_default_XC8_assemblePreprocess)
    list(APPEND newlabtest1_default_library_list "$<TARGET_OBJECTS:newlabtest1_default_default_XC8_assemblePreprocess>")
endif()

# Handle files with suffix [cC], for group default-XC8
if(newlabtest1_default_default_XC8_FILE_TYPE_compile)
add_library(newlabtest1_default_default_XC8_compile OBJECT ${newlabtest1_default_default_XC8_FILE_TYPE_compile})
    newlabtest1_default_default_XC8_compile_rule(newlabtest1_default_default_XC8_compile)
    list(APPEND newlabtest1_default_library_list "$<TARGET_OBJECTS:newlabtest1_default_default_XC8_compile>")
endif()


add_executable(newlabtest1_default_image_CNeS1WAA ${newlabtest1_default_library_list})

set_target_properties(newlabtest1_default_image_CNeS1WAA PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${newlabtest1_default_output_dir})
set_target_properties(newlabtest1_default_image_CNeS1WAA PROPERTIES OUTPUT_NAME "default")
set_target_properties(newlabtest1_default_image_CNeS1WAA PROPERTIES SUFFIX ".elf")
         

target_link_libraries(newlabtest1_default_image_CNeS1WAA PRIVATE ${newlabtest1_default_default_XC8_FILE_TYPE_link})


# Add the link options from the rule file.
newlabtest1_default_link_rule(newlabtest1_default_image_CNeS1WAA)




