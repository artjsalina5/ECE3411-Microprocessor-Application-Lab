# The following variables contains the files used by the different stages of the build process.
set(labtest1new_default_default_XC8_FILE_TYPE_assemble)
set_source_files_properties(${labtest1new_default_default_XC8_FILE_TYPE_assemble} PROPERTIES LANGUAGE ASM)

# For assembly files, add "." to the include path for each file so that .include with a relative path works
foreach(source_file ${labtest1new_default_default_XC8_FILE_TYPE_assemble})
        set_source_files_properties(${source_file} PROPERTIES INCLUDE_DIRECTORIES "$<PATH:NORMAL_PATH,$<PATH:REMOVE_FILENAME,${source_file}>>")
endforeach()

set(labtest1new_default_default_XC8_FILE_TYPE_assemblePreprocess)
set_source_files_properties(${labtest1new_default_default_XC8_FILE_TYPE_assemblePreprocess} PROPERTIES LANGUAGE ASM)

# For assembly files, add "." to the include path for each file so that .include with a relative path works
foreach(source_file ${labtest1new_default_default_XC8_FILE_TYPE_assemblePreprocess})
        set_source_files_properties(${source_file} PROPERTIES INCLUDE_DIRECTORIES "$<PATH:NORMAL_PATH,$<PATH:REMOVE_FILENAME,${source_file}>>")
endforeach()

set(labtest1new_default_default_XC8_FILE_TYPE_compile "${CMAKE_CURRENT_SOURCE_DIR}/../../../main.c")
set_source_files_properties(${labtest1new_default_default_XC8_FILE_TYPE_compile} PROPERTIES LANGUAGE C)
set(labtest1new_default_default_XC8_FILE_TYPE_link)
set(labtest1new_default_image_name "default.elf")
set(labtest1new_default_image_base_name "default")


# The output directory of the final image.
set(labtest1new_default_output_dir "${CMAKE_CURRENT_SOURCE_DIR}/../../../out/labtest1new")
