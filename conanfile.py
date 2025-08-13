from conan import ConanFile
from conan.tools.build import check_min_cppstd
from conan.tools.files import load, copy, collect_libs
from conan.tools.cmake import cmake_layout, CMake, CMakeToolchain
import re
import os


class PackageConan(ConanFile):
    # @+ START USER META CONFIG
    license = "Apache 2.0"
    description = "A header only implementation of a utility classes that help develop multi-threaded code"
    topics = ("pattern", "stoppable", "multi-threading", "cpp17")
    settings = "os", "compiler", "build_type", "arch"
    options = {}
    default_options = {}
    default_user = "Hahn-Schickard"
    # @- END USER META CONFIG
    exports = [
        "CMakeLists.txt",
        "conanfile.py"
    ]
    exports_sources = [
        "cmake*",
        "includes*",
        "sources*",
        "CMakeLists.txt",
        "conanfile.py",
        # @+ START USER EXPORTS
        # @- END USER EXPORTS
    ]
    generators = "CMakeDeps"
    package_type = "header-library"
    short_paths = True

    @property
    def cwd(self):
        return os.path.dirname(os.path.realpath(__file__))

    @property
    def full_name(self):
        content = load(self, path=os.path.join(
            self.recipe_folder, 'CMakeLists.txt'))
        return re.search('set\(THIS (.*)\)', content).group(1).strip()

    def set_name(self):
        self.name = self.full_name.lower()

    def validate(self):
        if self.settings.compiler.get_safe("cppstd"):
            check_min_cppstd(self, "17")

    def requirements(self):
        # @+ START USER REQUIREMENTS
        pass
        # @- END USER REQUIREMENTS

    def build_requirements(self):
        self.test_requires("gtest/[~1.16]")
        # @+ START USER BUILD REQUIREMENTS
        # @- END USER BUILD REQUIREMENTS

    def configure(self):
        # @+ START USER REQUIREMENTS OPTION CONFIGURATION
        self.options["gtest/*"].shared = True
        # @- END USER REQUIREMENTS OPTION CONFIGURATION

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.user_presets_path = False
        tc.variables['STATIC_CODE_ANALYSIS'] = False
        tc.variables['RUN_TESTS'] = False
        tc.variables['COVERAGE_TRACKING'] = False
        tc.variables['CMAKE_CONAN'] = False
        # @+ START USER CMAKE OPTIONS
        # @- END USER CMAKE OPTIONS
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
        copy(self, pattern='LICENSE', dst='licenses', src=self.cwd)
        copy(self, pattern='NOTICE', dst='licenses', src=self.cwd)
        copy(self, pattern='AUTHORS', dst='licenses', src=self.cwd)

    def package_info(self):
        self.cpp_info.libs = collect_libs(self)
        self.cpp_info.set_property("cmake_find_mode", "both")
        # @+ START USER DEFINES
        # @- END USER DEFINES
        self.cpp_info.set_property("cmake_file_name", self.full_name)
        cmake_target_name = self.full_name + "::" + self.full_name
        self.cpp_info.set_property("cmake_target_name", cmake_target_name)

    def package_id(self):
        self.info.clear()
