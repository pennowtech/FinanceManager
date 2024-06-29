# ----------------------------------------------------------------------
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <https://www.gnu.org/licenses/>.
# ----------------------------------------------------------------------

from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.files import copy
from conan.tools.cmake import CMakeToolchain
import os

class FinanceManagerAppConan(ConanFile):
    name = 'FinanceManager'
    version = '0.1'
    settings = 'os', 'compiler', 'build_type', 'arch'
    options = {'with_tests': [True, False]}
    default_options = {'with_tests': False}
    generators = 'CMakeDeps'
    exports_sources = 'src/*', 'qml/*', 'CMakeLists.txt'

    def configure(self):
        if self.settings.compiler.cppstd:
            self.settings.compiler.cppstd = 20

    def requirements(self):
        self.requires('redis-plus-plus/1.3.12')
        self.requires('cassandra-cpp-driver/2.17.1')
        self.requires('libpqxx/7.9.1')
        self.requires('quill/4.4.1')
        self.requires('drogon/1.9.5')
        self.requires('cpp-httplib/0.16.0')
        self.requires('nlohmann_json/3.11.3')
        self.requires('grpc/1.54.3')
        self.requires('c-ares/1.25.0', override=True) #required by grpc. override to resolve version conflict during conan install
        self.requires('jwt-cpp/0.7.0')
        self.requires('uwebsockets/20.63.0')
        self.requires('libenvpp/1.4.1')
        self.requires('cxxopts/3.2.0')

        if self.options.with_tests:
            self.output.info(f'SDSINGH2: Installing Gtest')
            self.requires('gtest/1.14.0')

    def layout(self):
        cmake_layout(self)
        # self.folders.build = "build"
        # self.folders.generators = "build/generators"

    def generate(self):
        # super().generate()
        tc = CMakeToolchain(self)
        tc.variables['CMAKE_BUILD_TYPE'] = self.settings.build_type
        tc.variables['CMAKE_EXPORT_COMPILE_COMMANDS'] = True
        tc.generate()

        self._setup_dependencies()

    def _setup_dependencies(self):
        base_build_folder = os.path.join(self.build_folder, '..')
        build_folder = os.path.join(base_build_folder, 'bin')
        lib_folder = os.path.join(base_build_folder, 'lib')
        qml_folder = os.path.join(base_build_folder, 'qml')
        plugins_folder = os.path.join(base_build_folder, 'plugins')

        self.output.info(f'Build folder QT: {build_folder}')
        pass

        for dep in self.dependencies.values():
            self.output.info(f'SDSINGH: {str(dep), dep.package_folder}')
            if str(dep).startswith('videobase'):
                self.output.info(f'Copying QML files from {dep.package_folder} to {qml_folder}')
                copy(self, pattern='*', src=os.path.join(dep.package_folder, 'qml'), dst=qml_folder)
                copy(
                    self, pattern='*', src=os.path.join(dep.package_folder, 'lib'), dst=lib_folder
                )
                copy(
                    self,
                    pattern='*',
                    src=os.path.join(dep.package_folder, 'plugins'),
                    dst=plugins_folder,
                )



    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def test(self):
        if self.options.with_tests:
            self.output.info(f'SDSINGH2: Testing Gtest')
            cmake = CMake(self)
            cmake.test()

    def package(self):
        cmake = CMake(self)
        cmake.install()
        copy(self, pattern='*.h', dst=os.path.join(self.package_folder, 'include'), src=os.path.join(self.source_folder, 'src'))
        copy(self, pattern='*.qml', dst=os.path.join(self.package_folder, 'qml'), src=os.path.join(self.source_folder, 'qml'))
        copy(self, pattern='*.png', dst=os.path.join(self.package_folder, 'qml/resources/icons'), src=os.path.join(self.source_folder, 'qml/resources/icons'))
        copy(self, pattern='luzern-videobox', dst=os.path.join(self.package_folder, 'bin'), src=os.path.join(self.build_folder, ''), keep_path=False)


    def package_info(self):
        self.cpp_info.libs = ['luzern-videobox']
        self.cpp_info.bindirs = ['bin']
        self.cpp_info.includedirs = ['include']
        self.cpp_info.qmldirs = ['qml']
