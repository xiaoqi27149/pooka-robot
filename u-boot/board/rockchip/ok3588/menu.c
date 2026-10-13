#include <common.h>
#include <boot_rkimg.h>
#include <dm.h>
#include <malloc.h>
#include <linux/libfdt.h>
#include <fdt_support.h>
#include <fdtdec.h>
#include <of_live.h>
#include <dm/root.h>
#include <dm/of_access.h>
#include <dm/lists.h>
#include <asm/arch/hotkey.h>
#include <environment.h>
#include <stdlib.h>

DECLARE_GLOBAL_DATA_PTR;

typedef enum {
    vp0_t = 0,
    vp1_t,
    vp2_t,
    vp3_t,
    vp_off_t,
    vp_max_t
} _vp_t;

typedef enum {
    hdmi0_t = 0,
    hdmi1_t,
    edp0_t,
    edp1_t,
    dp0_t,
    dp1_t,
    mipi0_t,
    mipi1_t,
    rgb_t,
    dsp_max_t
} _dsp_t;

static char * vps_name[] = {"VP0", "VP1", "VP2", "VP3", ""};
static char * inf_name[] = {"HDMI0", "HDMI1", "EDP0", "EDP1", "DP0",
							"DP1", "MIPI0", "MIPI1", "RGB", "OFF"
};

static int vp_use[4]; //vp >= dsp_max_t off
static int display_port_cfg[dsp_max_t];
#define PRIMARY_RESOLUTIONS 4
static char * primary_resolution[PRIMARY_RESOLUTIONS] = {"3840x2160p60", "3840x2160p30",
							"1920x1080p60", "1920x1080p30" };


