# rkros_asr

本目录包含两个ASR程序：`rk3588_asr` 和 `rk182x_asr`，这两个程序不能同时编译使用，只能选择其中一个进行编译。

## 编译说明

默认情况下，编译的是 `rk3588_asr` 程序。编译开关通过 `COLCON_IGNORE` 文件进行控制。

### 编译 rk182x_asr

```bash
rm rk182x_asr/COLCON_IGNORE
touch rk3588_asr/COLCON_IGNORE
colcon build
```

### 编译 rk3588_asr

```bash
rm rk3588_asr/COLCON_IGNORE
touch rk182x_asr/COLCON_IGNORE
colcon build
```

## 使用说明

关于 `rk3588_asr` 和 `rk182x_asr` 的详细使用说明，请查看各个目录下的 `doc/README.md` 文件。