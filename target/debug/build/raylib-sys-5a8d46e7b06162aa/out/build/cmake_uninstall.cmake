if(NOT EXISTS "/home/connor/Documents/chess/target/debug/build/raylib-sys-5a8d46e7b06162aa/out/build/install_manifest.txt")
  message(FATAL_ERROR "Cannot find install manifest: /home/connor/Documents/chess/target/debug/build/raylib-sys-5a8d46e7b06162aa/out/build/install_manifest.txt")
endif()

file(READ "/home/connor/Documents/chess/target/debug/build/raylib-sys-5a8d46e7b06162aa/out/build/install_manifest.txt" files)
string(REGEX REPLACE "\n" ";" files "${files}")
foreach(file ${files})
  message(STATUS "Uninstalling $ENV{DESTDIR}${file}")
  if(IS_SYMLINK "$ENV{DESTDIR}${file}" OR EXISTS "$ENV{DESTDIR}${file}")
    exec_program(
      "/usr/bin/cmake" ARGS "-E remove \"$ENV{DESTDIR}${file}\""
      OUTPUT_VARIABLE rm_out
      RETURN_VALUE rm_retval
      )
    if(NOT "${rm_retval}" STREQUAL 0)
      message(FATAL_ERROR "Problem when removing $ENV{DESTDIR}${file}")
    endif()
  else(IS_SYMLINK "$ENV{DESTDIR}${file}" OR EXISTS "$ENV{DESTDIR}${file}")
    message(STATUS "File $ENV{DESTDIR}${file} does not exist.")
  endif()
endforeach()
