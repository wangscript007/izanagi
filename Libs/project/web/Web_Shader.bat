call PreBuild.bat

emcc -DEMSCRIPTEN -D__IZ_GLUT__ -D_DEBUG -D_LIB -D__IZ_DEBUG__ -D__IZ_OGL__ -D__IZANAGI_NO_USE_D3D__ -I../../include ../../source/shader/ShaderManager.cpp ../../source/shader/ShaderUtil.cpp ../../source/shader/base/ShaderPass.cpp ../../source/shader/base/ShaderBasic.cpp ../../source/shader/base/table/ShaderAttrTable.cpp ../../source/shader/base/table/ShaderParameterTable.cpp ../../source/shader/base/table/ShaderPassTable.cpp ../../source/shader/base/table/ShaderSamplerTable.cpp -o ../lib/libShader.bc