if(EXISTS "/workspaces/codespaces-blank/meu-vst3/build/Tests")
  if(NOT EXISTS "/workspaces/codespaces-blank/meu-vst3/build/Tests-b12d07c_tests.cmake" OR
     NOT "/workspaces/codespaces-blank/meu-vst3/build/Tests-b12d07c_tests.cmake" IS_NEWER_THAN "/workspaces/codespaces-blank/meu-vst3/build/Tests" OR
     NOT "/workspaces/codespaces-blank/meu-vst3/build/Tests-b12d07c_tests.cmake" IS_NEWER_THAN "${CMAKE_CURRENT_LIST_FILE}")
    include("/workspaces/codespaces-blank/meu-vst3/build/_deps/catch2-src/extras/CatchAddTests.cmake")
    catch_discover_tests_impl(
      TEST_EXECUTABLE [==[/workspaces/codespaces-blank/meu-vst3/build/Tests]==]
      TEST_EXECUTOR [==[]==]
      TEST_WORKING_DIR [==[/workspaces/codespaces-blank/meu-vst3/build]==]
      TEST_SPEC [==[]==]
      TEST_EXTRA_ARGS [==[]==]
      TEST_PROPERTIES [==[SKIP_RETURN_CODE;4]==]
      TEST_PREFIX [==[]==]
      TEST_SUFFIX [==[]==]
      TEST_LIST [==[Tests_TESTS]==]
      TEST_REPORTER [==[]==]
      TEST_OUTPUT_DIR [==[]==]
      TEST_OUTPUT_PREFIX [==[]==]
      TEST_OUTPUT_SUFFIX [==[]==]
      CTEST_FILE [==[/workspaces/codespaces-blank/meu-vst3/build/Tests-b12d07c_tests.cmake]==]
      TEST_DL_PATHS [==[]==]
      TEST_DL_FRAMEWORK_PATHS [==[]==]
      ADD_TAGS_AS_LABELS [==[FALSE]==]
    )
  endif()
  include("/workspaces/codespaces-blank/meu-vst3/build/Tests-b12d07c_tests.cmake")
else()
  add_test(Tests_NOT_BUILT Tests_NOT_BUILT)
endif()
