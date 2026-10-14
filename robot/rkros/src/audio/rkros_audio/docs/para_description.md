
### rk audio hci json参数详细说明
```
{
    "hci_configs": {

      "license_path": "/data/key_audio3a.lic",          # license路径

      "para_path": "./para",                           # json, model,cfg等文件夹所在路径

      "case": {
        "mic_num": 6,                                   # 麦克风采集通道数
        "far_num": 2,                                   # 远端参考信号通道数
        "sample_rate": 16000,                           # 采样率
        "frame_len_ms": 20,                             # 算法处理帧长，单位ms，目前固定为20ms
        "mic_gain_dB": 0,                               # 麦克风增益，单位dB
        "far_gain_dB": 0,                               # 参考信号增益，单位dB
        "aec_gain_dB": 7,                               # 线性回声消除后麦克增益，单位dB
        "aec_level" : 0,                                # 回声消除等级，范围[0, 5]
        "aiec_level" : 0,                               # AI回声消除开关，0关闭，1开启
        "ainr_level" : 1,                               # AI降噪开关，0关闭，1开启
        "post_level" : 1,                               # 后处理开关，0关闭，1开启
        "eq_level" : 0,                                 # 频域均衡开关，0关闭，1开启
        "agc_level" : 1,                                # 自动增益控制开关，0关闭，1开启
        "doa_level" : 1,                                # 声源定位开关，0关闭，1开启
        "bf_level" : 1,                                 # 波束开关，0关闭，1开启
        "delay_level": 0                                # 回声与远端参考信号延时估计开关，0关闭，1开启
        "kws_level": 1                                  # 唤醒开关，0关闭，1开启
        “sed_level: 1                                   # 场景检测开关，0关闭，1开启
      },

      "aiec": {
        "post_level": 0,                                # AI回声消除后处理开关，0关闭，1开启
        "min_gain_d100": 0,                             # 回声消除最小增益，单位0.01dB
        "post_ser_echo_d100": 80,                       # 信回线性比低于此值，为降噪最小增益，单位0.01
        "post_ser_near_d100": 200                       # 信回线性比高于此值，为降噪最大增益，单位0.01
      },

      "ainr": {
        "post_level" : 0,                               # AI降噪后处理开关，0关闭，1开启
        "min_gain_d100": 0,                             # 降噪最小增益，单位0.01dB
        "post_snr_noise_d100": 80,                      # 信噪线性比低于此值，为降噪最小增益，单位0.01
        "post_snr_speech_d100": 200                     # 信噪线性比高于此值，为降噪最大增益，单位0.01
      },

      "post": {
        "post_mode" : 2,                                # 后处理模式，1通话模式，用于抑制残留回声，2识别模式，用于降低3A抑制程度，保护语音
        "add_gain_d100":10,                             # 识别模式时，回复原始信号比例，单位0.01
        "thres": 0                                      # 识别模式时，vad概率阈值，大于此值则触发回复原始信号，单位0.01
      },

      "agc": {
        "attack_time_ms" : 200,                         # attack时间，单位ms
        "release_time_ms" : 200,                        # release时间，单位ms
        "max_gain_dB": 5,                               # 最大增益，单位dB；注意：expand_end_dB+max_gain_dB应小于peak_value_dB
        "peak_value_dB": -3,                            # 处理后最大幅度，单位dBFS；注意：expand_end_dB+max_gain_dB应小于peak_value_dB
        "noise_thd_dB": -70,                            # 噪声阈值，低于此值不放大，单位dBFS
        "compress_start_dB": -60,                       # 压缩开始点，单位dBFS
        "expand_end_dB": -20,                           # 扩张结束点，单位dBFS；注意：expand_end_dB+max_gain_dB应小于peak_value_dB
        "out_vad_flag": 1                               # 是否采用外部VAD，0否，1是
      },

      "eq": {
        "section_num" : 2,                              # EQ段数，范围[2,5]
        "section_1_center_freq" : 200,                  # 第一段EQ的中心频率
        "section_1_gain_dB": -3,                        # 第一段EQ的增益，单位dB
        "section_1_q_factor_d100": 70,                  # 第一段EQ的Q值，单位0.01
        "section_2_center_freq" : 1600,                 # 第二段EQ的中心频率
        "section_2_gain_dB": -24,                       # 第二段EQ的增益，单位dB
        "section_2_q_factor_d100": 25,                  # 第二段EQ的Q值，单位0.01
        "section_3_center_freq" : 0,
        "section_3_gain_dB": 0,
        "section_3_q_factor_d100": 0,
        "section_4_center_freq" : 0,
        "section_4_gain_dB": 0,
        "section_4_q_factor_d100": 0,
        "section_5_center_freq" : 0,
        "section_5_gain_dB": 0,
        "section_5_q_factor_d100": 0
      },

      "doa": {
        "array_type": 0,                                 # 阵型，0为环阵，1为线阵
        "array_distance_mm": 35,                         # 环阵的半径，或者线阵的间距，单位mm
        "skip_frms": 1,                                  # 跳帧数，增大此值，平均算力降低，doa输出延时增大，但稳定性增加
        "degree_resolution": 2,                          # 角度精度，范围[2,5]，单位度
        "start_freq": 300,                               # 搜索起始频率，单位Hz
        "end_freq": 3500,                                # 搜索终止频率，单位Hz
        "delay_frms": 25,                                # 估计延时数，越大越稳定，反应速度越慢
        "speech_energy_thd": 100                         # DOA生效的能量阈值，数值增大，抗干扰变强，但反应灵敏度下降
      },

      "sed": {
        "skip_frm1": 5,                                  # 小孩哭声检测跳帧数，建议范围[1， 30], 数值越大，精度越低，复杂度越低。
        "skip_frm2": 5,                                  # 狗叫声检测跳帧数, 建议范围[1， 10]。
        "prob": 75                                       # 检测阈值，范围[0, 100], 数值越大，漏检率越高，数值越小，误警率越高。
      }
    }
  }

  "skv_configs" : 这里目前不需要调整，保持默认即可。
  ```
