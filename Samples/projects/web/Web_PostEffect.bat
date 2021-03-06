if exist ../lib/SampleKit.bc (
    call PreBuild.bat
) else (
    call Web_SampleKit.bat
)

if exist ../../PostEffect/pre.js (
    set PREJS=--pre-js ../../Render2D/pre.js
) else (
    set PREJS=
)

emcc -DEMSCRIPTEN -D__IZ_GLUT__ -D_DEBUG -D__IZANAGI_NO_USE_D3D__ -D__IZ_DEBUG__ -D__IZ_OGL__  -I../../../Libs/include  -I../../SampleKit %LIB%libShader.bc %LIB%libPostEffect.bc %LIB%libMath.bc %LIB%libSceneGraph.bc %LIB%libSystem.bc %LIB%libDebugUtil.bc %LIB%libGraph.bc %LIB%libStd.bc ../lib/libSampleKit.bc ../../PostEffect/main.cpp ../../PostEffect/PostEffectApp.cpp ../../PostEffect/PostEffectSample.cpp ../../PostEffect/SceneRenderer.cpp ../../PostEffect/StateBase.cpp ../../PostEffect/StateBloomStar.cpp ../../PostEffect/StateBloomStarMGF.cpp ../../PostEffect/StateHDR.cpp ../../PostEffect/StateHDR2.cpp ../../PostEffect/StateManager.cpp ../../PostEffect/StateMonotone.cpp ../../PostEffect/StateNegative.cpp ../../PostEffect/StatePostEffect.cpp ../../PostEffect/StateSepia.cpp -o PostEffect.html %PREJS%