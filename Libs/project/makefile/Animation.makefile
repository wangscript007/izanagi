# Compiler flags...
CPP_COMPILER = g++
C_COMPILER = gcc

# Include paths...
Debug_GLUT_Include_Path=-I"../../include" -I"../../../External/glew/include" -I"../../../External/freeglut/include" 

# Library paths...
Debug_GLUT_Library_Path=

# Additional libraries...
Debug_GLUT_Libraries=

# Preprocessor definitions...
Debug_GLUT_Preprocessor_Definitions=-D GCC_BUILD -D _DEBUG -D _LIB -D __IZ_DEBUG__ -D __IZ_OGL__ -D __IZ_GLUT__ -D __IZANAGI_NO_USE_D3D__ 

# Implictly linked object files...
Debug_GLUT_Implicitly_Linked_Objects=

# Compiler flags...
Debug_GLUT_Compiler_Flags=-O0 

# Builds all configurations for this project...
.PHONY: build_all_configurations
build_all_configurations: Debug_GLUT 

# Builds the Debug_GLUT configuration...
.PHONY: Debug_GLUT
Debug_GLUT: create_folders gccDebug_GLUT/source/animation/EasingInterpolator.o gccDebug_GLUT/source/animation/Interpolator.o gccDebug_GLUT/source/animation/LinearInterpolator.o gccDebug_GLUT/source/animation/SplineInterpolator.o gccDebug_GLUT/source/animation/StoryBoard.o gccDebug_GLUT/source/animation/Timeline.o gccDebug_GLUT/source/animation/Tweener.o gccDebug_GLUT/source/animation/TweenerUtility.o 
	ar rcs ../lib/gccDebug_GLUT/libAnimation.a gccDebug_GLUT/source/animation/EasingInterpolator.o gccDebug_GLUT/source/animation/Interpolator.o gccDebug_GLUT/source/animation/LinearInterpolator.o gccDebug_GLUT/source/animation/SplineInterpolator.o gccDebug_GLUT/source/animation/StoryBoard.o gccDebug_GLUT/source/animation/Timeline.o gccDebug_GLUT/source/animation/Tweener.o gccDebug_GLUT/source/animation/TweenerUtility.o  $(Debug_GLUT_Implicitly_Linked_Objects)

# Compiles file ../../source/animation/EasingInterpolator.cpp for the Debug_GLUT configuration...
-include gccDebug_GLUT/source/animation/EasingInterpolator.d
gccDebug_GLUT/source/animation/EasingInterpolator.o: ../../source/animation/EasingInterpolator.cpp
	$(CPP_COMPILER) $(Debug_GLUT_Preprocessor_Definitions) $(Debug_GLUT_Compiler_Flags) -c ../../source/animation/EasingInterpolator.cpp $(Debug_GLUT_Include_Path) -o gccDebug_GLUT/source/animation/EasingInterpolator.o
	$(CPP_COMPILER) $(Debug_GLUT_Preprocessor_Definitions) $(Debug_GLUT_Compiler_Flags) -MM ../../source/animation/EasingInterpolator.cpp $(Debug_GLUT_Include_Path) > gccDebug_GLUT/source/animation/EasingInterpolator.d

# Compiles file ../../source/animation/Interpolator.cpp for the Debug_GLUT configuration...
-include gccDebug_GLUT/source/animation/Interpolator.d
gccDebug_GLUT/source/animation/Interpolator.o: ../../source/animation/Interpolator.cpp
	$(CPP_COMPILER) $(Debug_GLUT_Preprocessor_Definitions) $(Debug_GLUT_Compiler_Flags) -c ../../source/animation/Interpolator.cpp $(Debug_GLUT_Include_Path) -o gccDebug_GLUT/source/animation/Interpolator.o
	$(CPP_COMPILER) $(Debug_GLUT_Preprocessor_Definitions) $(Debug_GLUT_Compiler_Flags) -MM ../../source/animation/Interpolator.cpp $(Debug_GLUT_Include_Path) > gccDebug_GLUT/source/animation/Interpolator.d

# Compiles file ../../source/animation/LinearInterpolator.cpp for the Debug_GLUT configuration...
-include gccDebug_GLUT/source/animation/LinearInterpolator.d
gccDebug_GLUT/source/animation/LinearInterpolator.o: ../../source/animation/LinearInterpolator.cpp
	$(CPP_COMPILER) $(Debug_GLUT_Preprocessor_Definitions) $(Debug_GLUT_Compiler_Flags) -c ../../source/animation/LinearInterpolator.cpp $(Debug_GLUT_Include_Path) -o gccDebug_GLUT/source/animation/LinearInterpolator.o
	$(CPP_COMPILER) $(Debug_GLUT_Preprocessor_Definitions) $(Debug_GLUT_Compiler_Flags) -MM ../../source/animation/LinearInterpolator.cpp $(Debug_GLUT_Include_Path) > gccDebug_GLUT/source/animation/LinearInterpolator.d

