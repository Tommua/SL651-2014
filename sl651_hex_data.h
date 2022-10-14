#ifndef __SL651_HEX__H__
#define __SL651_HEX__H__

#include <string.h>
#include "time.h"
#include "math.h"
#include <stdio.h>

#define RT_NULL    (0)

struct rt_slist_node
{
    struct rt_slist_node *next;                         /**< point to next node. */
};
typedef struct rt_slist_node rt_slist_t;                /**< Type for single list. */

#define rt_container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))

#define rt_slist_entry(node, type, member) \
    rt_container_of(node, type, member)

#define rt_slist_for_each(pos, head) \
    for (pos = (head)->next; pos != RT_NULL; pos = pos->next)

static __inline void rt_slist_init(rt_slist_t *l)
{
    l->next = RT_NULL;
}

static __inline void rt_slist_append(rt_slist_t *l, rt_slist_t *n)
{
    struct rt_slist_node *node;

    node = l;
    while (node->next) node = node->next;

    /* append the node to the tail */
    node->next = n;
    n->next = RT_NULL;
}

struct data_info{
	char* tag;
	time_t timestamp;
	double up_threshold;
	double down_threshold;
    unsigned char sl_signal;

	double ratio;    //系数
	double val;
	double his_data[12];
	
	rt_slist_t datalist;
};

struct dev_info{
	char* name;
	int station;
	rt_slist_t dataslist;
};

//遥测站
typedef enum {
    PRECIPITATION_ST = 0,   //降水
    RIVER_ST,               //河道
    RESERVOIR_ST,           //水库
    GATE_DAM_ST,            //闸坝
    PUMPING_ST,             //泵站
    TIDE_ST,                //潮汐
    MOISTURE_ST,            //墒情
    GROUNDWATER_ST,         //地下水
    WATERQ_ST,              //水质
    WINTAKE_ST,             //取水口
    OUTLET_ST,              //排水口
    BUTT_ST,         
}TYPE_REMOTE_ST_E;

typedef enum{
	KEEP_CON_CODE_TYPE = 0,
	TEST_CODE_TYPE,
	EQU_INTERVAL_CODE_TYPE,
	REG_UPLOAD_CODE_TYPE,
	TRIGGLE_UPLOAD_CODE_TYPE,
	HOUR_UPLOAD_CODE_TYPE,
	REALTIME_DATA_CODE_TYPE,
	CODE_TYPE_BUTT  
}FUNC_CODE_TYPE;


#define TIME_INTERVAL_D      0x18  //时间步长数据定义
#define VT_D                 0x22  //电压数据定义

#define INTERVAL             30  //30 min 

//控制字定义
#define SINGAL_CTRL_START    0x7E
#define SINGAL_CTRL_STX      0x02
#define SINGAL_CTRL_SYN      0x16
#define SINGAL_CTRL_ETX      0x03
#define SINGAL_CTRL_ETB      0x17
#define SINGAL_CTRL_ENQ      0x05
#define SINGAL_CTRL_EOT      0x04
#define SINGAL_CTRL_ACK      0x06
#define SINGAL_CTRL_NAK      0x15
#define SINGAL_CTRL_ESC      0x1B

//功能码
#define KEEP_CON_CODE         0x2F  //链路维持报
#define TEST_CODE             0x30  //测试报
#define EQU_INTERVAL_CODE     0x31  //均匀时段水温信息报
#define REG_UPLOAD_CODE       0x32  //遥测站定时报
#define TRIGGLE_UPLOAD_CODE   0x33  //遥测站加报报
#define HOUR_UPLOAD_CODE      0x34  //遥测站小时报

//要素标识符
#define SIGNAL_GUIDE_TT            0xF0
#define SIGNAL_GUIDE_ST            0xF1
#define SIGNAL_GUIDE_DRP           0xF4
#define SIGNAL_GUIDE_DRZ1          0xF5
#define SIGNAL_GUIDE_DRZ2          0xF6
#define SIGNAL_GUIDE_DRZ3          0xF7
#define SIGNAL_GUIDE_DRZ4          0xF8
#define SIGNAL_GUIDE_DRZ5          0xF9
#define SIGNAL_GUIDE_DRZ6          0xFA
#define SIGNAL_GUIDE_DRZ7          0xFB
#define SIGNAL_GUIDE_DRZ8          0xFC

