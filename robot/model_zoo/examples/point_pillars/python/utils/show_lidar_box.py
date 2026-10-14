import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from mpl_toolkits.mplot3d.art3d import Poly3DCollection


def show_box(points, boxes, save_path=None):
    points = np.asarray(points)

    fig = plt.figure(figsize=(10, 10))
    ax = fig.add_subplot(111, projection='3d')

    ax.scatter(points[:, 0], points[:, 1], points[:, 2], s=0.001, c='gray', alpha=0.5)

    # 绘制每个边界框
    for data in boxes:
        extent = data[3:6]
        center = np.array(data[:3])
        center[2] = center[2] + extent[2] / 2  
        rotation = data[6]  

        half_extents = extent / 2
        vertices = np.array([
            [half_extents[0], half_extents[1], half_extents[2]],
            [half_extents[0], half_extents[1], -half_extents[2]],
            [half_extents[0], -half_extents[1], half_extents[2]],
            [half_extents[0], -half_extents[1], -half_extents[2]],
            [-half_extents[0], half_extents[1], half_extents[2]],
            [-half_extents[0], half_extents[1], -half_extents[2]],
            [-half_extents[0], -half_extents[1], half_extents[2]],
            [-half_extents[0], -half_extents[1], -half_extents[2]]
        ])

        if rotation != 0:
            cos_rot = np.cos(rotation)
            sin_rot = np.sin(rotation)
            rotation_matrix = np.array([
                [cos_rot, -sin_rot, 0],
                [sin_rot, cos_rot, 0],
                [0, 0, 1]
            ])
            vertices = np.dot(vertices, rotation_matrix.T)

        vertices += center

        faces = [
            [vertices[0], vertices[1], vertices[3], vertices[2]],
            [vertices[4], vertices[5], vertices[7], vertices[6]],
            [vertices[0], vertices[1], vertices[5], vertices[4]],
            [vertices[2], vertices[3], vertices[7], vertices[6]],
            [vertices[0], vertices[2], vertices[6], vertices[4]],
            [vertices[1], vertices[3], vertices[7], vertices[5]]
        ]

        box_collection = Poly3DCollection(faces, linewidths=1, edgecolors='green', alpha=1)
        box_collection.set_facecolor('green')
        ax.add_collection3d(box_collection)

        in_box = np.all(np.abs(points[:, :3] - center) <= half_extents, axis=1)
        ax.scatter(points[in_box, 0], points[in_box, 1], points[in_box, 2], s=2, c='red', alpha=0.8)

    axis_length = 2
    ax.quiver(0, 0, 0, axis_length, 0, 0, color='r', lw=2, label='X')
    ax.quiver(0, 0, 0, 0, axis_length, 0, color='g', lw=2, label='Y')
    ax.quiver(0, 0, 0, 0, 0, axis_length, color='b', lw=2, label='Z')

    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')
    ax.set_title('Point Cloud with Bounding Boxes')
    ax.legend()
    ax.set_axis_off()


    max_range = np.max([
        np.max(points[:, 0]) - np.min(points[:, 0]),
        np.max(points[:, 1]) - np.min(points[:, 1]),
        np.max(points[:, 2]) - np.min(points[:, 2])
    ])
    mid_x = (np.max(points[:, 0]) + np.min(points[:, 0])) / 2
    mid_y = (np.max(points[:, 1]) + np.min(points[:, 1])) / 2
    mid_z = (np.max(points[:, 2]) + np.min(points[:, 2])) / 2

    ax.set_xlim(mid_x - max_range / 2, mid_x + max_range / 2)
    ax.set_ylim(mid_y - max_range / 2, mid_y + max_range / 2)
    ax.set_zlim(mid_z - max_range / 2, mid_z + max_range / 2)

    if save_path is None:
        save_path = "out.png"
    plt.savefig(save_path, dpi=300, bbox_inches='tight')
    print(f"write result: {save_path}")