# Compiles file ../../source/animation/SplineInterpolator.cpp for the Debug_GLUT configuration...
-include gccDebug_GLUT/source/animation/SplineInterpolator.d
gccDebug_GLUT/source/animation/SplineInterpolator.o: ../../source/animation/SplineInterpolator.cpp
	$(CPP_COMPILER) $(Debug_GLUT_Preprocessor_Definitions) $(Debug_GLUT_Compiler_Flags) -c ../../source/animation/SplineInterpolator.cpp $(Debug_GLUT_Include_Path) -o gccDebug_GLUT/source/animation/SplineInterpolator.o
	$(CPP_COMPILER) $(Debug_GLUT_Preprocessor_Definitions) $(Debug_GLUT_Compiler_Flags) -MM ../../source/animation/SplineInterpolator.cpp $(Debug_GLUT_Include_Path) > gccDebug_GLUT/source/animation/SplineInterpolator.d

# Compiles file ../../source/animation/StoryBoard.cpp for the Debug_GLUT configuration...
-include gccDebug_GLUT/source/animation/StoryBoard.d
gccDebug_GLUT/source/animation/StoryBoard.o: ../../source/animation/StoryBoard.cpp
	$(CPP_COMPILER) $(Debug_GLUT_Preprocessor_Definitions) $(Debug_GLUT_Compiler_Flags) -c ../../source/animation/StoryBoard.cpp $(Debug_GLUT_Include_Path) -o gccDebug_GLUT/source/animation/StoryBoard.o
	$(CPP_COMPILER) $(Debug_GLUT_Preprocessor_Definitions) $(Debug_GLUT_Compiler_Flags) -MM ../../source/animation/StoryBoard.cpp $(Debug_GLUT_Include_Path) > gccDebug_GLUT/source/animation/StoryBoard.d

# Compiles file ../../source/animation/Timeline.cpp for the Debug_GLUT configuration...
-include gccDebug_GLUT/source/animation/Timeline.d
gccDebug_GLUT/source/animation/Timeline.o: ../../source/animation/Timeline.cpp
	$(CPP_COMPILER) $(Debug_GLUT_Preprocessor_Definitions) $(Debug_GLUT_Compiler_Flags) -c ../../source/animation/Timeline.cpp $(Debug_GLUT_Include_Path) -o gccDebug_GLUT/source/animation/Timeline.o
	$(CPP_COMPILER) $(Debug_GLUT_Preprocessor_Definitions) $(Debug_GLUT_Compiler_Flags) -MM ../../source/animation/Timeline.cpp $(Debug_GLUT_Include_Path) > gccDebug_GLUT/source/animation/Timeline.d

# Compiles file ../../source/animation/Tweener.cpp for the Debug_GLUT configuration...
-include gccDebug_GLUT/source/animation/Tweener.d
gccDebug_GLUT/source/animation/Tweener.o: ../../source/animation/Tweener.cpp
	$(CPP_COMPILER) $(Debug_GLUT_Preprocessor_Definitions) $(Debug_GLUT_Compiler_Flags) -c ../../source/animation/Tweener.cpp $(Debug_GLUT_Include_Path) -o gccDebug_GLUT/source/animation/Tweener.o
	$(CPP_COMPILER) $(Debug_GLUT_Preprocessor_Definitions) $(Debug_GLUT_Compiler_Flags) -MM ../../source/animation/Tweener.cpp $(Debug_GLUT_Include_Path) > gccDebug_GLUT/source/animation/Tweener.d

# Compiles file ../../source/animation/TweenerUtility.cpp for the Debug_GLUT configuration...
-include gccDebug_GLUT/source/animation/TweenerUtility.d
gccDebug_GLUT/source/animation/TweenerUtility.o: ../../source/animation/TweenerUtility.cpp
	$(CPP_COMPILER) $(Debug_GLUT_Preprocessor_Definitions) $(Debug_GLUT_Compiler_Flags) -c ../../source/animation/TweenerUtility.cpp $(Debug_GLUT_Include_Path) -o gccDebug_GLUT/source/animation/TweenerUtility.o
	$(CPP_COMPILER) $(Debug_GLUT_Preprocessor_Definitions) $(Debug_GLUT_Compiler_Flags) -MM ../../source/animation/TweenerUtility.cpp $(Debug_GLUT_Include_Path) > gccDebug_GLUT/source/animation/TweenerUtility.d

# Creates the intermediate and output folders for each configuration...
.PHONY: create_folders
create_folders:
	mkdir -p gccDebug_GLUT/source/animation
	mkdir -p ../lib/gccDebug_GLUT

# Cleans intermediate and output files (objects, libraries, executables)...
.PHONY: clean
clean:
	rm -f gccDebug_GLUT/source/animation/*.o
	rm -f gccDebug_GLUT/source/animation/*.d
	rm -f ../lib/gccDebug_GLUT/libAnimation.a