static void display_update_primary(void);
static void display_update_secondary(void);
static void board_pcie_enable(void);
int forlinx_board_fdt_fixup(const void *blob)
{
	char *vps[32];
	char vpname[32];
	int vp, dsp;
	char endpoint[128];
	u32 handle;
	bool get_dp0 = false;
	bool get_dp1 = false;

	for(vp = 0; vp < 4; vp++){
		sprintf(vpname, "video_vp%d", vp);
		vps[vp] = env_get(vpname);

		if(vps[vp] == NULL || !strncmp(vps[vp], "OFF", 3))
			continue;

		for(dsp = hdmi0_t; dsp < dsp_max_t; dsp++){
			if(strcmp(vps[vp], inf_name[dsp]) == 0)
				break;
		}
		switch(dsp){
			case hdmi0_t:
				sprintf(endpoint, "/hdmi@fde80000/ports/port@0/endpoint@%d", vp);
				do_fixup_by_path((void*)blob, endpoint, "status", "okay", 5, 1);
				do_fixup_by_path((void*)blob, "/display-subsystem/route/route-hdmi0", "status", "okay", 5, 1);
				do_fixup_by_path((void*)blob, "/hdmi@fde80000", "status", "okay", 5, 1);
				do_fixup_by_path((void*)blob, "/hdmiphy@fed60000", "status", "okay", 5, 1);
				sprintf(endpoint, "/vop@fdd90000/ports/port@%d/endpoint@2", vp);
				handle = fdt_getprop_u32_default((void*)blob, endpoint, "phandle", 0);
				do_fixup_by_path_u32((void*)blob, "/display-subsystem/route/route-hdmi0", "connect", handle, 1);
				break;
			case hdmi1_t:
				sprintf(endpoint, "/hdmi@fdea0000/ports/port@0/endpoint@%d", vp);
				do_fixup_by_path((void*)blob, endpoint, "status", "okay", 5, 1);
				do_fixup_by_path((void*)blob, "/display-subsystem/route/route-hdmi1", "status", "okay", 5, 1);
				do_fixup_by_path((void*)blob, "/hdmi@fdea0000", "status", "okay", 5, 1);
				do_fixup_by_path((void*)blob, "/hdmiphy@fed70000", "status", "okay", 5, 1);

				sprintf(endpoint, "/vop@fdd90000/ports/port@%d/endpoint@%d", vp, vp==2?7:5);
				handle = fdt_getprop_u32_default((void*)blob, endpoint, "phandle", 0);
				do_fixup_by_path_u32((void*)blob, "/display-subsystem/route/route-hdmi1", "connect", handle, 1);
				break;
			case edp0_t:
				sprintf(endpoint, "/edp@fdec0000/ports/port@0/endpoint@%d", vp);
				do_fixup_by_path((void*)blob, endpoint, "status", "okay", 5, 1);
				do_fixup_by_path((void*)blob, "/display-subsystem/route/route-edp0", "status", "okay", 5, 1);
				do_fixup_by_path((void*)blob, "/edp@fdec0000", "status", "okay", 5, 1);
				do_fixup_by_path((void*)blob, "/phy@fed60000", "status", "okay", 5, 1);

				sprintf(endpoint, "/vop@fdd90000/ports/port@%d/endpoint@1", vp);
				handle = fdt_getprop_u32_default((void*)blob, endpoint, "phandle", 0);
				do_fixup_by_path_u32((void*)blob, "/display-subsystem/route/route-edp0", "connect", handle, 1);
				break;
			case edp1_t:
				sprintf(endpoint, "/edp@fded0000/ports/port@0/endpoint@%d", vp);
				do_fixup_by_path((void*)blob, endpoint, "status", "okay", 5, 1);
				do_fixup_by_path((void*)blob, "/display-subsystem/route/route-edp1", "status", "okay", 5, 1);
				do_fixup_by_path((void*)blob, "/edp@fded0000", "status", "okay", 5, 1);
				do_fixup_by_path((void*)blob, "/phy@fed70000", "status", "okay", 5, 1);

				sprintf(endpoint, "/vop@fdd90000/ports/port@%d/endpoint@%d", vp, vp==2?6:4);
				handle = fdt_getprop_u32_default((void*)blob, endpoint, "phandle", 0);
				do_fixup_by_path_u32((void*)blob, "/display-subsystem/route/route-edp1", "connect", handle, 1);
				break;
			case dp0_t:
				sprintf(endpoint, "/dp@fde50000/ports/port@0/endpoint@%d", vp);
				do_fixup_by_path((void*)blob, endpoint, "status", "okay", 5, 1);
				do_fixup_by_path((void*)blob, "/display-subsystem/route/route-dp0", "status", "okay", 5, 1);
				do_fixup_by_path((void*)blob, "/dp@fde50000", "status", "okay", 5, 1);
				do_fixup_by_path((void*)blob, "/phy@fed80000", "status", "okay", 5, 1);

				sprintf(endpoint, "/vop@fdd90000/ports/port@%d/endpoint@0", vp);
				handle = fdt_getprop_u32_default((void*)blob, endpoint, "phandle", 0);
				do_fixup_by_path_u32((void*)blob, "/display-subsystem/route/route-dp0", "connect", handle, 1);
				get_dp0 = true;
				break;
			case dp1_t:
				sprintf(endpoint, "/dp@fde60000/ports/port@0/endpoint@%d", vp);
				do_fixup_by_path((void*)blob, endpoint, "status", "okay", 5, 1);
				do_fixup_by_path((void*)blob, "/display-subsystem/route/route-dp1", "status", "okay", 5, 1);
				do_fixup_by_path((void*)blob, "/dp@fde60000", "status", "okay", 5, 1);
				do_fixup_by_path((void*)blob, "/phy@fed90000", "status", "okay", 5, 1);

				sprintf(endpoint, "/vop@fdd90000/ports/port@%d/endpoint@%d", vp, vp==2?5:3);
				handle = fdt_getprop_u32_default((void*)blob, endpoint, "phandle", 0);
				do_fixup_by_path_u32((void*)blob, "/display-subsystem/route/route-dp1", "connect", handle, 1);
				get_dp1 = true;
				break;
			case mipi0_t:
				sprintf(endpoint, "/dsi@fde20000/ports/port@0/endpoint@%d", vp - 2);
				do_fixup_by_path((void*)blob, endpoint, "status", "okay", 5, 1);
				do_fixup_by_path((void*)blob, "/display-subsystem/route/route-dsi0", "status", "okay", 5, 1);
				do_fixup_by_path((void*)blob, "/dsi@fde20000", "status", "okay", 5, 1);
				do_fixup_by_path((void*)blob, "/phy@feda0000", "status", "okay", 5, 1);

				sprintf(endpoint, "/vop@fdd90000/ports/port@%d/endpoint@%d", vp, vp==2?3:0);
				handle = fdt_getprop_u32_default((void*)blob, endpoint, "phandle", 0);
				do_fixup_by_path_u32((void*)blob, "/display-subsystem/route/route-dsi0", "connect", handle, 1);
				break;
			case mipi1_t:
				sprintf(endpoint, "/dsi@fde30000/ports/port@0/endpoint@%d", vp - 2);
				do_fixup_by_path((void*)blob, endpoint, "status", "okay", 5, 1);
				do_fixup_by_path((void*)blob, "/display-subsystem/route/route-dsi1", "status", "okay", 5, 1);
				do_fixup_by_path((void*)blob, "/dsi@fde30000", "status", "okay", 5, 1);
				do_fixup_by_path((void*)blob, "/phy@fedb0000", "status", "okay", 5, 1);

				sprintf(endpoint, "/vop@fdd90000/ports/port@%d/endpoint@%d", vp, vp==2?4:1);
				handle = fdt_getprop_u32_default((void*)blob, endpoint, "phandle", 0);
				do_fixup_by_path_u32((void*)blob, "/display-subsystem/route/route-dsi1", "connect", handle, 1);
				break;
			case rgb_t:
				sprintf(endpoint, "/syscon@fd58c000/rgb/ports/port@0/endpoint@2");
				do_fixup_by_path((void*)blob, endpoint, "status", "okay", 5, 1);
				do_fixup_by_path((void*)blob, "/display-subsystem/route/route-rgb", "status", "okay", 5, 1);
				do_fixup_by_path((void*)blob, "/syscon@fd58c000/rgb", "status", "okay", 5, 1);
				break;
		}
	}

	if(get_dp0)
		do_fixup_by_path((void*)blob, "/i2c@feaa0000/fusb302@22/connector/altmodes", "status", "okay", 5, 1);
	else
		do_fixup_by_path((void*)blob, "/i2c@feaa0000/fusb302@22/connector/altmodes", "status", "disabled", 9, 1);
	if(get_dp1)
		do_fixup_by_path((void*)blob, "/i2c@fead0000/fusb302@22/connector/altmodes", "status", "okay", 5, 1);
	else
		do_fixup_by_path((void*)blob, "/i2c@fead0000/fusb302@22/connector/altmodes", "status", "disabled", 9, 1);

	if(!get_dp0){
		handle = fdt_path_offset((void*)blob, "/phy@fed80000");
		if(handle > 0){
			if(fdt_delprop((void*)blob, handle, "rockchip,dp-lane-mux") < 0)
				printf("dp0 dp-lane-mux can't be remove\n");
		}
	}
	
	if(!get_dp1){
		handle = fdt_path_offset((void*)blob, "/phy@fed90000");
		if(handle > 0){
			if(fdt_delprop((void*)blob, handle, "rockchip,dp-lane-mux") < 0)
				printf("dp1 dp-lane-mux can't be remove\n");
		}
	}

	return 0;
}

