{
  'targets': [
    {
      'target_name': 'cpp_server',
      'type': 'executable',
      'libraries': [
        '<!@(echo "$(pwd)/deps/libs/microhttpd/lib/libmicrohttpd.a")',
      ],
      'sources': [
        '<!@(find src -name "*.cpp" -o -name "*.hpp" -o -name "*.h")',
      ],
      'include_dirs': [
        '<!@(find src -type d)',
        'deps/libs/microhttpd/include',
      ],
      'all_dependent_settings': {
        'include_dirs': [
          '<!@(find src -type d)',
          'deps/libs/microhttpd/include',
        ],
      },
    },
    {
      'target_name': 'test',
      'type': 'executable',
      'defines+': [ 'TARGET_TEST=1' ],
      'dependencies': [
        'deps/gtest.gyp:gtest',
      ],
      'libraries': [
        '<!@(echo "$(pwd)/deps/libs/microhttpd/lib/libmicrohttpd.a")',
      ],
      'sources': [
        '<!@(find src -name "*.cpp" -o -name "*.hpp" -o -name "*.h")',
      ],
      'sources!': ['src/main.cpp'],
      'include_dirs': [
        '<!@(find test -type d)',
        '<!@(find src -type d)',
        'deps/libs/microhttpd/include',
      ],
      'all_dependent_settings': {
        'include_dirs': [
          '<!@(find test -type d)',
          '<!@(find src -type d)',
          'deps/libs/microhttpd/include',
        ],
      },
    },
  ],
}
