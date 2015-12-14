from distutils.core import setup, Extension


module = Extension('zippie',
                   language='c++',
                   include_dirs=['src'],
                   sources=['src/zippiemodule.cpp',
                            'src/zippie/zippie.cpp',
                            'src/zippie/zmi.cpp',
                            'src/zippie/cdfh.cpp',
                            'src/zippie/lfh.cpp',
                            'src/zippie/extrafield.cpp',
                            'src/zippie/datadescriptor.cpp',
                            'src/zippie/scopedstream.cpp',
                            'src/zippie/utils.cpp',
                            'src/zippie/os.cpp',
                            'src/zippie/logging.cpp'])

setup(name='zippie',
      version='0.0.1',
      description='Zip file handling library',
      author='Andrean Franc',
      author_email='andrean.franc@gmail.com',
      url='https://github.com/integricho/zippie',
      ext_modules=[module])
