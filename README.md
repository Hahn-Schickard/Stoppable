[![pipeline status](https://git.hahn-schickard.de/hahn-schickard/software-sollutions/application-engineering/internal/cpp-projects/stoppable/badges/master/pipeline.svg)](https://git.hahn-schickard.de/hahn-schickard/software-sollutions/application-engineering/internal/cpp-projects/stoppable/-/commits/master)
[![coverage report](https://git.hahn-schickard.de/hahn-schickard/software-sollutions/application-engineering/internal/cpp-projects/stoppable/badges/master/coverage.svg)](https://git.hahn-schickard.de/hahn-schickard/software-sollutions/application-engineering/internal/cpp-projects/stoppable/-/commits/master)

<img src="docs/code_documentation/vendor-logo.png" alt="" width="200"/>

# Stoppable
## Brief description

A header only implementation of a utility classes that help develop multi-threaded code, requires std::thread library and C++11 support.

### Contains:
 * `Stoppable` - A class that provides functionality to start and stop a given routine 
 * `StoppableTask` - A class that manages `Stoppable` in a separate thread
 * `JobHandler` - A class that cleans up allocated `std::future` instances from `std::async` calls. 

## Required dependencies
* [Python 3.7](https://www.python.org/downloads/release/python-370/)
* [conan](https://docs.conan.io/en/latest/installation.html)
