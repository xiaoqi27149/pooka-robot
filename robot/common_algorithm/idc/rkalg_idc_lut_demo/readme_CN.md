<h1 align = "center">
    RK IDC Lut Demo
</h1>



# 1 Overview

```
/
├── demo/
│   ├── data/
│   └── src/
│       └── rkalg_idc_lut_demo.cpp
├── include/
│   ├── drm/
│   ├── rkalg_idc_comm.h
│   ├── rkalg_idc_dma_buffer.h
│   └── rkalg_idc_lut_api.h
│   ├── rkalg_idc_lut_def.h
├── lib/
│   └── gcc-arm-10.3-2021.07-aarch64/
│       └── librkalg_idc.so
├── doc/
└── readme.md
```

IDC 算法库依赖 OpenCL 库，初始化时将从以下路径依次查找和链接 OpenCL 库，需确认以下路径之一存在且有效。

```
/usr/lib/libOpenCL.so
/oem/usr/lib/libmali.so
```

/demo/data/ 目录下包含畸变校正测试用例，供参考效果.
