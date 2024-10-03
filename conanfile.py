from conan import ConanFile
from conan.errors import ConanInvalidConfiguration
from conan.tools.build import check_min_cppstd, valid_min_cppstd
from conan.tools.cmake import cmake_layout, CMake
from conan.tools.microsoft import is_msvc, check_min_vs
from conan.tools.scm import Version

class Cli151Conan(ConanFile):
    name = "cli151"
    settings = "os", "arch", "compiler", "build_type"
    generators = "CMakeDeps", "CMakeToolchain"

    def validate(self):
        # check c++20 first
        check_min_cppstd(self, "20")

        compiler_version = Version(self.settings.compiler.version)
        if is_msvc(self):
            check_min_vs(self, "192")
        elif self.settings.compiler == "gcc":
            if compiler_version < "10":
                # Requires concepts support
                raise ConanInvalidConfiguration("GCC<10 is not supported")
        elif self.settings.compiler == "clang":
            if self.settings.compiler.libcxx == "libc++":
                if compiler_version < "13":
                    # Requires concepts support in libc++
                    raise ConanInvalidConfiguration("Clang<13 with libc++ is not supported")
            else:
                if compiler_version < "10":
                    # Requires concepts support in libstdc++, optimistically we assume this is the case.
                    raise ConanInvalidConfiguration("Clang<10 is not supported")
        elif self.settings.compiler == "apple-clang":
            if compiler_version < "13.1.6":
                # 13.1.6 maps to LLVM 13, and uses libc++. Before this concepts aren't supported.
                raise ConanInvalidConfiguration("Apple Clang<13.1.6 is not supported")

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

    @property
    def _has_std_from_chars(self):
        # This is redundant since this library requires C++20, but just for completion.
        if not valid_min_cppstd(self, "17"):
            return False

        compiler_version = Version(self.settings.compiler.version)
        if is_msvc(self):
            # 16.4, in VS2019
            return check_min_vs(self, "192", raise_invalid=False)
        elif self.settings.compiler == "gcc":
            # TODO: It is technically available in GCC11, but the implementation
            # might be slower. It's probably okay to use the std version still, but
            # need to make a final decision.
            return compiler_version >= "11"
        # Neither clang nor clang has support for floating point from_chars (as of clang 20).
        elif self.settings.compiler == "clang":
            # For the libstdc++ case, it depends on the installed version, which isn't typically
            # modeled in conan. To avoid going into the complexity of adding a setting, just assume
            # no, and always fall back to fast_float. 
            return False
        elif self.settings.compiler == "apple-clang":
            return False
        
        # Otherwise, play it safe and assume we don't have it.
        return False

    def requirements(self):
        self.requires("frozen/1.2.0")

        if not self._has_std_expected:
            self.requires("tl-expected/1.1.0")
        if not self._has_std_from_chars:
            self.requires("fast_float/6.1.5")

        self.test_requires("doctest/2.4.11")

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
