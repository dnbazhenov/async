# SPDX-License-Identifier: LGPL-2.1-or-later

project(
    'async',
    'cpp',
    default_options: [
        'werror=false',
        'warning_level=3',
        'cpp_std=c++23',
        'buildtype=minsize',
        #'buildtype=debug',
        #'buildtype=debugoptimized',
        #        'buildtype=release',
        #        'buildtype=plain',
        'b_ndebug=if-release',
        'b_lto=true',
    ],
)

boost_compile_args = [
    '-DBOOST_ALL_NO_LIB',
    '-DBOOST_ASIO_DISABLE_THREADS',
    '-DBOOST_ASIO_NO_DEPRECATED',
    '-DBOOST_ASIO_HAS_BOOST_CONTEXT_FIBER',
    '-DBOOST_ASIO_DISABLE_BOOST_COROUTINE',
]

boost_dep = declare_dependency(
    dependencies: dependency(
        'boost',
        modules: ['context'],
        include_type: 'system',
        required: true,
    ),
    compile_args: boost_compile_args,
)

incdir = include_directories('include')

asynclib = static_library(
    'async_lib',
    'src/coro_impl.cpp',
    'src/coro_context.cpp',
    'src/scheduler.cpp',
    'src/wait_queue.cpp',
    include_directories: incdir,
    dependencies: [boost_dep],
)

asynclib_dep = declare_dependency(compile_args: boost_compile_args, link_with: asynclib)

subdir('examples')