static int of_set_property_by_path(char *path, char *prop, void *value, int prop_len, int no)
{
	struct device_node *node;
	struct property *node_prop;
	int len = 0;
	node = of_find_node_opts_by_path(path, NULL);
	if(node){
		node_prop = of_find_property(node, prop, &len);
		if(node_prop) {
			node_prop->length = prop_len;
			memcpy(node_prop->value, value, prop_len);
			lists_bind_fdt(gd->dm_root, np_to_ofnode(node), NULL);
		} else {
			printf("found not node:%s\n", path);
		}
	}
	return 0;
}

static int of_set_phandle(char *path, char *phandle)
{
	struct device_node *node, *pnode;
	struct property *node_prop, *pnode_prop;
	int len = 0 ;

	pnode = of_find_node_opts_by_path(phandle, NULL);
	if(pnode){
		pnode_prop = of_find_property(pnode, "phandle", &len);
		if(pnode_prop){
			node = of_find_node_opts_by_path(path, NULL);
			if(node){
				node_prop = of_find_property(node, "connect", &len);
				if(node_prop) {
					memcpy(node_prop->value, pnode_prop->value, len);
				} else {
					printf("found not node:%s\n", path);
				}
			}
		}
	}
	return 0;
}

static int forlinx_display_is_force(void)
{
	void *fdt = (void*)gd->fdt_blob;
	int offset;
	offset = fdt_path_offset(fdt, "/forlinx-control");
	if(offset > 0){
		//fdt_getprop	
		printf("display forced by kernel dtb\n");
		return fdtdec_get_is_enabled(fdt, offset);
	}
	return 0;
}

