prepare:
	del build
	mkdir build

dependency:
	cd build && cmake .. --graphviz=graph.dot && dot -Tpng graph.dot -o graph_image.png
