cmake_minimum_required(VERSION 3.2)

catapult_test_executable_target(tests.catapult.plugins core)
target_link_libraries(tests.catapult.plugins)
target_link_libraries(tests.catapult.plugins catapult.plugins.transfer) # allow transfer to be loaded implicitly