static void forlinx_fixup_display_force(void)
{
	const void *fdt = gd->fdt_blob;
	int offset;
	int dsp, vp;
	const char *prop;
	char env[64];
	offset = fdt_path_offset(fdt, "/forlinx-control");
	if(offset > 0 && fdtdec_get_is_enabled(fdt, offset)){

		for(vp = 0; vp < 4; vp++){
			sprintf(env, "video_vp%d", vp);
			env_set(env, "OFF");
		}
		for(dsp = hdmi0_t; dsp < dsp_max_t; dsp++){
			prop = fdt_getprop(fdt, offset, inf_name[dsp], NULL);
			if(prop == NULL)
				continue;
			for(vp = 0; vp < 4; vp++){
				if(!strcmp(prop, vps_name[vp])){
					sprintf(env, "video_vp%d", vp);
					env_set(env, inf_name[dsp]);
					printf("force %s=%s\n", env, inf_name[dsp]);
				}
			}
		}
		//primary
		prop = fdt_getprop(fdt, offset, "primary_display", NULL);
		if(prop){
			env_set("primary_display", prop);
			printf("force primay %s\n", prop);
			//update secondary_display
			display_update_secondary();
		}
		//update primary hdmi or dp resolotion
		prop = fdt_getprop(fdt, offset, "primary_display_resolution", NULL);
		if(prop){
			env_set("disp_hdmi_resolution", prop);
			printf("force primary hdmi or dp resolution %s\n", prop);
		}

		//update disp type
		prop = fdt_getprop(fdt, offset, "disp_type", NULL);
		if(prop){
			env_set("disp_type", prop);
			printf("disp_type %s\n", prop);
		}
		
//		env_save();   //no need save, just use env in ram
	}
}

static void display_change_primary_display_hdmidp(void)
{
	char *res;
	int idx;
	res = env_get("disp_hdmi_resolution");
	if(res){
		for(idx = 0; idx < PRIMARY_RESOLUTIONS; idx++){
			if(!strcmp(res, primary_resolution[idx])){
				break;
			}
		}
		if(idx < PRIMARY_RESOLUTIONS){
			idx = (idx + 1) % PRIMARY_RESOLUTIONS;
			env_set("disp_hdmi_resolution", primary_resolution[idx]);
		} else {
			env_set("disp_hdmi_resolution", primary_resolution[0]);
		}
	}
}

