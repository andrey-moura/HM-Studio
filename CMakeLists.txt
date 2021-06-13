#Require a minimum version
cmake_minimum_required(VERSION 3.10)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)
set (CMAKE_CXX_STANDARD 20)

#Set project name
project(HMStudio)

#Don't try to search under Windows
if(WIN32)
	#wxWidgets
	if(DEFINED ENV{WXWIN})		
		message(STATUS "Found wxWidgets: $ENV{WXWIN}")
		
		# wxWidgets include directory
		include_directories("$ENV{WXWIN}\\include")
		include_directories("$ENV{WXWIN}\\include\\msvc")
		# wxWidgets library folder
		link_directories("$ENV{WXWIN}\\lib\\vc_lib")
	endif()
	if(DEFINED ENV{HUNSPELL})
		message(STATUS "Found Hunspell: $ENV{HUNSPELL}")
		
		# hunspell include directory
		include_directories("$ENV{HUNSPELL}\\src")
		# hunspell library folder
		link_directories("$ENV{Hunspell}\\msvc\\${CMAKE_BUILD_TYPE}\\libhunspell")		
		
		# enable Hunspell
		add_definitions(-DENABLE_SPELL)
	endif()

	# Set characters to Unicode
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /UMBCS /D_UNICODE /DUNICODE")
			
	# Group ALL_BUILD and ZERO_CHECK in one folder
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)
	# Set HMStudio as the startup project
	set_property (DIRECTORY PROPERTY VS_STARTUP_PROJECT "HMStudio")
	# Disable these annoying warnings
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
	#wxWidgets
	find_package(wxWidgets REQUIRED COMPONENTS net core base xml stc)
	include(${wxWidgets_USE_FILE})
	
	find_package(hunspell)
	
	if(DEFINED HUNSPELL_FOUND)
		# hunspell include directory
		include_directories(${HUNSPELL_INCLUDE_DIR})
		# enable Hunspell
		add_definitions(-DENABLE_SPELL)
	endif(DEFINED HUNSPELL_FOUND)	
endif()

include_directories(include)

SET(SRC
	src/FishEditor.cpp
	src/animation_frame.cpp
	src/animator.cpp
	src/animator_editor.cpp
	src/class_app.cpp
	src/class_editor.cpp
	
	src/class_graphics.cpp
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
	src/frame_font_editor.cpp
	src/frame_graphics_editor.cpp
	src/frame_item_editor.cpp
	src/frame_letter_editor.cpp
	src/frame_main.cpp
	src/frame_name_screen.cpp
	src/frame_save_editor.cpp
	src/frame_script_editor.cpp
	src/frame_search_script.cpp
	src/frame_string_editor.cpp
	src/frame_text_block_editor.cpp
	src/frame_value_editor.cpp
	src/palctrl.cpp
	src/window_find_results.cpp
)

SET(HDR
	src/FishEditor.hpp
	src/animation_frame.hpp
	src/animator.hpp
	src/animator_editor.hpp
	src/class_app.hpp
	src/class_editor.hpp
	
	src/class_graphics.hpp
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
	src/frame_font_editor.hpp
	src/frame_graphics_editor.hpp
	src/frame_item_editor.hpp
	src/frame_letter_editor.hpp
	src/frame_main.hpp
	src/frame_name_editor.hpp
	src/frame_save_editor.hpp
	src/frame_script_editor.hpp
	src/frame_search_script.hpp
	src/frame_string_editor.hpp
	src/frame_text_block_editor.hpp
	src/frame_value_editor.hpp
	src/gba_graphics.hpp
	src/palctrl.hpp
	src/window_find_results.hpp
)

if(WIN32)
	add_executable(HMStudio WIN32 ${SRC} ${HDR})	
	
	target_link_libraries(HMStudio libhunspell)
else()
	add_executable(HMStudio ${SRC} ${HDR})	
	
	target_link_libraries(HMStudio ${wxWidgets_LIBRARIES})
	
	if(DEFINED HUNSPELL_FOUND)
		target_link_libraries(HMStudio ${HUNSPELL_LIBRARIES})
	endif()	
endif()
