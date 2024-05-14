import yaml
import argparse
from matplotlib import pyplot as plt
import matplotlib.image as mpimg


def vizualize_tile_set(path: str):
    with open(path) as stream:
        tile_set_description = yaml.safe_load(stream)
    for file in tile_set_description:
        img = mpimg.imread(file["file_name"])
        plt.imshow(img)
        plt.show()


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Process some integers.')
    parser.add_argument('texture_set_path', type=str,
                        help='path to a yaml file describing a texture set')
    args = parser.parse_args()
    vizualize_tile_set(args.texture_set_path)