#define SIGNAL_GUIDE_AC            0x01
#define SIGNAL_AIR_TEMP            0x02
#define SIGNAL_WATER_TEMP          0x03
#define SIGNAL_DRXNN               0x04
#define SIGNAL_GUIDE_DT            0x05
#define SIGNAL_GUIDE_ED            0x06
#define SIGNAL_GUIDE_EJ            0x07
#define SIGNAL_GUIDE_FL            0x08
#define SIGNAL_GUIDE_GH            0x09
#define SIGNAL_GUIDE_GN            0x0A
#define SIGNAL_GUIDE_GS            0x0B
#define SIGNAL_GUIDE_GT            0x0C
#define SIGNAL_GUIDE_GTP           0x0D
#define SIGNAL_GUIDE_H             0x0E
#define SIGNAL_GUIDE_HW            0x0F
#define SIGNAL_GUIDE_M10           0x10
#define SIGNAL_GUIDE_M20           0x11
#define SIGNAL_GUIDE_M30           0x12
#define SIGNAL_GUIDE_M40           0x13
#define SIGNAL_GUIDE_M50           0x14
#define SIGNAL_GUIDE_M60           0x15
#define SIGNAL_GUIDE_M80           0x16
#define SIGNAL_GUIDE_M100          0x17
#define SIGNAL_MST                 0x18
#define SIGNAL_NS                  0x19
#define SIGNAL_P1                  0x1A
#define SIGNAL_P2                  0x1B
#define SIGNAL_P3                  0x1C
#define SIGNAL_P6                  0x1D
#define SIGNAL_P12                 0x1E
#define SIGNAL_PD                  0x1F
#define SIGNAL_PJ                  0x20
#define SIGNAL_PN01                0x21
#define SIGNAL_PN05                0x22
#define SIGNAL_PN10                0x23
#define SIGNAL_PN30                0x24
#define SIGNAL_PR                  0x25
#define SIGNAL_PT                  0x26
#define SIGNAL_Q                   0x27
#define SIGNAL_Q1                  0x28
#define SIGNAL_Q2                  0x29
#define SIGNAL_Q3                  0x2A
#define SIGNAL_Q4                  0x2B
#define SIGNAL_Q5                  0x2C
#define SIGNAL_Q6                  0x2D
#define SIGNAL_Q7                  0x2E
#define SIGNAL_Q8                  0x2F
#define SIGNAL_QA                  0x30
#define SIGNAL_QZ                  0x31
#define SIGNAL_SW                  0x32
#define SIGNAL_UC                  0x33
#define SIGNAL_UE                  0x34
#define SIGNAL_US                  0x35
#define SIGNAL_VA                  0x36
#define SIGNAL_VJ                  0x37
#define SIGNAL_VT                  0x38
#define SIGNAL_Z                   0x39
#define SIGNAL_ZB                  0x3A
#define SIGNAL_ZU                  0x3B
#define SIGNAL_Z1                  0x3C
#define SIGNAL_Z2                  0x3D
#define SIGNAL_Z3                  0x3E
#define SIGNAL_Z4                  0x3F
#define SIGNAL_Z5                  0x40
#define SIGNAL_Z6                  0x41
#define SIGNAL_Z7                  0x42
#define SIGNAL_Z8                  0x43
#define SIGNAL_SQ                  0x44
#define SIGNAL_ZT                  0x45
#define SIGNAL_PH                  0x46
#define SIGNAL_DO                  0x47
#define SIGNAL_COND                0x48
#define SIGNAL_TURB                0x49
#define SIGNAL_CODMN               0x4A
#define SIGNAL_REDOX	           0x4B
#define SIGNAL_NH4N		           0x4C
#define SIGNAL_TP	               0x4D
#define SIGNAL_TN	               0x4E
#define SIGNAL_TOC	               0x4F
#define SIGNAL_CU	               0x50
#define SIGNAL_ZN	               0x51
#define SIGNAL_SE                  0x52
#define SIGNAL_AS	               0x53
#define SIGNAL_THG	               0x54
#define SIGNAL_CD	               0x55
#define SIGNAL_PB	               0x56
#define SIGNAL_CHLA	               0x57
#define SIGNAL_WP1	               0x58
#define SIGNAL_WP2	               0x59
#define SIGNAL_WP3	               0x5A
#define SIGNAL_WP4	               0x5B
#define SIGNAL_WP5	               0x5C
#define SIGNAL_WP6	               0x5D
#define SIGNAL_WP7	               0x5E
#define SIGNAL_WP8	               0x5F

typedef struct sl651_setting
{
    unsigned char master_st;
    unsigned long int remote_st;
    unsigned short passwd;
	TYPE_REMOTE_ST_E st;
}sl651_setting_t;

extern struct dev_info* dev;
extern double Bat_Cap;
int serialed_sl651_keepalive_frame(unsigned char* buf, sl651_setting_t* setting);
int serialed_sl651_test_frame(unsigned char* buf, sl651_setting_t* setting, struct data_info* datas);
int serialed_sl651_equ_interval_frame(unsigned char* buf, sl651_setting_t* setting, struct data_info* data);
int serialed_sl651_timer_upload_data_frame(unsigned char* buf, sl651_setting_t* setting, struct data_info* datas);
int serialed_sl651_triggle_frame(unsigned char* buf, sl651_setting_t* setting, struct data_info* data);
int serialed_sl651_hour_frame(unsigned char* buf, sl651_setting_t* setting, struct data_info* data);
int serialed_realtime_datas_frame(unsigned char* buf, sl651_setting_t* setting, struct data_info* data);

#endif