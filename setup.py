#
# Copyright (c) 2016. Jean-Baptiste Galet. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
from codecs import open

from setuptools import setup, Extension

with open('README.rst', 'r', 'utf-8') as f:
    readme = f.read()

setup(
    name='etwtracer',
    version='0.0.1',
    description='Python interface for ETW',
    long_description=readme,
    license='Apache 2.0',
    author='Jean-Baptiste Galet',
    author_email='jb@jbgalet.fr',
    url='https://github.com/jbgalet/etwtracer',
    zip_safe=False,
    ext_modules=[Extension(
        name='etw',
        include_dirs=['krabsetw.1.0.0/lib/native/include', 'boost.1.61.0.0/lib/native/include'],
        library_dirs=['boost/'],
        extra_compile_args=['-D_UNICODE'],
        sources=['krabsetw.1.0.0/lib/native/src/krabs.cpp',
                 'etw-python/etw-python.cpp',
                 'etw-python/trace.cpp',
                 'etw-python/provider.cpp',
                 'etw-python/kernel_trace.cpp',
                 'etw-python/kernel_provider.cpp',
                 'etw-python/event.cpp',
                 ]
    )]
)
