set(
    EXEC_SOURCES
    "main.cpp"
    "Mesh.cpp"
)

# Adding the main file as an executable to our project
add_executable(
    ${EXECUTABLE_NAME}
    ${EXEC_SOURCES}
)

# We have inheritance modes - PUBLIC, PRIVATE, INTERFACE
target_link_libraries(
    ${EXECUTABLE_NAME} PUBLIC
    ${LIBRARY_NAME}
    ${EXTERN_LIBRARY_NAME}
)