void forlinx_fixup_display(void)
{
#ifndef CONFIG_FORLINX_OK3588_LINUX
	char *cfg = NULL;
	char *root_access = NULL;
	char buff[128];
#endif

	char *vps[32];
	char vpname[32];
	int vp, dsp;
	char endpoint[128];
	bool dp0 = false;
	bool dp1 = false;
#ifndef CONFIG_FORLINX_OK3588_LINUX
	char *primary_env = NULL;
	char *secondary_env= NULL;
	char *hdmi_resolution = NULL;
#endif
	forlinx_fixup_display_force();

#ifndef CONFIG_FORLINX_OK3588_LINUX
	cfg = env_get("android_navigationbar");
	if (cfg != NULL) {
		sprintf(buff, "androidboot.hidden_navigationbar=%s", cfg);
		env_update("bootargs", buff);
	} else  {
		env_update("bootargs", "androidboot.hidden_navigationbar=off");
	}

	cfg = env_get("android_topsliding");
	if (cfg != NULL) {
		sprintf(buff, "androidboot.top_list_sliding=%s", cfg);
		env_update("bootargs", buff);
	} else {
		env_update("bootargs", "androidboot.top_list_sliding=off");
	}

	root_access = env_get("android_root_access");
	if (root_access != NULL) {
		sprintf(buff, "androidboot.root_access=%s", root_access);
		env_update("bootargs", buff);
	} else {
		env_update("bootargs", "androidboot.root_access=off");
	}

	primary_env = env_get("primary_display");
	if (primary_env != NULL) {
		sprintf(buff, "androidboot.primary_display=%s", primary_env);
		env_update("bootargs", buff);
	} else {
		env_update("bootargs", "androidboot.primary_display=HDMI0");
	}

	secondary_env = env_get("secondary_display");
	if(secondary_env != NULL) {
		sprintf(buff, "androidboot.secondary_display=%s", secondary_env);
		env_update("bootargs", buff);
	} else {
		env_update("bootargs", "androidboot.secondary_display=EDP1,MIPI0,MIPI1");
	}

	hdmi_resolution = env_get("disp_hdmi_resolution");
	if (hdmi_resolution != NULL) {
		sprintf(buff, "androidboot.disp_hdmi_resolution=%s", hdmi_resolution);
		env_update("bootargs", buff);
	} else {
		env_update("bootargs", "androidboot.disp_hdmi_resolution=1920x1080");
	}
#endif
	env_save();

	for(vp = 0; vp < 4; vp++){
		sprintf(vpname, "video_vp%d", vp);
		vps[vp] = env_get(vpname);

		if(vps[vp] == NULL || !strncmp(vps[vp], "OFF", 3))
			continue;

		for(dsp = hdmi0_t; dsp < dsp_max_t; dsp++){
			if(strcmp(vps[vp], inf_name[dsp]) == 0)
				break;
		}

		switch(dsp){
			case hdmi0_t:
				sprintf(endpoint, "/hdmi@fde80000/ports/port@0/endpoint@%d", vp);
				of_set_property_by_path(endpoint, "status", "okay", 5, 1);
				of_set_property_by_path("/display-subsystem/route/route-hdmi0", "status", "okay", 5, 1);
				of_set_property_by_path("/hdmi@fde80000", "status", "okay", 5, 1);
				of_set_property_by_path("/hdmiphy@fed60000", "status", "okay", 5, 1);
				sprintf(endpoint, "/vop@fdd90000/ports/port@%d/endpoint@2", vp);
				of_set_phandle("/display-subsystem/route/route-hdmi0", endpoint);
				break;
			case hdmi1_t:
				sprintf(endpoint, "/hdmi@fdea0000/ports/port@0/endpoint@%d", vp);
				of_set_property_by_path(endpoint, "status", "okay", 5, 1);
				of_set_property_by_path("/display-subsystem/route/route-hdmi1", "status", "okay", 5, 1);
				of_set_property_by_path("/hdmi@fdea0000", "status", "okay", 5, 1);
				of_set_property_by_path("/hdmiphy@fed70000", "status", "okay", 5, 1);

				sprintf(endpoint, "/vop@fdd90000/ports/port@%d/endpoint@%d", vp, vp==2?7:5);
				of_set_phandle("/display-subsystem/route/route-hdmi1", endpoint);
				break;
			case edp0_t:
				sprintf(endpoint, "/edp@fdec0000/ports/port@0/endpoint@%d", vp);
				of_set_property_by_path(endpoint, "status", "okay", 5, 1);
				of_set_property_by_path("/display-subsystem/route/route-edp0", "status", "okay", 5, 1);
				of_set_property_by_path("/edp@fdec0000", "status", "okay", 5, 1);
				of_set_property_by_path("/phy@fed60000", "status", "okay", 5, 1);

				sprintf(endpoint, "/vop@fdd90000/ports/port@%d/endpoint@1", vp);
				of_set_phandle("/display-subsystem/route/route-edp0", endpoint);
				break;
			case edp1_t:
				sprintf(endpoint, "/edp@fded0000/ports/port@0/endpoint@%d", vp);
				of_set_property_by_path(endpoint, "status", "okay", 5, 1);
				of_set_property_by_path("/display-subsystem/route/route-edp1", "status", "okay", 5, 1);
				of_set_property_by_path("/edp@fded0000", "status", "okay", 5, 1);
				of_set_property_by_path("/phy@fed70000", "status", "okay", 5, 1);

				sprintf(endpoint, "/vop@fdd90000/ports/port@%d/endpoint@%d", vp, vp==2?6:4);
				of_set_phandle("/display-subsystem/route/route-edp1", endpoint);
				break;
			case dp0_t:
				sprintf(endpoint, "/dp@fde50000/ports/port@0/endpoint@%d", vp);
				of_set_property_by_path(endpoint, "status", "okay", 5, 1);
				of_set_property_by_path("/display-subsystem/route/route-dp0", "status", "okay", 5, 1);
				of_set_property_by_path("/dp@fde50000", "status", "okay", 5, 1);
				of_set_property_by_path("/phy@fed80000", "status", "okay", 5, 1);

				sprintf(endpoint, "/vop@fdd90000/ports/port@%d/endpoint@0", vp);
				of_set_phandle("/display-subsystem/route/route-dp0", endpoint);
				dp0 = true;
				break;
			case dp1_t:
				sprintf(endpoint, "/dp@fde60000/ports/port@0/endpoint@%d", vp);
				of_set_property_by_path(endpoint, "status", "okay", 5, 1);
				of_set_property_by_path("/display-subsystem/route/route-dp1", "status", "okay", 5, 1);
				of_set_property_by_path("/dp@fde60000", "status", "okay", 5, 1);
				of_set_property_by_path("/phy@fed90000", "status", "okay", 5, 1);

				sprintf(endpoint, "/vop@fdd90000/ports/port@%d/endpoint@%d", vp, vp==2?5:3);
				of_set_phandle("/display-subsystem/route/route-dp1", endpoint);
				dp1 = true;
				break;
			case mipi0_t:
				sprintf(endpoint, "/dsi@fde20000/ports/port@0/endpoint@%d", vp - 2);
				of_set_property_by_path(endpoint, "status", "okay", 5, 1);
				of_set_property_by_path("/display-subsystem/route/route-dsi0", "status", "okay", 5, 1);
				of_set_property_by_path("/dsi@fde20000", "status", "okay", 5, 1);
				of_set_property_by_path("/phy@feda0000", "status", "okay", 5, 1);

				sprintf(endpoint, "/vop@fdd90000/ports/port@%d/endpoint@%d", vp, vp==2?3:0);
				of_set_phandle("/display-subsystem/route/route-dsi0", endpoint);
				break;
			case mipi1_t:
				sprintf(endpoint, "/dsi@fde30000/ports/port@0/endpoint@%d", vp - 2);
				of_set_property_by_path(endpoint, "status", "okay", 5, 1);
				of_set_property_by_path("/display-subsystem/route/route-dsi1", "status", "okay", 5, 1);
				of_set_property_by_path("/dsi@fde30000", "status", "okay", 5, 1);
				of_set_property_by_path("/phy@fedb0000", "status", "okay", 5, 1);

				sprintf(endpoint, "/vop@fdd90000/ports/port@%d/endpoint@%d", vp, vp==2?4:1);
				of_set_phandle("/display-subsystem/route/route-dsi1", endpoint);
				break;
			case rgb_t:
				sprintf(endpoint, "/syscon@fd58c000/rgb/ports/port@0/endpoint@2");
				of_set_property_by_path(endpoint, "status", "okay", 5, 1);
				of_set_property_by_path("/display-subsystem/route/route-rgb", "status", "okay", 5, 1);
				of_set_property_by_path("/syscon@fd58c000/rgb", "status", "okay", 5, 1);
				break;
		}

	}
	if(dp0)
		of_set_property_by_path("/i2c@feaa0000/fusb302@22/connector/altmodes", "status", "okay", 5, 1);
	else
		of_set_property_by_path("/i2c@feaa0000/fusb302@22/connector/altmodes", "status", "disabled", 9, 1);
	if(dp1)
		of_set_property_by_path("/i2c@fead0000/fusb302@22/connector/altmodes", "status", "okay", 5, 1);
	else
		of_set_property_by_path("/i2c@fead0000/fusb302@22/connector/altmodes", "status", "disabled", 9, 1);
	if(!strstr(env_get("bootargs"), "loglevel") && !strcmp(env_get("loglevel"), "1"))
        {
                env_update("bootargs", "loglevel=1");
		env_save();
        }
	board_pcie_enable();
}


