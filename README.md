NERD Recruitment Tests
======================

- [NERD Recruitment Tests](#nerd-recruitment-tests)
  - [How to build](#how-to-build-c-exercises)
  - [Where to put your code](#where-to-put-your-code)
  - [How to submit your implementation](#how-to-submit-your-implementation)
  - [CMake build system details](#cmake-build-system-details)
    - [GCC/Clang machine flags control](#gccclang-machine-flags-control)
    - [Exercises build options](#exercises-build-options)
    - [Candidate namespace option](#candidate-namespace-option)

How to build C++ exercises
--------------------------

First of all, you need [CMake](http://www.cmake.org).

### CMake Presets

CMake presets for the most common scenarios are provided, you can use them
either on the command line as follows:

```bash
cmake --list-presets
cmake --preset gcc-ninja
cmake --build build/Linux-gcc-ninja --config Debug
```

Alternatively, you can use an IDE with native support for CMake presets, such
as [VS Code](https://code.visualstudio.com/) or [Visual Studio](https://visualstudio.microsoft.com/).

### Window Users

For Visual Studio users under Windows, you can simply run the `GenSolution.cmd`
script and answer the questions.

### Linux Users

For Linux users, a master `Makefile` is provided that should "just work".
  - To build in debug, type `make MODE=debug`.
  - To build with Clang, type `make CC=clang`.

### Raw CMake

You can also use raw CMake commands:

For Make:
```bash
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -B build/debug -S .
cmake --build build/debug
```

For Ninja:
```bash
cmake -G "Ninja Multi-Config" -B build/ninja -S .
cmake --build build/ninja --config Debug
```

Where to put your code
----------------------

We provide a framework for each exercise that typically consists of a `main()`
and a placeholder for the function(s) you have to implement (plus some data
where applicable). That placeholder consists of two files: one `.h` and one
`.cpp`.

You can modify them as you see fit, as long as:

- you don't change the namespace nor the files' names
- it still builds against the unmodified `main()`

Keep in mind that the framework we provide is a minimal one and that we will
likely **put your code to more aggressive testing**.

How to submit your implementation
---------------------------------

- Before submitting, you are required to configure your project again
  (or run any helper you may have used initially to setup the tree).
  This will allow the build system to refresh the manifest of files
  that need to be included that you might have added/worked on for
  your exercises.

    ```bash
    cmake -B build/release -S .
    
    # the file manifest.submission.txt at the root gets updated, you can check
    # the file includes the files you worked on.
    # It will also include the GIT repository files, may you have used versioning
    # this will reviewers to look into your local commits too
    ```

- Build the `Submission` target, this will package your files in a file called
  `Submission-${CANDIDATEID}.zip`.

    ```bash
    cmake --build build/your_config --target Submission --config Release
    ```

You then need to upload it to the same place you downloaded this package from.

CMake build system details
--------------------------

The build system here has been kept as simple as possible, but with time it had
to account for more and more details and options have been added to accomodate
more peculiarities such as better x86 architecture flag controls.

### GCC/Clang machine flags control

For candidates and reviewers using a GCC/Clang based toolchain:

- `COMPILER_ENABLE_avx`: Enable/Disable compiler corresponding -m flag
- `COMPILER_ENABLE_avx2`: Enable/Disable compiler corresponding -m flag
- `COMPILER_ENABLE_avx512f`: Enable/Disable compiler corresponding -m flag
- `COMPILER_ENABLE_bmi2`: Enable/Disable compiler corresponding -m flag
- `COMPILER_ENABLE_fma`: Enable/Disable compiler corresponding -m flag
- `COMPILER_ENABLE_sse4`: Enable/Disable compiler corresponding -m flag

> **__NOTE__** On Linux i386/amd64 hosts, these flags get sensible default values
> grepping the /proc/cpuinfo file. For other hosts you must specify them to avoid
> generating code that generates illegal instructions for your host CPU. Using
> -march=native is not desired because it is then hard to replicate the
> candidate setup. Do not hesitate to communicate your flags if you submit
> solutions to optimization oriented exercises.

### Exercises build options

Depending on the package you received the following options may be at your
disposal to enable/disable each exercise individually:

- `NERD_ENABLE_EXERCISE_DEEPLEARNING_INFERENCE`: Enable/Disable compilation of the
  "Deep Learning Inference" exercise
- `NERD_ENABLE_EXERCISE_LECOMPTEESTBON`: Enable/Disable compilation of the
  "Le compte est Bon" exercise
- `NERD_ENABLE_EXERCISE_MOMOMOTUS`: Enable/Disable compilation of the
  "MoMoMotus" exercise
- `NERD_ENABLE_EXERCISE_ROTATION`: Enable/Disable compilation of the
  "Rotation" exercise
- `NERD_ENABLE_EXERCISE_STARSYSTEM`: Enable/Disable compilation of the
  "Star System" exercise
- `NERD_ENABLE_EXERCISE_T10`: Enable/Disable compilation of the "T10"
  exercise
- `NERD_ENABLE_EXERCISE_CONCURRENCY`: Enable/Disable compilation of the concurrency
  exercise
- `NERD_ENABLE_EXERCISE_RIGID_BODY_PHYSICS`: Enable/Disable compilation of
  the rigid body physics exercise

### Candidate namespace option

Controlling the candidate NAMESPACE. Useful for reviewers more than the candidates:

- `NERD_CANDIDATE`: set it to the namespace wished to be tested. The special
  value NONE disables this override and fallbacks to the code default value
  specified in the compilation units