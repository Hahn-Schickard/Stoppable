# Changelog
## [0.3.1] - 2025.09.18
### Changed
 - gtest to v 1.17

## [0.3.0] - 2025.09.02
### Added 
 - Installation documentation
 - `StopToken` class

### Removed 
 - `JobHandler.hpp`

### Changed
 - `Stoppable.hpp` into `Routine.hpp`
 - `Stoppable` class into `Routine` class
 - `StoppableTask.hpp` into `Task.hpp`
 - `StoppableTask` class into `Task` class
 - unit tests
 - integration test
 - contribution guide
 - basic usage documentation
 - compiler hardening settings
 - Conan recipe to support c++11 compilers
 - Conan package type to compiled library

## [0.2.8] - 2025.05.23
### Added
  - compiler hardening settings
  
### Changed
 - gtest to v 1.16

## [0.2.7] - 2024.08.05
### Added
 - windows support
 - example runner 

### Removed
 - `FIND_INSTALLED_DEPENDENCIES` macro usage from root `CMakeLists.txt`
 - `WRITE_PKG_CONFIG_IN_FILE` macro usage from root `CMakeLists.txt`

## [0.2.6] - 2023.07.28 
### Added 
 - usage example in documentation intro

### Removed 
 - code coverage hooks from conan package 

## [0.2.5] - 2023.07.20
### Changed
 - conan recipe to use conan v2 syntax
 - CMake requirement to 3.24
 - conan cmake integration to use conan v2 engine

## [0.2.4] - 2022.11.16
### Changed
 - conan packaging recipe
 - gtest dependency to fuzzy v1.11

## [0.2.3] - 2022.02.03
### Fixed
 - missing <thread> include in JobHander.hpp

### Added
 - find_package(Threads) to library CMakeLists.txt
 - NOLINT markers for GTest macros
 - JobHandler to conan integration test

### Changed
 - JobHandlerTests.cc `rethrowException()` to use **const &** instead of value
 - JobHandlerTests.cc `FakeException::()(const shared_ptr<bool> &completed)` to use const & instead of value
 - StoppableTaskTests.cc to be compliant with readability rules in clang-tidy
 - conan integration test to better test StoppableTask

## [0.2.2] - 2021.07.01
### Fixed
 - install include directory location

## [0.2.1] - 2021.07.01
### Changed
 - JobHandler `std::exception` to ellipsis operator to handle anything that can be thrown
 - JobHandler `tryClean()` method to use `remove_if()` idiom
 - JobHandler `clean()` method to use the correct erase pattern
 - JobHandler `std::deque` to `std::list` for faster mid-container erases

### Added
 - canHandleExceptionAndStartNewJob test case

## [0.2.0] - 2021.06.02
### Added
 - JobHandler
 - JobHandlerTests
 - StoppablePtr alias
 - StoppableTask destructor to stop the task before destruction is finished

### Changed
 - StoppableTask to use StoppablePtr instead of `std::unique_ptr<Stoppable>`
 - `StoppableTask::startTask` to return bool instead of throwing an exception
 - `StoppableTask::stopTask` to return bool instead of throwing an exception

## [0.1.0] - 2020.09.29
### Added
 - Stoppable
 - StoppableTask
 - unit tests
 - conan packaging
 - conan package integration test
 - documentation
 - LICENSE and NOTICE files
 - Contributing guide
 - Authors