void board_android_systemui_config(void)
{
	int c = 0;
	char *nav = NULL, *sliding = NULL, *root_access;
	while(1){
		nav = env_get("android_navigationbar");
		sliding = env_get("android_topsliding");
		root_access = env_get("android_root_access");
		printf(
				"---------------------------------------------\n"
				"android_navigationbar==>%s android_topsliding==>%s android_root_access==>%s\n"
				"Select  android config\n"
				"0:Exit\n"
				"1:android navigationbar display %s\n"
				"2:android topsliding display %s\n"
				"3:android root access status %s\n"
				"---------------------------------------------\n"
				, nav, sliding, root_access, nav, sliding, root_access
			  );
		c = getc();
		switch(c) {
			case '0':
				return ;
			case '1':
				if(!strcmp(nav, "on"))
					env_set("android_navigationbar", "off");
				else 
					env_set("android_navigationbar", "on");
				break;
			case '2':
				if(!strcmp(sliding, "on"))
					env_set("android_topsliding", "off");
				else 
					env_set("android_topsliding", "on");
				break;
			case '3':
				if(!strcmp(root_access, "on"))
					env_set("android_root_access", "off");
				else 
					env_set("android_root_access", "on");
				break;
		}
		env_save();
	}
}

static int display_port_update_cfg(void)
{
	char *vp0, *vp1, *vp2, *vp3;
	int i, j;
	vp0 = env_get("video_vp0");
	vp1 = env_get("video_vp1");
	vp2 = env_get("video_vp2");
	vp3 = env_get("video_vp3");

	vp_use[0] = dsp_max_t;
	vp_use[1] = dsp_max_t;
	vp_use[2] = dsp_max_t;
	vp_use[3] = dsp_max_t;

	for(i = 0; i < dsp_max_t; i++){
		if(!strcmp(vp0, inf_name[i]))
			vp_use[0] = i;
		else if(!strcmp(vp1, inf_name[i]))
			vp_use[1] = i;
		else if(!strcmp(vp2, inf_name[i]))
			vp_use[2] = i;
		else if(!strcmp(vp3, inf_name[i]))
			vp_use[3] = i;
	}

	for(i = 0; i < dsp_max_t; i++){
		display_port_cfg[i] = vp_off_t;
		for(j = 0; j < 4; j++){
			if(vp_use[j] == i){
				display_port_cfg[i] = j;
				break;
			}
		}
//		if(j == 4)
//			display_port_cfg[i] = vp_off_t;
	}
	return 0;
}

