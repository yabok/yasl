sphinx = find_program('sphinx-build')
if sphinx.found()
    docs_sources = [
        'yasl.rst',
        'yaslfree.rst',
        'yaslnew.rst',
    ]
    custom_target('manpages',
                  command: [sphinx, '-W', '-b', 'man', meson.current_source_dir(), '@OUTPUT@'],
                  input: docs_sources,
                  output: 'man3',
                  build_by_default: true,
                  install: true,
                  install_dir: get_option('mandir'))
endif
