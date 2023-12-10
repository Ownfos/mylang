#include <gtest/gtest.h>

TEST(CodeGenerator, EmptyModule)
{
    // TODO: write test case
    /*
    ```source code
    module a;
    ```

    ==>

    ```a.h
    #ifndef MODULE_a_H
    #define MOUDLE_a_H

    #endif MODULE_a_H
    ```

    ```a.cpp
    #include "a.h"
    ```
    */
   FAIL();
}

TEST(CodeGenerator, SinglePrivateFunction)
{
    // TODO: write test case
    /*
    ```source code
    module a;

    foo: func = () {
        i: i32 = 0;
    }
    ```

    ==>

    ```a.h
    #ifndef MODULE_a_H
    #define MOUDLE_a_H

    #endif MODULE_a_H
    ```

    ```a.cpp
    #include "a.h"

    void foo()
    {
        int i = 0;
    }
    ```
    */
   FAIL();
}