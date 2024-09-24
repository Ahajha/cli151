from conan import ConanFile
from conan.tools.build import valid_min_cppstd
from conan.tools.cmake import cmake_layout, CMake
from conan.tools.microsoft import is_msvc, check_min_vs
from conan.tools.scm import Version

class Cli151Conan(ConanFile):
    name = "cli151"
    settings = "os", "arch", "compiler", "build_type"
    generators = "CMakeDeps", "CMakeToolchain"
    
    @property
    def _has_std_expected(self):
        # check c++23 first
        if not valid_min_cppstd(self, "23"):
            return False

        compiler_version = Version(self.settings.compiler.version)
        if is_msvc(self):
            return check_min_vs(self, "193", raise_invalid=False)
        elif self.settings.compiler == "gcc":
            return compiler_version >= "12"
        elif self.settings.compiler == "clang":
            if self.settings.compiler.libcxx == "libc++":
                return compiler_version >= "16"
            else:
                # https://github.com/llvm/llvm-project/issues/62801
                # __cpp_concepts == 202002 in clang 19
                return compiler_version >= "19"
        elif self.settings.compiler == "apple-clang":
            return compiler_version >= "15"

        # Otherwise, play it safe and assume we don't have it.
        return False

    def requirements(self):
        self.requires("frozen/1.2.0")

        if not self._has_std_expected:
            self.requires("tl-expected/1.1.0")

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
