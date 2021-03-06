# Compiler flags...
CPP_COMPILER = g++
C_COMPILER = gcc

# Include paths...
Debug_GLUT_Include_Path=-I"../../../External/glew/include" -I"../../../Libs/include" -I"../../SampleKit" 

# Library paths...
Debug_GLUT_Library_Path=-L"../../../Libs/project/lib/gccDebug_GLUT" -L"SampleKit/gccDebug_GLUT" -L"../../../External/glew/lib" 

# Additional libraries...
Debug_GLUT_Libraries=-Wl,--no-as-needed -Wl,--start-group -lShader -lMath -lSceneGraph -lSystem -lDebugUtil -lGraph -lStd -lSampleKit -lglut -lGLU -lGLEW -lGL -lX11 -lXxf86vm -lXi  -Wl,--end-group

# Preprocessor definitions...
Debug_GLUT_Preprocessor_Definitions=-D GCC_BUILD -D _DEBUG -D __IZANAGI_NO_USE_D3D__ -D __IZ_DEBUG__ -D __IZ_OGL__ -D __IZ_GLUT__ 

# Implictly linked object files...
Debug_GLUT_Implicitly_Linked_Objects=

# Compiler flags...
Debug_GLUT_Compiler_Flags=-O0 -g 

# Builds all configurations for this project...
.PHONY: build_all_configurations
build_all_configurations: Debug_GLUT 

# Builds the Debug_GLUT configuration...
.PHONY: Debug_GLUT
Debug_GLUT: create_folders Shader/gccDebug_GLUT/Shader/main.o Shader/gccDebug_GLUT/Shader/StateBase.o Shader/gccDebug_GLUT/Shader/StateManager.o Shader/gccDebug_GLUT/Shader/StateMirrorMap.o Shader/gccDebug_GLUT/Shader/StateNormalMap.o Shader/gccDebug_GLUT/Shader/StatePhongShader.o Shader/gccDebug_GLUT/Shader/StatePointLight.o Shader/gccDebug_GLUT/Shader/StateSpotLight.o 
	g++ Shader/gccDebug_GLUT/Shader/main.o Shader/gccDebug_GLUT/Shader/StateBase.o Shader/gccDebug_GLUT/Shader/StateManager.o Shader/gccDebug_GLUT/Shader/StateMirrorMap.o Shader/gccDebug_GLUT/Shader/StateNormalMap.o Shader/gccDebug_GLUT/Shader/StatePhongShader.o Shader/gccDebug_GLUT/Shader/StatePointLight.o Shader/gccDebug_GLUT/Shader/StateSpotLight.o  $(Debug_GLUT_Library_Path) $(Debug_GLUT_Libraries) -Wl,-rpath,./ -o ../../gccShader/Shader.exe

# Compiles file ../../Shader/main.cpp for the Debug_GLUT configuration...
-include Shader/gccDebug_GLUT/Shader/main.d
Shader/gccDebug_GLUT/Shader/main.o: ../../Shader/main.cpp
	$(CPP_COMPILER) $(Debug_GLUT_Preprocessor_Definitions) $(Debug_GLUT_Compiler_Flags) -c ../../Shader/main.cpp $(Debug_GLUT_Include_Path) -o Shader/gccDebug_GLUT/Shader/main.o
	$(CPP_COMPILER) $(Debug_GLUT_Preprocessor_Definitions) $(Debug_GLUT_Compiler_Flags) -MM ../../Shader/main.cpp $(Debug_GLUT_Include_Path) > Shader/gccDebug_GLUT/Shader/main.d

# Compiles file ../../Shader/StateBase.cpp for the Debug_GLUT configuration...
-include Shader/gccDebug_GLUT/Shader/StateBase.d
Shader/gccDebug_GLUT/Shader/StateBase.o: ../../Shader/StateBase.cpp
	$(CPP_COMPILER) $(Debug_GLUT_Preprocessor_Definitions) $(Debug_GLUT_Compiler_Flags) -c ../../Shader/StateBase.cpp $(Debug_GLUT_Include_Path) -o Shader/gccDebug_GLUT/Shader/StateBase.o
	$(CPP_COMPILER) $(Debug_GLUT_Preprocessor_Definitions) $(Debug_GLUT_Compiler_Flags) -MM ../../Shader/StateBase.cpp $(Debug_GLUT_Include_Path) > Shader/gccDebug_GLUT/Shader/StateBase.d

# Compiles file ../../Shader/StateManager.cpp for the Debug_GLUT configuration...
-include Shader/gccDebug_GLUT/Shader/StateManager.d
Shader/gccDebug_GLUT/Shader/StateManager.o: ../../Shader/StateManager.cpp
	$(CPP_COMPILER) $(Debug_GLUT_Preprocessor_Definitions) $(Debug_GLUT_Compiler_Flags) -c ../../Shader/StateManager.cpp $(Debug_GLUT_Include_Path) -o Shader/gccDebug_GLUT/Shader/StateManager.o
	$(CPP_COMPILER) $(Debug_GLUT_Preprocessor_Definitions) $(Debug_GLUT_Compiler_Flags) -MM ../../Shader/StateManager.cpp $(Debug_GLUT_Include_Path) > Shader/gccDebug_GLUT/Shader/StateManager.d

