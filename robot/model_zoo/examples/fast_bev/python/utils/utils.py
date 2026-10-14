import os
import cv2
import numpy as np
import shutil
from utils.lidar_box3d import LiDARInstance3DBoxes as LB

def save_ten(tensor, file):
    
    if not isinstance(tensor, np.ndarray):
        import torch
        if isinstance(tensor, torch.Tensor):
            tensor = tensor.detach().cpu().data.numpy()
        else:
            tensor = np.array(tensor)

    dtype_map = {"float32" : 3, "float16" : 2, "int32" : 1, "int64" : 4, "uint64": 5, "uint32": 6, "int8": 7, "uint8": 8}
    if str(tensor.dtype) not in dtype_map:
        raise RuntimeError(f"Unsupport dtype {tensor.dtype}")

    magic_number = 0x33ff1101
    with open(file, "wb") as f:
        head = np.array([magic_number, tensor.ndim, dtype_map[str(tensor.dtype)]], dtype=np.int32).tobytes()
        f.write(head)

        dims = np.array(tensor.shape, dtype=np.int32).tobytes()
        f.write(dims)
        
        data = tensor.tobytes()
        f.write(data)


def load_ten(file, return_torch=False):

    dtype_for_integer_mapping = {3: np.float32, 2: np.float16, 1: np.int32, 4: np.int64, 5: np.uint64, 6: np.uint32, 7: np.int8, 8: np.uint8}
    dtype_size_mapping        = {np.float32 : 4, np.float16 : 2, np.int32 : 4, np.int64 : 8, np.uint64 : 8, np.uint32 : 4, np.int8 : 1, np.uint8 : 1}

    with open(file, "rb") as f:
        magic_number, ndim, dtype_integer = np.frombuffer(f.read(12), dtype=np.int32)
        if dtype_integer not in dtype_for_integer_mapping:
            raise RuntimeError(f"Can not find match dtype for index {dtype_integer}")

        dtype            = dtype_for_integer_mapping[dtype_integer]
        magic_number_std = 0x33ff1101
        assert magic_number == magic_number_std, f"this file is not tensor file"
        dims   = np.frombuffer(f.read(ndim * 4), dtype=np.int32)
        volumn = np.cumprod(dims)[-1]
        data   = np.frombuffer(f.read(volumn * dtype_size_mapping[dtype]), dtype=dtype).reshape(*dims)

        if return_torch:
            import torch
            return torch.from_numpy(data)
        return data

def copy_file(src_file, dst_file):
    dst_dir, file = os.path.split(dst_file)
    os.makedirs(dst_dir, exist_ok=True)
    
    shutil.copy2(src_file, dst_file)
    print(f"copy file: {src_file} -> {dst_file}")

def read_img_metas(img_metas):
    info_dict = {}
    info_dict["cams"] = {}
    j = 0
    img_info = img_metas[0]['img_info']
    for file in img_info[:6]:
        file = file['filename']
        file_arr = file.split('/')[-2]
        info_dict["cams"][file_arr] = {}
        info_dict["cams"][file_arr]['img_path'] = file
        
        lidar2img_aug = img_metas[0]['lidar2img']["lidar2img_aug"][j]
        info_dict["cams"][file_arr]['lidar2img'] = {}
        info_dict["cams"][file_arr]['lidar2img']['extrinsic'] = img_metas[0]['lidar2img']['extrinsic_org'][j]
        info_dict["cams"][file_arr]['lidar2img']['intrinsic'] = img_metas[0]['lidar2img']['intrinsic'][:3,:3]
        info_dict["cams"][file_arr]['post_tran'] = lidar2img_aug['post_tran']
        info_dict["cams"][file_arr]['post_rot'] = lidar2img_aug['post_rot']
        
        j = j + 1
            
    return info_dict


def check_point_in_img(points, height, width):
    valid = np.logical_and(points[:, 0] >= 0, points[:, 1] >= 0)
    valid = np.logical_and(valid, np.logical_and(points[:, 0] < width, points[:, 1] < height))
    return valid


def depth2color(depth):
    gray = max(0, min((depth + 2.5) / 3.0, 1.0))
    max_lumi = 200
    colors = np.array([[max_lumi, 0, max_lumi], 
                       [max_lumi, 0, 0],
                       [max_lumi, max_lumi, 0],
                       [0, max_lumi, 0], 
                       [0, max_lumi, max_lumi], 
                       [0, 0, max_lumi]],
        dtype=np.float32)
    if gray == 1:
        return tuple(colors[-1].tolist())
    num_rank = len(colors) - 1
    rank     = np.floor(gray * num_rank).astype(np.int)
    diff     = (gray - rank / num_rank) * num_rank
    return tuple((colors[rank] + (colors[rank + 1] - colors[rank]) * diff).tolist())


def lidar2img(points_lidar, camrera_info):
    points_lidar_homogeneous  = np.concatenate([points_lidar, np.ones((points_lidar.shape[0], 1), dtype=points_lidar.dtype)], axis=1)
    
    lidar2camera              = camrera_info['lidar2img']['extrinsic']
    points_camera_homogeneous = points_lidar_homogeneous @ lidar2camera.T
    points_camera             = points_camera_homogeneous[:, :3]
    valid                     = np.ones((points_camera.shape[0]), dtype=bool)
    valid                     = np.logical_and(points_camera[:, -1] > 0.5, valid)
    points_camera             = points_camera / points_camera[:, 2:3]
    camera2img                = camrera_info['lidar2img']['intrinsic']
    points_img                = points_camera @ camera2img.T
    post_aug                  = np.eye(3, dtype=np.float32)
    post_aug[:2, :2]          = camrera_info['post_rot'][:2,:2]
    post_aug[:2, 2]           = camrera_info['post_tran'][:2]
    points_img                = np.linalg.inv(post_aug) @ points_img.transpose(1,0)
    points_img                = points_img.transpose(1,0)[:, :2]
    return points_img, valid



