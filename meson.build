project('yasl', 'c',
        version : '0.0.1',
        license : 'ISC',
        default_options : ['c_std=c99'])

inc = include_directories('include')

subdir('include')
subdir('src')
subdir('man')
subdir('test')

pkg = import('pkgconfig')
pkg.generate(libraries : yasllib,
             name : 'libyasl',
             version : meson.project_version(),
             filebase : 'yasl',
             description : 'Yet Another String Library for C',
             url : 'https://github.com/yabok/yasl')
