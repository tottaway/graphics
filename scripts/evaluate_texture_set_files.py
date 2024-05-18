from typing import Optional
import yaml
import argparse
from matplotlib import pyplot as plt
from PIL import Image
import numpy as np

def visualize_subsection(subsection_node, full_image):
    start = np.array(subsection_node["start"])
    tile_size = np.array(subsection_node["tile_size"])
    padding = np.array(subsection_node["padding"])
    for i in range(0, subsection_node["horizontal_tile_count"]):
        for j in range(0, subsection_node["vertical_tile_count"]):
            # __import__('pdb').set_trace()
            tile_start = start + tile_size * np.array([i, j])
            tile_end = tile_start + tile_size
            img_view = full_image[tile_start[1] + padding[1]:tile_end[1] - padding[1],
                                  tile_start[0] + padding[0]:tile_end[0] - padding[0]] 
            plt.imshow(img_view)
            plt.show()

def visualize_file(file_node, subsection_name: str):
    img = np.asarray(Image.open(file_node["file_name"]))
    for curr_subsection_name, subsection in file_node["subsections"].items():
        if not subsection_name or subsection_name == curr_subsection_name:
            visualize_subsection(subsection, img)

def visualize_tile_set(path: str, subsection_name: str):
    with open(path) as stream:
        tile_set_description = yaml.safe_load(stream)
    for file in tile_set_description:
        visualize_file(file, subsection_name)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Process some integers.')
    parser.add_argument('texture_set_path', type=str,
                        help='path to a yaml file describing a texture set')

    parser.add_argument('-s', '--subsection-name', type=str,
                        help='name of the subsection to display', default="", required=False)

    args = parser.parse_args()
    visualize_tile_set(args.texture_set_path, args.subsection_name)
