from conan import ConanFile
import os, sys, platform


class EfanoRecipe(ConanFile):
    name = "efano"

    #+++++++++++-+-+--+----- --- -- -  -  -   -
    # Optional metadata
    #
    license = "UNLICENSED"

    author = ""

    url = ""

    description = ""

    topics = ("",)
    #
    #+++++++++++-+-+--+----- --- -- -  -  -   -

    python_requires = "cor_recipe_utils/0.18.3"
    python_requires_extend = "cor_recipe_utils.ConanFileBase"

    settings = "os", "compiler", "build_type", "arch"

    exports_sources = [
        "CMakeLists.txt",
        "**/CMakeLists.txt",
        "src/*.hpp",
        "src/**/*.hpp",
        "src/*.ipp",
        "src/**/*.ipp",
        "src/*.cpp",
        "src/**/*.cpp",
    ]

    #+++++++++++-+-+--+----- --- -- -  -  -   -

    def requirements(self):
        VISIBLE = self.cor.VISIBLE
        OVERRIDE = self.cor.OVERRIDE

        self.requires("boost/1.90.0", **VISIBLE)
        self.requires("batteries/0.65.0", **VISIBLE)
        self.requires("xxhash/[>=0.8.3 <1]", **VISIBLE)

    def build_requirements(self):
        self.tool_requires("cmake/[>=3.20.0 <4]")
        self.tool_requires("ninja/[>=1.12 <2]")
        self.test_requires("gtest/[>=1.14.0 <2]")

    #+++++++++++-+-+--+----- --- -- -  -  -   -

    def configure(self):
        self.cor.configure_default(self)

    def config_options(self):
        self.cor.config_options_default(self)

    def set_version(self):
        self.cor.set_version_from_git_tags(self)

    def layout(self):
        self.cor.layout_cmake_unified_src(self)

    def generate(self):
        self.cor.generate_cmake_default(self)

    def build(self):
        self.cor.build_cmake_default(self)

    def package(self):
        self.cor.package_cmake_lib_default(self)

    def package_info(self):
        self.cor.package_info_lib_default(self)

    def package_id(self):
        self.cor.package_id_lib_default(self)

    #+++++++++++-+-+--+----- --- -- -  -  -   -
