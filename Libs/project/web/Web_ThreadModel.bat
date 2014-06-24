call PreBuild.bat

emcc -DEMSCRIPTEN -D__IZ_GLUT__ -D_DEBUG -D_LIB -D__IZ_DEBUG__ -D__IZ_OGL__ -D__IZANAGI_NO_USE_D3D__ -I../../include ../../source/threadmodel/Parallel.cpp ../../source/threadmodel/ThreadModelTask.cpp ../../source/threadmodel/ThreadPool.cpp ../../source/threadmodel/TimerThread.cpp ../../source/threadmodel/ThreadModelJob.cpp ../../source/threadmodel/ThreadModelJobQueue.cpp ../../source/threadmodel/ThreadModelJobWorker.cpp ../../source/threadmodel/ThreadModelMessageQueue.cpp -o ../lib/libThreadModel.bc