# Compiles file ../../Shader/StateMirrorMap.cpp for the Debug_GLUT configuration...
-include Shader/gccDebug_GLUT/Shader/StateMirrorMap.d
Shader/gccDebug_GLUT/Shader/StateMirrorMap.o: ../../Shader/StateMirrorMap.cpp
	$(CPP_COMPILER) $(Debug_GLUT_Preprocessor_Definitions) $(Debug_GLUT_Compiler_Flags) -c ../../Shader/StateMirrorMap.cpp $(Debug_GLUT_Include_Path) -o Shader/gccDebug_GLUT/Shader/StateMirrorMap.o
	$(CPP_COMPILER) $(Debug_GLUT_Preprocessor_Definitions) $(Debug_GLUT_Compiler_Flags) -MM ../../Shader/StateMirrorMap.cpp $(Debug_GLUT_Include_Path) > Shader/gccDebug_GLUT/Shader/StateMirrorMap.d

# Compiles file ../../Shader/StateNormalMap.cpp for the Debug_GLUT configuration...
-include Shader/gccDebug_GLUT/Shader/StateNormalMap.d
Shader/gccDebug_GLUT/Shader/StateNormalMap.o: ../../Shader/StateNormalMap.cpp
	$(CPP_COMPILER) $(Debug_GLUT_Preprocessor_Definitions) $(Debug_GLUT_Compiler_Flags) -c ../../Shader/StateNormalMap.cpp $(Debug_GLUT_Include_Path) -o Shader/gccDebug_GLUT/Shader/StateNormalMap.o
	$(CPP_COMPILER) $(Debug_GLUT_Preprocessor_Definitions) $(Debug_GLUT_Compiler_Flags) -MM ../../Shader/StateNormalMap.cpp $(Debug_GLUT_Include_Path) > Shader/gccDebug_GLUT/Shader/StateNormalMap.d

# Compiles file ../../Shader/StatePhongShader.cpp for the Debug_GLUT configuration...
-include Shader/gccDebug_GLUT/Shader/StatePhongShader.d
Shader/gccDebug_GLUT/Shader/StatePhongShader.o: ../../Shader/StatePhongShader.cpp
	$(CPP_COMPILER) $(Debug_GLUT_Preprocessor_Definitions) $(Debug_GLUT_Compiler_Flags) -c ../../Shader/StatePhongShader.cpp $(Debug_GLUT_Include_Path) -o Shader/gccDebug_GLUT/Shader/StatePhongShader.o
	$(CPP_COMPILER) $(Debug_GLUT_Preprocessor_Definitions) $(Debug_GLUT_Compiler_Flags) -MM ../../Shader/StatePhongShader.cpp $(Debug_GLUT_Include_Path) > Shader/gccDebug_GLUT/Shader/StatePhongShader.d

# Compiles file ../../Shader/StatePointLight.cpp for the Debug_GLUT configuration...
-include Shader/gccDebug_GLUT/Shader/StatePointLight.d
Shader/gccDebug_GLUT/Shader/StatePointLight.o: ../../Shader/StatePointLight.cpp
	$(CPP_COMPILER) $(Debug_GLUT_Preprocessor_Definitions) $(Debug_GLUT_Compiler_Flags) -c ../../Shader/StatePointLight.cpp $(Debug_GLUT_Include_Path) -o Shader/gccDebug_GLUT/Shader/StatePointLight.o
	$(CPP_COMPILER) $(Debug_GLUT_Preprocessor_Definitions) $(Debug_GLUT_Compiler_Flags) -MM ../../Shader/StatePointLight.cpp $(Debug_GLUT_Include_Path) > Shader/gccDebug_GLUT/Shader/StatePointLight.d

# Compiles file ../../Shader/StateSpotLight.cpp for the Debug_GLUT configuration...
-include Shader/gccDebug_GLUT/Shader/StateSpotLight.d
Shader/gccDebug_GLUT/Shader/StateSpotLight.o: ../../Shader/StateSpotLight.cpp
	$(CPP_COMPILER) $(Debug_GLUT_Preprocessor_Definitions) $(Debug_GLUT_Compiler_Flags) -c ../../Shader/StateSpotLight.cpp $(Debug_GLUT_Include_Path) -o Shader/gccDebug_GLUT/Shader/StateSpotLight.o
	$(CPP_COMPILER) $(Debug_GLUT_Preprocessor_Definitions) $(Debug_GLUT_Compiler_Flags) -MM ../../Shader/StateSpotLight.cpp $(Debug_GLUT_Include_Path) > Shader/gccDebug_GLUT/Shader/StateSpotLight.d

# Creates the intermediate and output folders for each configuration...
.PHONY: create_folders
create_folders:
	mkdir -p Shader/gccDebug_GLUT/Shader
	mkdir -p ../../gccShader

# Cleans intermediate and output files (objects, libraries, executables)...
.PHONY: clean
clean:
	rm -f Shader/gccDebug_GLUT/Shader/*.o
	rm -f Shader/gccDebug_GLUT/Shader/*.d
	rm -f ../../gccShader/Shader.exe

