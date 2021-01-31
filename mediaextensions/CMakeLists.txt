set(mediaextensions_src

	"interface/mediaextensionspublic.h"
	"interface/iwallpaperenginemediaextensions.h"

	"include/mp3soundstream.h"
	"include/wallpaperenginemediaextensions.h"

	"src/dllmain.cpp"
	"src/mp3soundstream.cpp"
	"src/wallpaperenginemediaextensions.cpp"
)

include_directories("include")
include_directories("interface")
include_directories("${OP_SFML2_INC}")
include_directories("${OP_MPG123_INC}")

link_directories("${OP_SFML2_LIB}")
link_directories("${OP_SFML2_EXTERNAL_LIB}")
link_directories("${OP_MPG123_LIB}")

set(projoutputname "mediaextensions${BITS}")

ADD_MSVC_PRECOMPILED_HEADER("pch.h" "src/pch.cpp" mediaextensions_src)
add_library(${projoutputname} SHARED ${mediaextensions_src})

target_link_libraries(${projoutputname} optimized libcmt)
target_link_libraries(${projoutputname} flac ogg vorbis vorbisfile Shlwapi openal32)
target_link_libraries(${projoutputname} optimized sfml-system-s debug sfml-system-s-d)
target_link_libraries(${projoutputname} optimized sfml-audio-s debug sfml-audio-s-d)
target_link_libraries(${projoutputname} optimized libmpg123 debug libmpg123d)

target_link_libraries(${projoutputname} Winmm)




ADD_TARGET_OUTPUT_DIR(${projoutputname} "")
ADD_SOURCE_DIRS(mediaextensions_src)