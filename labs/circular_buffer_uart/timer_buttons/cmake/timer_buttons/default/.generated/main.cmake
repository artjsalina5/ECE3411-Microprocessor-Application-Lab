# cmake files support debug production
include("${CMAKE_CURRENT_LIST_DIR}/rule.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/file.cmake")

set(timer_buttons_default_library_list )

# Handle files with suffix s, for group default-AVR-GCC
if(timer_buttons_default_default_AVR_GCC_FILE_TYPE_assemble)
add_library(timer_buttons_default_default_AVR_GCC_assemble OBJECT ${timer_buttons_default_default_AVR_GCC_FILE_TYPE_assemble})
    timer_buttons_default_default_AVR_GCC_assemble_rule(timer_buttons_default_default_AVR_GCC_assemble)
    list(APPEND timer_buttons_default_library_list "$<TARGET_OBJECTS:timer_buttons_default_default_AVR_GCC_assemble>")
endif()

# Handle files with suffix S, for group default-AVR-GCC
if(timer_buttons_default_default_AVR_GCC_FILE_TYPE_assembleWithPreprocess)
add_library(timer_buttons_default_default_AVR_GCC_assembleWithPreprocess OBJECT ${timer_buttons_default_default_AVR_GCC_FILE_TYPE_assembleWithPreprocess})
    timer_buttons_default_default_AVR_GCC_assembleWithPreprocess_rule(timer_buttons_default_default_AVR_GCC_assembleWithPreprocess)
    list(APPEND timer_buttons_default_library_list "$<TARGET_OBJECTS:timer_buttons_default_default_AVR_GCC_assembleWithPreprocess>")
endif()

# Handle files with suffix [cC], for group default-AVR-GCC
if(timer_buttons_default_default_AVR_GCC_FILE_TYPE_compile)
add_library(timer_buttons_default_default_AVR_GCC_compile OBJECT ${timer_buttons_default_default_AVR_GCC_FILE_TYPE_compile})
    timer_buttons_default_default_AVR_GCC_compile_rule(timer_buttons_default_default_AVR_GCC_compile)
    list(APPEND timer_buttons_default_library_list "$<TARGET_OBJECTS:timer_buttons_default_default_AVR_GCC_compile>")
endif()

# Handle files with suffix cpp, for group default-AVR-GCC
if(timer_buttons_default_default_AVR_GCC_FILE_TYPE_compile_cpp)
add_library(timer_buttons_default_default_AVR_GCC_compile_cpp OBJECT ${timer_buttons_default_default_AVR_GCC_FILE_TYPE_compile_cpp})
    timer_buttons_default_default_AVR_GCC_compile_cpp_rule(timer_buttons_default_default_AVR_GCC_compile_cpp)
    list(APPEND timer_buttons_default_library_list "$<TARGET_OBJECTS:timer_buttons_default_default_AVR_GCC_compile_cpp>")
endif()

# Handle files with suffix elf, for group default-AVR-GCC
if(timer_buttons_default_default_AVR_GCC_FILE_TYPE_objcopy_ihex)
add_library(timer_buttons_default_default_AVR_GCC_objcopy_ihex OBJECT ${timer_buttons_default_default_AVR_GCC_FILE_TYPE_objcopy_ihex})
    timer_buttons_default_default_AVR_GCC_objcopy_ihex_rule(timer_buttons_default_default_AVR_GCC_objcopy_ihex)
    list(APPEND timer_buttons_default_library_list "$<TARGET_OBJECTS:timer_buttons_default_default_AVR_GCC_objcopy_ihex>")
endif()

# Handle files with suffix elf, for group default-AVR-GCC
if(timer_buttons_default_default_AVR_GCC_FILE_TYPE_objcopy_eep)
add_library(timer_buttons_default_default_AVR_GCC_objcopy_eep OBJECT ${timer_buttons_default_default_AVR_GCC_FILE_TYPE_objcopy_eep})
    timer_buttons_default_default_AVR_GCC_objcopy_eep_rule(timer_buttons_default_default_AVR_GCC_objcopy_eep)
    list(APPEND timer_buttons_default_library_list "$<TARGET_OBJECTS:timer_buttons_default_default_AVR_GCC_objcopy_eep>")
endif()

# Handle files with suffix elf, for group default-AVR-GCC
if(timer_buttons_default_default_AVR_GCC_FILE_TYPE_objcopy_lss)
add_library(timer_buttons_default_default_AVR_GCC_objcopy_lss OBJECT ${timer_buttons_default_default_AVR_GCC_FILE_TYPE_objcopy_lss})
    timer_buttons_default_default_AVR_GCC_objcopy_lss_rule(timer_buttons_default_default_AVR_GCC_objcopy_lss)
    list(APPEND timer_buttons_default_library_list "$<TARGET_OBJECTS:timer_buttons_default_default_AVR_GCC_objcopy_lss>")
endif()

# Handle files with suffix elf, for group default-AVR-GCC
if(timer_buttons_default_default_AVR_GCC_FILE_TYPE_objcopy_srec)
add_library(timer_buttons_default_default_AVR_GCC_objcopy_srec OBJECT ${timer_buttons_default_default_AVR_GCC_FILE_TYPE_objcopy_srec})
    timer_buttons_default_default_AVR_GCC_objcopy_srec_rule(timer_buttons_default_default_AVR_GCC_objcopy_srec)
    list(APPEND timer_buttons_default_library_list "$<TARGET_OBJECTS:timer_buttons_default_default_AVR_GCC_objcopy_srec>")
endif()

# Handle files with suffix elf, for group default-AVR-GCC
if(timer_buttons_default_default_AVR_GCC_FILE_TYPE_objcopy_sig)
add_library(timer_buttons_default_default_AVR_GCC_objcopy_sig OBJECT ${timer_buttons_default_default_AVR_GCC_FILE_TYPE_objcopy_sig})
    timer_buttons_default_default_AVR_GCC_objcopy_sig_rule(timer_buttons_default_default_AVR_GCC_objcopy_sig)
    list(APPEND timer_buttons_default_library_list "$<TARGET_OBJECTS:timer_buttons_default_default_AVR_GCC_objcopy_sig>")
endif()


add_executable(timer_buttons_default_image_8s0EOuEP ${timer_buttons_default_library_list})

set_target_properties(timer_buttons_default_image_8s0EOuEP PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${timer_buttons_default_output_dir})
set_target_properties(timer_buttons_default_image_8s0EOuEP PROPERTIES OUTPUT_NAME "default")
set_target_properties(timer_buttons_default_image_8s0EOuEP PROPERTIES SUFFIX ".elf")
         

target_link_libraries(timer_buttons_default_image_8s0EOuEP PRIVATE ${timer_buttons_default_default_AVR_GCC_FILE_TYPE_link})

#Add objcopy steps
timer_buttons_default_objcopy_ihex_rule(timer_buttons_default_image_8s0EOuEP)
timer_buttons_default_objcopy_eep_rule(timer_buttons_default_image_8s0EOuEP)
timer_buttons_default_objcopy_lss_rule(timer_buttons_default_image_8s0EOuEP)
timer_buttons_default_objcopy_srec_rule(timer_buttons_default_image_8s0EOuEP)
timer_buttons_default_objcopy_sig_rule(timer_buttons_default_image_8s0EOuEP)

# Add the link options from the rule file.
timer_buttons_default_link_rule(timer_buttons_default_image_8s0EOuEP)




