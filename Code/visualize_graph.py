import pydot

# Read the DOT file
dot_file_path = "Graph1_dot.dot"
graph = pydot.graph_from_dot_file(dot_file_path)

# Get the first graph (assuming there's only one graph in the file)
dot_graph = graph[0]

# Visualize the graph
png_file_path = "Graph.png"
dot_graph.write_png(png_file_path)

# # You can also display the graph directly in a Jupyter Notebook
# from IPython.display import Image
# Image(png_file_path)