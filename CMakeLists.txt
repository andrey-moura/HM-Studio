#Require a minimum version
cmake_minimum_required(VERSION 3.12)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)
set (CMAKE_CXX_STANDARD 17)

#Set project name
project(HMStudio)

#wxWidgets
find_package(wxWidgets REQUIRED COMPONENTS net core base xml stc)
find_package(libhunspell)

include(${wxWidgets_USE_FILE})

if(DEFINED HUNSPELL_FOUND)
	include(${HUNSPELL_INCLUDE_DIR})
	add_definitions(-DENABLE_SPELL)
endif(DEFINED HUNSPELL_FOUND)

include_directories(include)

SET(SRC
	src/FishEditor.cpp
	src/class_app.cpp
	src/class_editor.cpp
	src/class_finder.cpp
	src/class_graphics.cpp
	src/class_graphics_editor.cpp
	src/class_item.cpp
	src/class_item_editor.cpp
	src/class_letter.cpp
	src/class_letter_editor.cpp
	src/class_rom_file.cpp
	src/class_script.cpp
	src/class_script_editor.cpp
	src/class_spell_checker.cpp
	src/class_stc.cpp
	src/class_var_table.cpp
	src/control_graphics_view.cpp
	src/dialog_dump_insert.cpp
	src/dialog_get_text_from.cpp
	src/dialog_spellchecker.cpp
	src/frame_editor.cpp
	src/frame_graphics_editor.cpp
	src/frame_item_editor.cpp
	src/frame_letter_editor.cpp
	src/frame_main.cpp
	src/frame_save_editor.cpp
	src/frame_script_editor.cpp
	src/frame_search_script.cpp
	src/frame_string_editor.cpp
	src/frame_text_block_editor.cpp
	src/frame_value_editor.cpp
	src/graphicsinfoviewer.cpp
	src/namespace_math.cpp
	src/palctrl.cpp
	src/window_find_results.cpp
)

SET(HDR
	src/FishEditor.hpp
	src/class_app.hpp
	src/class_editor.hpp
	src/class_finder.hpp
	src/class_graphics.hpp
	src/class_graphics_editor.hpp
	src/class_item.hpp
	src/class_item_editor.hpp
	src/class_letter.hpp
	src/class_letter_editor.hpp
	src/class_rom_file.hpp
	src/class_script.hpp
	src/class_script_editor.hpp
	src/class_spell_checker.hpp
	src/class_stc.hpp
	src/class_var_table.hpp
	src/control_graphics_view.hpp
	src/dialog_dump_insert.hpp
	src/dialog_get_text_from.hpp
	src/dialog_spellchecker.hpp
	src/frame_editor.hpp
	src/frame_graphics_editor.hpp
	src/frame_item_editor.hpp
	src/frame_letter_editor.hpp
	src/frame_main.hpp
	src/frame_save_editor.hpp
	src/frame_script_editor.hpp
	src/frame_search_script.hpp
	src/frame_string_editor.hpp
	src/frame_text_block_editor.hpp
	src/frame_value_editor.hpp
	src/graphicsinfoviewer.hpp
	src/namespace_math.hpp
	src/palctrl.hpp
	src/window_find_results.hpp
)

#Add the executable
add_executable(HMStudio ${SRC} ${HDR})

target_link_libraries(HMStudio ${wxWidgets_LIBRARIES})