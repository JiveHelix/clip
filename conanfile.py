from conans import ConanFile, CMake


class ClipConan(ConanFile):
    name = "clip"
    version = "1.1.0"

    scm = {
        "type": "git",
        "url": "https://github.com/JiveHelix/clip.git",
        "revision": "auto",
        "submodule": "recursive"}

    # Optional metadata
    license = "MIT"
    author = "Jive Helix (jivehelix@gmail.com)"
    url = "https://github.com/JiveHelix/clip"
    description = "C++ wrapper around ffmpeg to create short clips from data."
    topics = ("Video encoding", "Computer Graphics")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"

    generators = "cmake"

    no_copy_source = True

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_id(self):
        self.info.header_only()

    def build_requirements(self):
        self.test_requires("catch2/2.13.8")

    def requirements(self):
        self.requires("jive/[~1]")
        self.requires("tau/[~1.1]")
        self.requires("eigen/3.4.0")
        self.requires("zlib/1.2.11")
        self.requires("ffmpeg/4.4")
        self.requires("eigen/3.4.0")
        self.requires("fmt/8.0.1")
