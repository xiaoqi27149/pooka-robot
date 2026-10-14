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

IDC depends on OpenCL library. During initialization, it will sequentially search for and link against the OpenCL library from the following paths. Please verify that at least one of these paths exists and is valid.

```
/usr/lib/libOpenCL.so
/oem/usr/lib/libmali.so
```

The `/demo/data/` directory contains distortion correction test cases for reference purposes.

