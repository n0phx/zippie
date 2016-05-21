from distutils.core import setup, Extension


module = Extension('zippie',
                   language='c++',
                   include_dirs=['src'],
                   extra_compile_args=['-std=c++11'],
                   sources=['src/zippiemodule.cpp',
                            'src/zippie/zippie.cpp',
                            'src/zippie/zmi.cpp',
                            'src/zippie/cdfh.cpp',
                            'src/zippie/lfh.cpp',
                            'src/zippie/extrafield.cpp',
                            'src/zippie/datadescriptor.cpp',
                            'src/zippie/streams/crc32stream.cpp',
                            'src/zippie/streams/crc32.cpp',
                            'src/zippie/streams/scopedstream.cpp',
                            'src/zippie/utils.cpp',
                            'src/zippie/os.cpp',
                            'src/zippie/logging.cpp'])

setup(name='zippie',
      version='0.1.dev1',
      description='Zip file handling library',
      author='Andrean Franc',
      author_email='andrean.franc@gmail.com',
      license="BSD",
      url='https://github.com/integricho/zippie',
      ext_modules=[module],
      classifiers=[
          'Development Status :: 2 - Pre-Alpha',
          'License :: OSI Approved :: BSD License',
          'Programming Language :: Python :: 2',
          'Topic :: Utilities',
      ])
