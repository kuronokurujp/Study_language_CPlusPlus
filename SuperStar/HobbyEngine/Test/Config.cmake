set(ENGINE_TEST_NAME HobbyEngine_UnitTests)

# テストソースをまとめる（複数ファイルがあればここに追加）
set(ENGINE_TARGET_TEST_SRC_FILES
    AsyncTask/TestAsyncTask.cpp

    Common/TestArray.cpp
    Common/TestHash.cpp
    Common/TestLambda.cpp
    Common/TestList.cpp
    Common/TestMacro.cpp
    Common/TestMap.cpp
    Common/TestPoolManager.cpp
    Common/TestStack.cpp
    Common/TestString.cpp
    Common/TestUUID.cpp
    Common/TestVector.cpp

    Math/TestMath.cpp

    Memory/TestMemory.cpp

    Time/TestFPS.cpp

    Task/TestTask.cpp
)
