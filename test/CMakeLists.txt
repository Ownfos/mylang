find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

set(testlist common file lexer parser)
foreach(test ${testlist})
    add_executable(${test} ${test}.cpp)
    target_link_libraries(${test} PRIVATE mylanglib GTest::gtest_main)
    gtest_discover_tests(${test})
endforeach()
