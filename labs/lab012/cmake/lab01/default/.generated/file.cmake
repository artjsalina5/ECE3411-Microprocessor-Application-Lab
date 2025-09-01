# The following variables contains the files used by the different stages of the build process.
set(lab01_default_toolchain_XC8_3_00_FILE_TYPE_assemble)
set_source_files_properties(${lab01_default_toolchain_XC8_3_00_FILE_TYPE_assemble} PROPERTIES LANGUAGE ASM)

# For assembly files, add "." to the include path for each file so that .include with a relative path works
foreach(source_file ${lab01_default_toolchain_XC8_3_00_FILE_TYPE_assemble})
        set_source_files_properties(${source_file} PROPERTIES INCLUDE_DIRECTORIES "$<PATH:NORMAL_PATH,$<PATH:REMOVE_FILENAME,${source_file}>>")
endforeach()

set(lab01_default_toolchain_XC8_3_00_FILE_TYPE_assemblePreprocess)
set_source_files_properties(${lab01_default_toolchain_XC8_3_00_FILE_TYPE_assemblePreprocess} PROPERTIES LANGUAGE ASM)

# For assembly files, add "." to the include path for each file so that .include with a relative path works
foreach(source_file ${lab01_default_toolchain_XC8_3_00_FILE_TYPE_assemblePreprocess})
        set_source_files_properties(${source_file} PROPERTIES INCLUDE_DIRECTORIES "$<PATH:NORMAL_PATH,$<PATH:REMOVE_FILENAME,${source_file}>>")
endforeach()

set(lab01_default_toolchain_XC8_3_00_FILE_TYPE_compile "${CMAKE_CURRENT_SOURCE_DIR}/../../../main.c")
set_source_files_properties(${lab01_default_toolchain_XC8_3_00_FILE_TYPE_compile} PROPERTIES LANGUAGE C)
set(lab01_default_toolchain_XC8_3_00_FILE_TYPE_link)
set(lab01_default_image_name "default.elf")
set(lab01_default_image_base_name "default")


# The output directory of the final image.
set(lab01_default_output_dir "${CMAKE_CURRENT_SOURCE_DIR}/../../../out/lab01")
