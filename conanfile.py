from conan import ConanFile


class ClipConan(ConanFile):
    name = "clip"
    version = "1.3.0"

    python_requires = "boiler/0.1"
    python_requires_extend = "boiler.HeaderConanFile"

    license = "MIT"
    author = "Jive Helix (jivehelix@gmail.com)"
    url = "https://github.com/JiveHelix/clip"
    description = "C++ wrapper around ffmpeg to create short clips from data."
    topics = ("Video encoding", "Computer Graphics")

    def configure(self):
        if self.settings.os == "Linux":
            self.options["ffmpeg"].with_pulse = False
            self.options["ffmpeg"].with_openjpeg = False
            self.options["ffmpeg"].with_libx264 = False
            self.options["ffmpeg"].with_libx265 = False

    def build_requirements(self):
        self.test_requires("catch2/2.13.8")

    def requirements(self):
        self.requires("tau/[>=1.14 <2]")
        self.requires("fmt/[~10]")
        self.requires("ffmpeg/[~6]")