static int display_change_vp_env(_vp_t old, _dsp_t dsp)
{
	char vpenv [16];
	int i;
	_vp_t next = old + 1;

	if(old != vp_off_t){ //turn off old vp
		sprintf(vpenv, "video_vp%d", old);
		env_set(vpenv, "OFF");
	}

	//hdmi & edp

	for(i = 0 ; i < 4; i++){
		if(dsp == edp0_t && vp_use[i] == hdmi0_t)
			return 0;
		if(dsp == edp1_t && vp_use[i] == hdmi1_t)
			return 0;
		if(dsp == hdmi0_t && vp_use[i] == edp0_t){
			sprintf(vpenv, "video_vp%d", i);
			env_set(vpenv, "OFF");
		}
		if(dsp == hdmi1_t && vp_use[i] == edp1_t){
			sprintf(vpenv, "video_vp%d", i);
			env_set(vpenv, "OFF");
		}
	}

	//detect vp next
	if(next == vp_max_t)
		next = vp0_t;

	for(i = next; i < 4; i++){
		if(vp_use[i] <= dsp 
				|| (i == 3 && dsp < mipi0_t)
				|| (( dsp == mipi0_t || dsp == mipi1_t ) && i < vp2_t)
				|| (dsp == rgb_t && i != vp3_t))
			continue;
		else 
			break;
	}

	next = i;//vp_off_t;

	if(next != vp_off_t){ 
		sprintf(vpenv, "video_vp%d", next);
		env_set(vpenv, inf_name[dsp]);
	} 
	return 0;
}

static void display_update_primary(void)
{
	char *primary;
	int i = 0, idx;
	primary = env_get("primary_display");
	for(idx = 0; idx < 4; idx++){
		if(strcmp(primary, inf_name[vp_use[idx]]) == 0)
			break;
	}

	if(idx == 4){ //update
		for(i = 0; i < 4; i++){
			if(vp_use[i] != dsp_max_t){
				env_set("primary_display", inf_name[vp_use[i]]);
				return ;
			}
		}
		env_set("primary_display", "off");
	}
	env_save();
}

static void display_update_secondary(void)
{
	char *primary;
	int vp;
	char vpenv[64];
	char *vpinf;
	char secval[128];

	primary = env_get("primary_display");
	if(primary == NULL || !strcmp(primary, "off") || !strcmp(primary, "OFF")){
		env_set("secondary_display", "off");
		return ;
	}

	secval[0] = 0;
	for(vp = 0; vp < 4; vp++){
		sprintf(vpenv, "video_vp%d", vp);
		vpinf = env_get(vpenv);
		if(vpinf == NULL || !strcmp(vpinf, "OFF") || ! strcmp(vpinf, "off") || !strcmp(vpinf, primary))
			continue;
		strcat(secval, vpinf);
		strcat(secval, ",");
	}
	if(strlen(secval) > 0 )
		secval[strlen(secval) - 1] = 0;
	env_set("secondary_display", secval);
	env_save();
}

static void display_change_primary(void)
{
	char *primary;
	int i = 0, idx;
	primary = env_get("primary_display");
	for(idx = 0; idx < 4; idx++){
		if(strcmp(primary, inf_name[vp_use[idx]]) == 0)
			break;
	}
	i = idx;;
	while(1){

		if(i == 4)
			i = 0;
		else 
			i = i + 1;
		if(i == idx)
			break;

		i = i % 4;
		if(vp_use[i] != dsp_max_t){
			env_set("primary_display", inf_name[vp_use[i]]);
			break;
		}
	}
}

static void board_pcie_enable()
{
	char *pcie3;
	pcie3 = env_get("pcie3");
	if(!strcmp(pcie3 , "on")){
		of_set_property_by_path("/phy@fee80000", "status", "okay", 5, 1);
		of_set_property_by_path("/pcie@fe150000", "status", "okay", 5, 1);
	}else{
		of_set_property_by_path("/pcie@fe150000", "status", "disabled", 9, 1);
		of_set_property_by_path("/phy@fee80000", "status", "disabled", 9, 1);
	}
	
}

