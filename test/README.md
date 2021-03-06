# Orange Tests

When running `orange test`, Orange will test programs and projects in the `test/`
subdirectory of the Orange project you're currently in.

When running tests, any non-zero value a program returns is considered a fail.
Tests are written in a way such that if a feature was implemented incorrectly,
they would return a non-zero value.

For example, adding `3.3` and `2` should return a `double`, not an `int`. So,
if the result of `3.3 + 2` is less than `5.3`, it was casted to an int. In this
case, the test would return 1, a fail.

# Test Precedence

The tests in each subdirectory have been arranged to have the following
precedence, from lowest to highest:

```
empty: test empty files, files with nothing but comments
return: test return statements
math: test constants, binary operators
functions: test functions that aren't generic
variables: test variables
generic_functions: test generic functions
if: test if statements
recursion: test recursive functions
loops: test any kind of loop structure
pointers: test pointers
array: test arrays
ternary: test ternary statement
enum: test enums
builtin: builtin function tests.
programs: test programs
bugs: test bugs found at some point
```

That is to say, tests in `array/` depend on tests in `loops/` and `if/` passing,
as `array/` has a higher precedence.

The `unit/` folder contains extra tests for the compiler as a whole, including
tests for orange programs that should fail, which cannot be covered here as
all tests in this folder are expected to pass.
