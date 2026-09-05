prepare:
	cmake -E remove_directory build
	cmake -E make_directory build

configure:
	cmake -S . -B build

build:
	cmake --build build --config Release
