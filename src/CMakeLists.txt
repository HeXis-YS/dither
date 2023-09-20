cmake_minimum_required(VERSION 3.12)
project(Dither)  # Replace with your project's name

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

# CUSTOM ZLIB-NG BUILD
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/zlib-ng/zconf.h.in ${CMAKE_CURRENT_BINARY_DIR}/zlib-ng/zconf.h COPYONLY)
# essentially, sed 's/@ZLIB_SYMBOL_PREFIX@//g' zlib.h.in >zlib.h
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/zlib-ng/zlib.h.in ${CMAKE_CURRENT_BINARY_DIR}/zlib-ng/zlib.h @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/zlib-ng/zlib_name_mangling.h.empty ${CMAKE_CURRENT_BINARY_DIR}/zlib-ng/zlib_name_mangling.h COPYONLY)
add_library(libz STATIC
    zlib-ng/adler32.c
    zlib-ng/adler32_fold.c
    zlib-ng/adler32_p.h
    zlib-ng/chunkset.c
    zlib-ng/chunkset_tpl.h
    zlib-ng/compare256.c
    zlib-ng/compare256_rle.h
    zlib-ng/compress.c
    zlib-ng/cpu_features.c
    zlib-ng/cpu_features.h
    zlib-ng/crc32_braid.c
    zlib-ng/crc32_braid_comb.c
    zlib-ng/crc32_braid_comb_p.h
    zlib-ng/crc32_braid_p.h
    zlib-ng/crc32_braid_tbl.h
    zlib-ng/crc32_fold.c
    zlib-ng/crc32_fold.h
    zlib-ng/deflate.c
    zlib-ng/deflate.h
    zlib-ng/deflate_fast.c
    zlib-ng/deflate_huff.c
    zlib-ng/deflate_medium.c
    zlib-ng/deflate_p.h
    zlib-ng/deflate_quick.c
    zlib-ng/deflate_rle.c
    zlib-ng/deflate_slow.c
    zlib-ng/deflate_stored.c
    zlib-ng/functable.c
    zlib-ng/functable.h
    zlib-ng/infback.c
    zlib-ng/inffast_tpl.h
    zlib-ng/inffixed_tbl.h
    zlib-ng/inflate.c
    zlib-ng/inflate.h
    zlib-ng/inflate_p.h
    zlib-ng/inftrees.c
    zlib-ng/inftrees.h
    zlib-ng/insert_string.c
    zlib-ng/insert_string_roll.c
    zlib-ng/insert_string_tpl.h
    zlib-ng/match_tpl.h
    zlib-ng/slide_hash.c
    zlib-ng/trees.c
    zlib-ng/trees.h
    zlib-ng/trees_emit.h
    zlib-ng/trees_tbl.h
    zlib-ng/uncompr.c
    zlib-ng/zbuild.h
    zlib-ng/zendian.h
    zlib-ng/zutil.c
    zlib-ng/zutil.h
    ${CMAKE_CURRENT_BINARY_DIR}/zlib-ng/zconf.h
    ${CMAKE_CURRENT_BINARY_DIR}/zlib-ng/zlib.h
    ${CMAKE_CURRENT_BINARY_DIR}/zlib-ng/zlib_name_mangling.h)
target_include_directories(libz PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/zlib-ng)

if (HAVE_UNISTD_H)
    set(Z_HAVE_UNISTD_H "Z_HAVE_UNISTD_H")
endif()
target_compile_definitions(libz PRIVATE ZLIB_COMPAT;$<$<NOT:$<C_COMPILER_ID:MSVC>>:HAVE_ATTRIBUTE_ALIGNED>)
target_compile_definitions(libz PUBLIC ${Z_HAVE_UNISTD_H})

# CUSTOM LIBPNG BUILD
file(COPY libpng/scripts/pnglibconf.h.prebuilt libpng/png.h libpng/pngconf.h DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/libpng)
file(RENAME ${CMAKE_CURRENT_BINARY_DIR}/libpng/pnglibconf.h.prebuilt ${CMAKE_CURRENT_BINARY_DIR}/libpng/pnglibconf.h)
add_library(libpng STATIC
    libpng/png.c
    libpng/png.h
    libpng/pngerror.c
    libpng/pngget.c
    libpng/pngmem.c
    libpng/pngpread.c
    libpng/pngread.c
    libpng/pngrio.c
    libpng/pngrtran.c
    libpng/pngrutil.c
    libpng/pngset.c
    libpng/pngtrans.c
    libpng/pngwio.c
    libpng/pngwrite.c
    libpng/pngwtran.c
    libpng/pngwutil.c)
target_include_directories(libpng PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/libpng)
target_link_libraries(libpng PRIVATE libz)

# Executable
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Getopt-for-Visual-Studio")
add_executable(dither
    MCQuantization.c
    DTDither.c
    DTImage.c
    DTPalette.c
    main.c
    include/DTDither.h
    include/DTImage.h
    include/DTPalette.h
    include/MCQuantization.h
)

# Link libraries
target_link_libraries(dither PRIVATE libpng)