def visual(img_metas, bboxes, scores, vis_path):
    info_dict               = read_img_metas(img_metas)
    cam_info_dict           = info_dict['cams']

    # Define indices for drawing bounding boxes in BEV view
    draw_boxes_indexes_bev      = [(0, 1), (1, 2), (2, 3), (3, 0)]
    draw_boxes_indexes_img_view = [(0, 1), (1, 2), (2, 3), (3, 0), (4, 5), (5, 6), 
                                   (6, 7), (7, 4), (0, 4), (1, 5), (2, 6), (3, 7)]
    canva_size   = 1000
    show_range   = 50
    scale_factor = 4
    color_map    = {0: (255, 255, 0), 1: (0, 255, 255)}

    # Visualization parameters
    print('start visualizing results')
    pred_boxes        = np.array(bboxes, dtype=np.float32)
    corners_lidar     = LB(pred_boxes, origin=(0.5, 0.5, 0), box_dim=9).corners.numpy().reshape(-1, 3)

    pred_flag = np.ones((corners_lidar.shape[0] // 8, ), dtype=np.bool_)
    scores    = np.array(scores, dtype=np.float32)

    # Create flag array for predicted boxes
    sort_ids  = np.argsort(scores)

    # Visualize results from different camera views
    imgs = []
    views = ['CAM_FRONT_LEFT', 'CAM_FRONT', 'CAM_FRONT_RIGHT', 'CAM_BACK_LEFT', 'CAM_BACK', 'CAM_BACK_RIGHT']
    for view in views:
            
        img = cv2.imread(cam_info_dict[view]['img_path'])
        # Convert LiDAR coordinates to image pixel coordinates and draw bounding boxes
        corners_img, valid = lidar2img(corners_lidar, cam_info_dict[view])
        valid = valid.reshape(-1, 8)
        corners_img = corners_img.reshape(-1, 8, 2).astype(np.int32)
        for aid in range(valid.shape[0]):
            for index in draw_boxes_indexes_img_view:
                if valid[aid, index[0]] and valid[aid, index[1]]:
                    cv2.line(img, corners_img[aid, index[0]], corners_img[aid, index[1]],
                            color=color_map[int(pred_flag[aid])], thickness=scale_factor)
        imgs.append(img)

    # Create canvas for BEV (Bird's Eye View) visualization
    canvas = np.zeros((int(canva_size), int(canva_size), 3), dtype=np.uint8)


    # Draw distance rings
    center_ego = (0, 0)
    center_canvas = int((center_ego[0] + show_range) / show_range / 2.0 * canva_size)
    cv2.circle(canvas, center=(center_canvas, center_canvas), radius=1, color=(255, 255, 255), thickness=0)
    dis = 10
    for r in range(dis, 100, dis):
        r_canvas = int(r / show_range / 2.0 * canva_size)
        cv2.circle(canvas, center=(center_canvas, center_canvas), radius=r_canvas, color=depth2color(r), thickness=0)
        
    
    # Prepare data for BEV visualization
    corners_lidar          = corners_lidar.reshape(-1, 8, 3)
    corners_lidar[:, :, 1] = -corners_lidar[:, :, 1]
    bottom_corners_bev     = corners_lidar[:, [0, 3, 7, 4], :2]
    bottom_corners_bev     = (bottom_corners_bev + show_range) / show_range / 2.0 * canva_size
    bottom_corners_bev     = np.round(bottom_corners_bev).astype(np.int32)
    center_bev             = corners_lidar[:, [0, 3, 7, 4], :2].mean(axis=1)
    head_bev               = corners_lidar[:, [0, 4], :2].mean(axis=1)
    canter_canvas          = (center_bev + show_range) / show_range / 2.0 * canva_size
    center_canvas          = canter_canvas.astype(np.int32)
    head_canvas            = (head_bev + show_range) / show_range / 2.0 * canva_size
    head_canvas            = head_canvas.astype(np.int32)

    # Draw predicted boxes in BEV view
    for rid in sort_ids:
        score = scores[rid]
        if score < 0.2 and pred_flag[rid]:
            continue
        score = min(score * 2.0, 1.0) if pred_flag[rid] else 1.0
        color = color_map[int(pred_flag[rid])]
        for index in draw_boxes_indexes_bev:
            cv2.line(canvas, bottom_corners_bev[rid, index[0]], bottom_corners_bev[rid, index[1]],
                    [color[0] * score, color[1] * score, color[2] * score], thickness=1)
        cv2.line(canvas, center_canvas[rid], head_canvas[rid], [color[0] * score, color[1] * score, color[2] * score], 1, lineType=8)

    # Combine all visualizations into a single image
    img = np.zeros((900 * 2 + canva_size * scale_factor, 1600 * 3, 3), dtype=np.uint8)
    img[:900, :, :] = np.concatenate(imgs[:3], axis=1)
    img_back = np.concatenate([imgs[3][:, ::-1, :], imgs[4][:, ::-1, :], imgs[5][:, ::-1, :]], axis=1)
    img[900 + canva_size * scale_factor:, :, :] = img_back
    img = cv2.resize(img, (int(1600 / scale_factor * 3), int(900 / scale_factor * 2 + canva_size)))
    w_begin = int((1600 * 3 / scale_factor - canva_size) // 2)
    img[int(900 / scale_factor):int(900 / scale_factor) + canva_size, w_begin:w_begin + canva_size, :] = canvas

    # Save visual result
    cv2.imwrite(vis_path, img)
    print(f'Saved visual result to {vis_path}')