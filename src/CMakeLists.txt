cmake_minimum_required( VERSION 3.10 )
project( matrix VERSION 0.1.1 )
add_executable( matrix main.cpp )
add_executable( matrix_test test_matrix.cpp )

# package part which enales
# run: cmake --build . --target package
install(TARGETS matrix RUNTIME DESTINATION bin)
set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PATCH_VERSION}")
set(CPACK_PACKAGE_CONTACT berrendej@gmail.com)
include(CPack)

#testing part which enables
# cmake --build . --target test
enable_testing()
add_test(matrix_test matrix_test) 