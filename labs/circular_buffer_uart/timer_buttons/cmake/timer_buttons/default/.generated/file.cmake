# The following variables contains the files used by the different stages of the build process.
set(timer_buttons_default_default_AVR_GCC_FILE_TYPE_assemble)
set_source_files_properties(${timer_buttons_default_default_AVR_GCC_FILE_TYPE_assemble} PROPERTIES LANGUAGE ASM)

# For assembly files, add "." to the include path for each file so that .include with a relative path works
foreach(source_file ${timer_buttons_default_default_AVR_GCC_FILE_TYPE_assemble})
        set_source_files_properties(${source_file} PROPERTIES INCLUDE_DIRECTORIES "$<PATH:NORMAL_PATH,$<PATH:REMOVE_FILENAME,${source_file}>>")
endforeach()

set(timer_buttons_default_default_AVR_GCC_FILE_TYPE_assembleWithPreprocess)
set_source_files_properties(${timer_buttons_default_default_AVR_GCC_FILE_TYPE_assembleWithPreprocess} PROPERTIES LANGUAGE ASM)

# For assembly files, add "." to the include path for each file so that .include with a relative path works
foreach(source_file ${timer_buttons_default_default_AVR_GCC_FILE_TYPE_assembleWithPreprocess})
        set_source_files_properties(${source_file} PROPERTIES INCLUDE_DIRECTORIES "$<PATH:NORMAL_PATH,$<PATH:REMOVE_FILENAME,${source_file}>>")
endforeach()

set(timer_buttons_default_default_AVR_GCC_FILE_TYPE_compile
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../include/circularbuff.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../include/cpu.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../include/rtc.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../include/tca0.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../include/uart.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../main.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../rtc_usage_examples.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../tca0_usage_examples.c")
set_source_files_properties(${timer_buttons_default_default_AVR_GCC_FILE_TYPE_compile} PROPERTIES LANGUAGE C)
set(timer_buttons_default_default_AVR_GCC_FILE_TYPE_compile_cpp)
set_source_files_properties(${timer_buttons_default_default_AVR_GCC_FILE_TYPE_compile_cpp} PROPERTIES LANGUAGE CXX)
set(timer_buttons_default_default_AVR_GCC_FILE_TYPE_link)
set(timer_buttons_default_default_AVR_GCC_FILE_TYPE_objcopy_ihex)
set(timer_buttons_default_default_AVR_GCC_FILE_TYPE_objcopy_eep)
set(timer_buttons_default_default_AVR_GCC_FILE_TYPE_objcopy_lss)
set(timer_buttons_default_default_AVR_GCC_FILE_TYPE_objcopy_srec)
set(timer_buttons_default_default_AVR_GCC_FILE_TYPE_objcopy_sig)
set(timer_buttons_default_image_name "default.elf")
set(timer_buttons_default_image_base_name "default")


# The output directory of the final image.
set(timer_buttons_default_output_dir "${CMAKE_CURRENT_SOURCE_DIR}/../../../out/timer_buttons")
