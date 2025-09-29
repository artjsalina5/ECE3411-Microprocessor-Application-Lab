# The following variables contains the files used by the different stages of the build process.
set(circular_buffers_uart_default_default_AVR_GCC_FILE_TYPE_assemble)
set_source_files_properties(${circular_buffers_uart_default_default_AVR_GCC_FILE_TYPE_assemble} PROPERTIES LANGUAGE ASM)

# For assembly files, add "." to the include path for each file so that .include with a relative path works
foreach(source_file ${circular_buffers_uart_default_default_AVR_GCC_FILE_TYPE_assemble})
        set_source_files_properties(${source_file} PROPERTIES INCLUDE_DIRECTORIES "$<PATH:NORMAL_PATH,$<PATH:REMOVE_FILENAME,${source_file}>>")
endforeach()

set(circular_buffers_uart_default_default_AVR_GCC_FILE_TYPE_assembleWithPreprocess)
set_source_files_properties(${circular_buffers_uart_default_default_AVR_GCC_FILE_TYPE_assembleWithPreprocess} PROPERTIES LANGUAGE ASM)

# For assembly files, add "." to the include path for each file so that .include with a relative path works
foreach(source_file ${circular_buffers_uart_default_default_AVR_GCC_FILE_TYPE_assembleWithPreprocess})
        set_source_files_properties(${source_file} PROPERTIES INCLUDE_DIRECTORIES "$<PATH:NORMAL_PATH,$<PATH:REMOVE_FILENAME,${source_file}>>")
endforeach()

set(circular_buffers_uart_default_default_AVR_GCC_FILE_TYPE_compile
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../include/circularbuff.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../include/uart.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../main.c")
set_source_files_properties(${circular_buffers_uart_default_default_AVR_GCC_FILE_TYPE_compile} PROPERTIES LANGUAGE C)
set(circular_buffers_uart_default_default_AVR_GCC_FILE_TYPE_compile_cpp)
set_source_files_properties(${circular_buffers_uart_default_default_AVR_GCC_FILE_TYPE_compile_cpp} PROPERTIES LANGUAGE CXX)
set(circular_buffers_uart_default_default_AVR_GCC_FILE_TYPE_link)
set(circular_buffers_uart_default_default_AVR_GCC_FILE_TYPE_objcopy_ihex)
set(circular_buffers_uart_default_default_AVR_GCC_FILE_TYPE_objcopy_eep)
set(circular_buffers_uart_default_default_AVR_GCC_FILE_TYPE_objcopy_lss)
set(circular_buffers_uart_default_default_AVR_GCC_FILE_TYPE_objcopy_srec)
set(circular_buffers_uart_default_default_AVR_GCC_FILE_TYPE_objcopy_sig)
set(circular_buffers_uart_default_image_name "default.elf")
set(circular_buffers_uart_default_image_base_name "default")


# The output directory of the final image.
set(circular_buffers_uart_default_output_dir "${CMAKE_CURRENT_SOURCE_DIR}/../../../out/circular_buffers_uart")
