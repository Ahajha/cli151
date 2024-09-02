from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMake

class Cli151Conan(ConanFile):
    name = "cli151"
    settings = "os", "arch", "compiler", "build_type"
    generators = "CMakeDeps", "CMakeToolchain"
    
    #def requirements(self):
    #    self.test_requires("catch2/[>=3.5.2 <4]")

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