static void board_display_select(void)
{
    int c = 0;
    while(1){
		
		display_port_update_cfg();
		display_update_primary();
		display_update_secondary();

        printf(
                "---------------------------------------------\n"
                "hdmi0 and edp0 share same port, only one can be used.\n"
                "hdmi1 and edp1 share same port, only one can be used.\n"
                "only four VPs internally, so up to four interfaces can be activated\n"
                "hdmi edp dp can only be displayed on VP0 or VP1 or VP2.\n"
                "dsi0 dsi1 can only be displayed on VP2 or  VP3.\n"
                "rgb can only be displayed on VP3.\n"

                "\nSelect  display\n"
                "  0:Exit\n"

                "  1: hdmi0 => %s\n"     //vp0  vp1 vp2 off
                "  2: hdmi1 => %s\n"
                "  3: edp0  => %s\n"     //vp0  vp1 vp2 off
                "  4: edp1  => %s\n"
                "  5: dp0   => %s\n"
                "  6: dp1   => %s\n"
                "  7: mipi0 => %s\n"
                "  8: mipi1 => %s\n"	//vp2 vp3
                "  9: rgb   => %s\n"	//vp3
                "  a: primary display  => %s\n"
                "  b: primary display resolution => %s\n"
#ifdef CONFIG_FORLINX_OK3588_LINUX
				"  c: display type => %s\n"
#endif
                "---------------------------------------------\n",
				vps_name[display_port_cfg[0]],
				vps_name[display_port_cfg[1]],
				vps_name[display_port_cfg[2]],
				vps_name[display_port_cfg[3]],
				vps_name[display_port_cfg[4]],
				vps_name[display_port_cfg[5]],
				vps_name[display_port_cfg[6]],
				vps_name[display_port_cfg[7]],
				vps_name[display_port_cfg[8]],
				env_get("primary_display"),
				env_get("disp_hdmi_resolution")
#ifdef CONFIG_FORLINX_OK3588_LINUX
				,env_get("disp_type")
#endif
              );
				while (1){
					c = getc();
					if(c == '\n')
						continue;
					else 
						break;
				}
        switch (c){
            case '0':
                return;
            case '1' ... '9':
				if(forlinx_display_is_force())
					break;
				display_change_vp_env(display_port_cfg[c - '1' + hdmi0_t], c - '1' + hdmi0_t);
                break;
			case 'a':
				if(forlinx_display_is_force())
					break;
				display_change_primary();
				break;
			case 'b':
				if(forlinx_display_is_force())
					break;
				display_change_primary_display_hdmidp();
				break;
#ifdef CONFIG_FORLINX_OK3588_LINUX
			case 'c':
				if (strstr(env_get("disp_type"), "async") != NULL)
					env_set("disp_type", "sync");
				else
					env_set("disp_type", "async");
				break;	
#endif
        }
        env_save();
    }
}

void ctrl_ubootmenu_display(void)
{
	int c = 0;
	char *loglevel;
	char *pcie3;
	while(1) {
		loglevel = env_get("loglevel");
		pcie3 = env_get("pcie3");
		printf(
				"---------------------------------------------\n"
				"0:Exit to console\n"
				"1:Reboot\n"
				"2:Display type\n"
				"3:Change kernel loglevel( level %s )\n"
				"4:Enable PCIE3 function(State %s)\n"
#ifndef CONFIG_FORLINX_OK3588_LINUX
				"5:Android config\n"
#endif
				"---------------------------------------------\n"
				, loglevel , pcie3
			  );
		c = getc();
		switch(c){
			case '0':
				return ;
			case '1':
				run_command("reset", 0);
			case '2':
				board_display_select();
				break;
			case '3':
				if(!strcmp(loglevel, "7")){
					env_set("loglevel", "1");
					env_update("bootargs", "loglevel=1");
				}
				else{
					env_set("loglevel", "7");
					env_update("bootargs", "loglevel=7");
				}
				break;
			case '4':
				if(!strcmp(pcie3, "on"))
				{
					env_set("pcie3" , "off");
				}else{
					env_set("pcie3" , "on");
				}
#ifndef CONFIG_FORLINX_OK3588_LINUX
			case '5':
				board_android_systemui_config();
				break;
#endif
		}
		env_save();
	}
}

