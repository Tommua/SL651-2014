#include "sl651_hex_data.h"


sl651_setting_t sl651_setting;
struct dev_info* dev;
int serial_num = 0;

double Bat_Cap = 0;  //电池电量全局变量

unsigned char Remote_ST[BUTT_ST] = {0x50, 0x48, 0x4B, 0x5A, 0x44, 0x54, 0x4D, 0x47, 0x51, 0x49, 0x4F};
unsigned char Func_Code[CODE_TYPE_BUTT] = {0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x37};

//十进制转bcd码
static int dec2bcd(unsigned char data)
{
    unsigned char temp;
    temp = (((data/10)<<4) + (data%10));
    return temp; 
}

//crc校验
static unsigned short sl651_crc(unsigned char *buf, int usDataLen)
{
    int i;
    unsigned short crc_reg, j, check;
    crc_reg = 0xFFFF;
    for (i = 0; i < usDataLen; i++)
    {
        crc_reg = (crc_reg >> 8) ^ buf[i];
        for (j = 0; j < 8; j++)
        {
            check = crc_reg & 0x0001;
            crc_reg >>= 1;
            if (check == 0x0001)
            {
                crc_reg ^= 0xA001;
            }
        }
    }
    return crc_reg;
}

static int sl651_dec2hex(unsigned char* bufIn, double dval, int nb_byte, int nb_dec)
{
    int val = 0;
    val = (int)(dval*pow(10, nb_dec));
    for (int i = 0; i < nb_byte; i++)
    {
        bufIn[i] = val >> (nb_byte - i - 1);
    }
}

static void sl651_dec2bcd(unsigned char* bufIn, double dval, int nb_byte, int nb_dec)
{
    int val = 0;
    int data_len = 0;

    val = (int)(dval*pow(10, nb_dec));

    char tmp[32];
    memset(tmp, 0, 32);
    sprintf(tmp, "%d", val);

    data_len = strlen(tmp);

    memset(tmp, 0, 32);

    for (int i = 0; i < data_len; i++)
    {
        
        if(data_len % 2)
        {
            tmp[0] = 0;

            tmp[i + 1] = (int)(val / pow(10, data_len - i - 1)) % 10;
        }
        else
        {
            tmp[i] = (int)(val / pow(10, data_len - i - 1)) % 10;
        }
    }
    
    int index_tmp = 0;
    data_len = data_len % 2 == 0 ? data_len / 2 : data_len / 2 + 1;

    printf("nb_byte:%d  nb_dec:%d val:%d data len:%d \n", nb_byte, nb_dec, val,  data_len);

    //数据
    for (int i = nb_byte; i > 0; i--)
    {
        if((i - data_len) > 0)
        {
            bufIn[nb_byte - i] = 0;
        }
        else
        {
            bufIn[nb_byte - i] = dec2bcd(tmp[0 + index_tmp*2] * 10 + tmp[1 + index_tmp*2]);
            index_tmp += 1;
        }
    }
}

static int serialed_sl651_datas(unsigned char* ucbuf, unsigned char signal, double* val, int* nb_byte, int* nb_dec)
{
    int len = 0;
    // int nb_byte = 0;
    // int nb_dec = 0;

    unsigned char buf[24] = {0};
    switch (signal)
    {
    case SIGNAL_GUIDE_GS: //N(1)
        *nb_byte = 1;
        *nb_dec = 0;
        sl651_dec2bcd(buf, val[0], 1, 0);
        break;

    case SIGNAL_NS: //N(2)
    case SIGNAL_UC:
    case SIGNAL_UE:
        *nb_byte = 2;
        *nb_dec = 0;
        sl651_dec2bcd(buf, val[0], 2, 0);
        break;
    
    case SIGNAL_GUIDE_GN: //N(3)
    case SIGNAL_GUIDE_GT:
    case SIGNAL_TURB:
        *nb_byte = 3;
        *nb_dec = 0;
        sl651_dec2bcd(buf, val[0], 3, 0);
        break;

    case SIGNAL_AIR_TEMP: //N(3,1)
    case SIGNAL_WATER_TEMP:
    case SIGNAL_GUIDE_GTP:
        *nb_byte = 3;
        *nb_dec = 1;
        sl651_dec2bcd(buf, val[0], 3, 1);
        break;

    case SIGNAL_DRXNN: //N(3) ddmmMM
        break;

    case SIGNAL_GUIDE_M10: //N(4,1)
    case SIGNAL_GUIDE_M20:
    case SIGNAL_GUIDE_M30:
    case SIGNAL_GUIDE_M40:
    case SIGNAL_GUIDE_M50:
    case SIGNAL_GUIDE_M60:
    case SIGNAL_GUIDE_M80:
    case SIGNAL_GUIDE_M100:
    case SIGNAL_MST:
    case SIGNAL_US:
    case SIGNAL_DO:
    case SIGNAL_CODMN:
        *nb_byte = 4;
        *nb_dec = 1;
        sl651_dec2bcd(buf, val[0], 4, 1);
        break;

    case SIGNAL_VT: //N(4,2)
    case SIGNAL_PH:
    case SIGNAL_TOC:
    case SIGNAL_CHLA:
        *nb_byte = 4;
        *nb_dec = 2;
        sl651_dec2bcd(buf, val[0], 4, 2);
        break;

    case SIGNAL_GUIDE_FL: //N(5)
    case SIGNAL_COND:
        *nb_byte = 5;
        *nb_dec = 0;
        sl651_dec2bcd(buf, val[0], 5, 0);
        break;

    case SIGNAL_GUIDE_ED: //N(5,1)
    case SIGNAL_GUIDE_EJ:
    case SIGNAL_P1:
    case SIGNAL_P2:
    case SIGNAL_P3:
    case SIGNAL_P6:
    case SIGNAL_P12:
    case SIGNAL_PD:
    case SIGNAL_PJ:
    case SIGNAL_PN01:
    case SIGNAL_PN05:
    case SIGNAL_PN10:
    case SIGNAL_PN30:
    case SIGNAL_PR:
    case SIGNAL_REDOX:
        *nb_byte = 5;
        *nb_dec = 1;
        sl651_dec2bcd(buf, val[0], 5, 1);
        break;

    case SIGNAL_GUIDE_GH: //N(5,2)
    case SIGNAL_GUIDE_HW:
    case SIGNAL_TN:
    case SIGNAL_WP1:
    case SIGNAL_WP2:
    case SIGNAL_WP3:
    case SIGNAL_WP4:
    case SIGNAL_WP5:
    case SIGNAL_WP6:
    case SIGNAL_WP7:
    case SIGNAL_WP8:
        *nb_byte = 5;
        *nb_dec = 2;
        sl651_dec2bcd(buf, val[0], 5, 2);
        break;

    case SIGNAL_VA: //N(5,3)
    case SIGNAL_VJ:
    case SIGNAL_TP:
        *nb_byte = 5;
        *nb_dec = 3;
        sl651_dec2bcd(buf, val[0], 5, 3);
        break;

    case SIGNAL_PT: //N(6,1)
        *nb_byte = 6;
        *nb_dec = 1;
        sl651_dec2bcd(buf, val[0], 6, 1);
        break;
        
    case SIGNAL_GUIDE_H: //N(6,2)
    case SIGNAL_NH4N:
        *nb_byte = 6;
        *nb_dec = 2;
        sl651_dec2bcd(buf, val[0], 6, 2);
        break;

    case SIGNAL_ZN: //N(6,4)
        *nb_byte = 6;
        *nb_dec = 4;
        sl651_dec2bcd(buf, val[0], 6, 4);
        break;

    case SIGNAL_Z: //N(7,3)
    case SIGNAL_ZB:
    case SIGNAL_ZU:
    case SIGNAL_Z1:
    case SIGNAL_Z2:
    case SIGNAL_Z3:
    case SIGNAL_Z4:
    case SIGNAL_Z5:
    case SIGNAL_Z6:
    case SIGNAL_Z7:
    case SIGNAL_Z8:
        *nb_byte = 7;
        *nb_dec = 3;
        sl651_dec2bcd(buf, val[0], 7, 3);
        break;

    case SIGNAL_CU: //N(7,4)
        *nb_byte = 7;
        *nb_dec = 4;
        sl651_dec2bcd(buf, val[0], 7, 4);
        break;

    case SIGNAL_SE: //N(7,5)
    case SIGNAL_AS:
    case SIGNAL_THG:
    case SIGNAL_CD: 
    case SIGNAL_PB:
        *nb_byte = 7;
        *nb_dec = 5;
        sl651_dec2bcd(buf, val[0], 7, 5);
        break;

    case SIGNAL_GUIDE_AC: //N(8,2)
        *nb_byte = 8;
        *nb_dec = 2;
        sl651_dec2bcd(buf, val[0], 8, 2);
        break;

    case SIGNAL_Q: //N(9,3)
    case SIGNAL_Q1:
    case SIGNAL_Q2:
    case SIGNAL_Q3:
    case SIGNAL_Q4:
    case SIGNAL_Q5:
    case SIGNAL_Q6:
    case SIGNAL_Q7:
    case SIGNAL_Q8:
    case SIGNAL_QA:
    case SIGNAL_QZ:
    case SIGNAL_SQ:
        *nb_byte = 9;
        *nb_dec = 3;
        sl651_dec2bcd(buf, val[0], 9, 3);
        break;

    case SIGNAL_GUIDE_TT:  //N(10)
    case SIGNAL_GUIDE_ST:
        *nb_byte = 10;
        *nb_dec = 0;
        sl651_dec2bcd(buf, val[0], 10, 0);
        break;

    // case SIGNAL_GUIDE_DT: //N(10,2)
    // nb_byte = 10;
    // nb_dec = 2;
    // sl651_dec2bcd(buf, val, 10, 2);
    // break;
    
    case SIGNAL_SW: //N(11,3)
        *nb_byte = 11;
        *nb_dec = 3;
        sl651_dec2bcd(buf, val[0], 11, 3);
        break;

    case SIGNAL_GUIDE_DRP: //(HEX 12B 1)
        *nb_byte = 12;
        *nb_dec = 1;
        for (int i = 0; i < 12; i++)
        {
            sl651_dec2hex(&buf[i], val[i], 1, 1);
        }
        break;

    case SIGNAL_GUIDE_DRZ1: //(HEX 24B 2)
    case SIGNAL_GUIDE_DRZ2:
    case SIGNAL_GUIDE_DRZ3:
    case SIGNAL_GUIDE_DRZ4:
    case SIGNAL_GUIDE_DRZ5:
    case SIGNAL_GUIDE_DRZ6:
    case SIGNAL_GUIDE_DRZ7:
    case SIGNAL_GUIDE_DRZ8:
        *nb_byte = 24;
        *nb_dec = 2;
        for (int i = 0; i < 12; i++)
        {
            sl651_dec2hex(&buf[i * 2], val[i], 2, 2);
        }
        break;


    case SIGNAL_GUIDE_DT: //(HH.mm)
    break;


    // case SIGNAL_ZT: //(HEX 4B)
    // break;
    
    default:
        break;
    }

    // ucbuf[len] = *nb_byte << 3 | *nb_dec;
    // len += 1;

    //数据
    for (int i = 0; i < *nb_byte; i++)
    {
        ucbuf[len] = buf[i];
        len += 1;
    }

    return len;
}

//组装数据帧头部
static void seriled_sl651_head(unsigned char* buf, sl651_setting_t* sl, FUNC_CODE_TYPE code)
{
    //起始符
    buf[0] = 0x7E;
    buf[1] = 0x7E;

    //中心站地址
    buf[2] = sl->master_st;

    //遥测站地址
    buf[3] = (sl->remote_st >> 32) & 0xff;
    buf[4] = (sl->remote_st >> 24) & 0xff;
    buf[5] = (sl->remote_st >> 16) & 0xff;
    buf[6] = (sl->remote_st >> 8) & 0xff;
    buf[7] = sl->remote_st & 0xff;

    //密码
    buf[8] = ((sl->passwd) >> 8) & 0xff;
    buf[9] = (sl->passwd) & 0xff;

    //功能码
    buf[10] = Func_Code[code];

    //报文起始符
    buf[13] = SINGAL_CTRL_STX;
}


//组装数据帧尾部
static int seriled_sl651_end(unsigned char* buf, int len)
{
    int index = 0; 
    //报文结束符
    buf[len] = SINGAL_CTRL_ETX;
    index += 1;

    //crc校验
    unsigned short crc = 0;
    crc = sl651_crc(&buf[0], len + index);

    //校验位
    buf[len + index] = (crc >> 8) & 0xff;
    index += 1;
    buf[len + index] = crc & 0xff;
    index += 1;

    return len + index;
}

//sl651链路维持报文
int serialed_sl651_keepalive_frame(unsigned char* buf, sl651_setting_t* setting)
{
    seriled_sl651_head(buf, setting, KEEP_CON_CODE_TYPE);

    serial_num += 1;
    serial_num = serial_num > 65535 ? 0 : serial_num;

    //流水号
    buf[14] = serial_num / 256;
    buf[15] = serial_num % 256;

    time_t now;
    now = time(&now);

    struct tm* tm_n;
	tm_n = localtime(&now);

    //时间戳
    // buf[16] = (tm_n->tm_year + 1900) % 100;
    // buf[17] = tm_n->tm_mon + 1;
    // buf[18] = tm_n->tm_mday;
    // buf[19] = tm_n->tm_hour;
    // buf[20] = tm_n->tm_min;
    // buf[21] = tm_n->tm_sec;

    buf[16] = dec2bcd((tm_n->tm_year + 1900) % 100);
    buf[17] = dec2bcd(tm_n->tm_mon + 1);
    buf[18] = dec2bcd(tm_n->tm_mday);
    buf[19] = dec2bcd(tm_n->tm_hour);
    buf[20] = dec2bcd(tm_n->tm_min);
    buf[21] = dec2bcd(tm_n->tm_sec);

    //上下行标识及正文长度
    unsigned short signal_up_len = 0;
    signal_up_len |= (21 - 13);
    buf[11] = (signal_up_len >> 8) & 0xff;
    buf[12] = signal_up_len & 0xff;

    return seriled_sl651_end(buf, 22);
}

//sl651测试上报数据帧
int serialed_sl651_test_frame(unsigned char* buf, sl651_setting_t* setting, struct data_info* datas)
{
    int index = 0;
    int tt_set = 0;
    int data_len = 0;

    seriled_sl651_head(buf, setting, REG_UPLOAD_CODE_TYPE);


    serial_num += 1;
    serial_num = serial_num > 65535 ? 0 : serial_num;

    //流水号
    buf[14] = serial_num / 256;
    buf[15] = serial_num % 256;

    time_t now;
    now = time(&now);

    struct tm* tm_n;
	tm_n = localtime(&now);

    //时间戳
    buf[16] = dec2bcd((tm_n->tm_year + 1900) % 100);
    buf[17] = dec2bcd(tm_n->tm_mon + 1);
    buf[18] = dec2bcd(tm_n->tm_mday);
    buf[19] = dec2bcd(tm_n->tm_hour);
    buf[20] = dec2bcd(tm_n->tm_min);
    buf[21] = dec2bcd(tm_n->tm_sec);   

    //遥测站地址标识符
    buf[22] = SIGNAL_GUIDE_ST;
    buf[23] = SIGNAL_GUIDE_ST;

    //遥测站地址
    buf[24] = buf[3];
    buf[25] = buf[4];
    buf[26] = buf[5];
    buf[27] = buf[6];
    buf[28] = buf[7];

    //遥测站分类码
    buf[29] = Remote_ST[setting->st];

    //观测时间标识符
    buf[30] = SIGNAL_GUIDE_TT;
    buf[31] = SIGNAL_GUIDE_TT;

    int each_data_len = 0;
    int nb_byte = 0, nb_dec = 0; 
    rt_slist_t *node_data;
    rt_slist_for_each(node_data, &(dev->dataslist))
    {
        struct data_info* data = rt_slist_entry(node_data, struct data_info, datalist);
        if(data != NULL)
        {
            if(data->sl_signal == SIGNAL_VT)
                continue;

            if(!tt_set)
            {
                tt_set = 1;
                tm_n = localtime(&data->timestamp);
                //观测时间
                buf[32] = dec2bcd((tm_n->tm_year + 1900) % 100);
                buf[33] = dec2bcd(tm_n->tm_mon + 1);
                buf[34] = dec2bcd(tm_n->tm_mday);
                buf[35] = dec2bcd(tm_n->tm_hour);
                buf[36] = dec2bcd(tm_n->tm_min);
                index += 1;
            }

            //数据定义
            buf[36 + index] = data->sl_signal;
            index += 1;
            
            each_data_len = serialed_sl651_datas(&buf[36 + index + 1], data->sl_signal, &data->val, &nb_byte, &nb_dec);
            buf[36 + index] = nb_byte << 3 | nb_dec;
            index += each_data_len; 
        }
    }

    index += 1;

    //电压
    buf[36 + index] = SIGNAL_VT;
    index += 1;
    // buf[36 + index] = VT_D;
    // index += 1;
    //电压数据
    
    each_data_len = serialed_sl651_datas(&buf[36 + index + 1], SIGNAL_VT, &Bat_Cap, &nb_byte, &nb_dec);
    buf[36 + index] = nb_byte << 3 | nb_dec;
    index += each_data_len + 1;

    //上下行标识及正文长度
    unsigned short signal_up_len = 0;
    signal_up_len |= (index + 36 - 14);
    buf[11] = (signal_up_len >> 8) & 0xff;
    buf[12] = signal_up_len & 0xff;

    return seriled_sl651_end(buf, 36 + index);
}

//sl651均匀时段上报数据帧
int serialed_sl651_equ_interval_frame(unsigned char* buf, sl651_setting_t* setting, struct data_info* data)
{
    if (data->sl_signal == SIGNAL_VT)
    {
        return 0;
    }
    
    int index = 0;
    int tt_set = 0;
    int data_len = 0;

    seriled_sl651_head(buf, setting, EQU_INTERVAL_CODE_TYPE);


    serial_num += 1;
    serial_num = serial_num > 65535 ? 0 : serial_num;

    //流水号
    buf[14] = serial_num / 256;
    buf[15] = serial_num % 256;

    time_t now;
    now = time(&now);

    struct tm* tm_n;
	tm_n = localtime(&now);

    //时间戳
    buf[16] = dec2bcd((tm_n->tm_year + 1900) % 100);
    buf[17] = dec2bcd(tm_n->tm_mon + 1);
    buf[18] = dec2bcd(tm_n->tm_mday);
    buf[19] = dec2bcd(tm_n->tm_hour);
    buf[20] = dec2bcd(tm_n->tm_min);
    buf[21] = dec2bcd(tm_n->tm_sec);   

    //遥测站地址标识符
    buf[22] = SIGNAL_GUIDE_ST;
    buf[23] = SIGNAL_GUIDE_ST;

    //遥测站地址
    buf[24] = buf[3];
    buf[25] = buf[4];
    buf[26] = buf[5];
    buf[27] = buf[6];
    buf[28] = buf[7];

    //遥测站分类码
    buf[29] = Remote_ST[setting->st];

    //观测时间标识符
    buf[30] = SIGNAL_GUIDE_TT;
    buf[31] = SIGNAL_GUIDE_TT;

    if(!tt_set)
    {
        tt_set = 1;
        tm_n = localtime(&data->timestamp);
        //观测时间
        buf[32] = dec2bcd((tm_n->tm_year + 1900) % 100);
        buf[33] = dec2bcd(tm_n->tm_mon + 1);
        buf[34] = dec2bcd(tm_n->tm_mday);
        buf[35] = dec2bcd(tm_n->tm_hour);
        buf[36] = dec2bcd(tm_n->tm_min);
    }

    //时间步长
    buf[37] = SIGNAL_DRXNN;
    buf[38] = TIME_INTERVAL_D;

    //时间步长数据
    buf[39] = 0x00;
    buf[40] = dec2bcd(INTERVAL);
    buf[41] = 00;

    //数据定义
    buf[42] = data->sl_signal;

    index += 1;
    
    int each_data_len = 0;
    int nb_byte = 0, nb_dec = 0;
    int if_fisrt = 0;

    for (size_t i = 0; i < 12; i++)
    {
        each_data_len = serialed_sl651_datas(&buf[42 + index + 1], data->sl_signal, &data->his_data[i], &nb_byte, &nb_dec);
        if(if_fisrt == 0)
        {
            if_fisrt = 1;
            buf[42 + index] = nb_byte << 3 | nb_dec;
            index += 1;
        }
        index += each_data_len;
    }

    index += 1;
    //电压
    buf[42 + index] = SIGNAL_VT;
    index += 1;
    // buf[42 + index] = VT_D;
    // index += 1;
    //电压数据
    each_data_len = serialed_sl651_datas(&buf[42 + index + 1], SIGNAL_VT, &Bat_Cap, &nb_byte, &nb_dec);
    buf[42 + index] = nb_byte << 3 | nb_dec;
    index += each_data_len + 1;

    //上下行标识及正文长度
    unsigned short signal_up_len = 0;
    signal_up_len |= (index + 42 - 14);
    buf[11] = (signal_up_len >> 8) & 0xff;
    buf[12] = signal_up_len & 0xff;

    return seriled_sl651_end(buf, 42 + index);
}

//sl651定时上报数据帧
int serialed_sl651_timer_upload_data_frame(unsigned char* buf, sl651_setting_t* setting, struct data_info* datas)
{
    int index = 0;
    int tt_set = 0;
    int data_len = 0;

    seriled_sl651_head(buf, setting, REG_UPLOAD_CODE_TYPE);

    serial_num += 1;
    serial_num = serial_num > 65535 ? 0 : serial_num;

    //流水号
    buf[14] = serial_num / 256;
    buf[15] = serial_num % 256;

    time_t now;
    now = time(&now);

    struct tm* tm_n;
	tm_n = localtime(&now);

    //时间戳
    buf[16] = dec2bcd((tm_n->tm_year + 1900) % 100);
    buf[17] = dec2bcd(tm_n->tm_mon + 1);
    buf[18] = dec2bcd(tm_n->tm_mday);
    buf[19] = dec2bcd(tm_n->tm_hour);
    buf[20] = dec2bcd(tm_n->tm_min);
    buf[21] = dec2bcd(tm_n->tm_sec);   

    //遥测站地址标识符
    buf[22] = SIGNAL_GUIDE_ST;
    buf[23] = SIGNAL_GUIDE_ST;

    //遥测站地址
    buf[24] = buf[3];
    buf[25] = buf[4];
    buf[26] = buf[5];
    buf[27] = buf[6];
    buf[28] = buf[7];

    //遥测站分类码
    buf[29] = Remote_ST[setting->st];

    //观测时间标识符
    buf[30] = SIGNAL_GUIDE_TT;
    buf[31] = SIGNAL_GUIDE_TT;

    int each_data_len = 0;
    int nb_byte = 0, nb_dec = 0;

    rt_slist_t *node_data;
    rt_slist_for_each(node_data, &(dev->dataslist))
    {
        struct data_info* data = rt_slist_entry(node_data, struct data_info, datalist);
        if(data != NULL)
        {
            if(data->sl_signal == SIGNAL_VT)
                continue;

            if(!tt_set)
            {
                tt_set = 1;
                tm_n = localtime(&data->timestamp);
                //观测时间
                buf[32] = dec2bcd((tm_n->tm_year + 1900) % 100);
                buf[33] = dec2bcd(tm_n->tm_mon + 1);
                buf[34] = dec2bcd(tm_n->tm_mday);
                buf[35] = dec2bcd(tm_n->tm_hour);
                buf[36] = dec2bcd(tm_n->tm_min);

                index += 1;
            }

            //数据定义
            buf[36 + index] = data->sl_signal;
            index += 1;
    
            each_data_len = serialed_sl651_datas(&buf[36 + index + 1], data->sl_signal, &data->val, &nb_byte, &nb_dec);
            buf[36 + index] = nb_byte << 3 | nb_dec;
            index += each_data_len;
        }
    }

    index += 1;

    //电压
    buf[36 + index] = SIGNAL_VT;
    index += 1;
    // buf[36 + index] = VT_D;
    // index += 1;
    //电压数据
    each_data_len += serialed_sl651_datas(&buf[36 + index + 1], SIGNAL_VT, &Bat_Cap, &nb_byte, &nb_dec);
    buf[36 + index] = nb_byte << 3 | nb_dec;
    index += each_data_len + 1;

    //上下行标识及正文长度
    unsigned short signal_up_len = 0;
    signal_up_len |= (index + 36 - 14);
    buf[11] = (signal_up_len >> 8) & 0xff;
    buf[12] = signal_up_len & 0xff;

    return seriled_sl651_end(buf, 36 + index);
}

//sl651加报上报数据帧
int serialed_sl651_triggle_frame(unsigned char* buf, sl651_setting_t* setting, struct data_info* data)
{
    int index = 0;
    int tt_set = 0;
    int data_len = 0;

    seriled_sl651_head(buf, setting, TRIGGLE_UPLOAD_CODE_TYPE);


    serial_num += 1;
    serial_num = serial_num > 65535 ? 0 : serial_num;

    //流水号
    buf[14] = serial_num / 256;
    buf[15] = serial_num % 256;

    time_t now;
    now = time(&now);

    struct tm* tm_n;
	tm_n = localtime(&now);

    //时间戳
    buf[16] = dec2bcd((tm_n->tm_year + 1900) % 100);
    buf[17] = dec2bcd(tm_n->tm_mon + 1);
    buf[18] = dec2bcd(tm_n->tm_mday);
    buf[19] = dec2bcd(tm_n->tm_hour);
    buf[20] = dec2bcd(tm_n->tm_min);
    buf[21] = dec2bcd(tm_n->tm_sec);   

    //遥测站地址标识符
    buf[22] = SIGNAL_GUIDE_ST;
    buf[23] = SIGNAL_GUIDE_ST;

    //遥测站地址
    buf[24] = buf[3];
    buf[25] = buf[4];
    buf[26] = buf[5];
    buf[27] = buf[6];
    buf[28] = buf[7];

    //遥测站分类码
    buf[29] = Remote_ST[setting->st];

    //观测时间标识符
    buf[30] = SIGNAL_GUIDE_TT;
    buf[31] = SIGNAL_GUIDE_TT;

    if(!tt_set)
    {
        tt_set = 1;
        tm_n = localtime(&data->timestamp);
        //观测时间
        buf[32] = dec2bcd((tm_n->tm_year + 1900) % 100);
        buf[33] = dec2bcd(tm_n->tm_mon + 1);
        buf[34] = dec2bcd(tm_n->tm_mday);
        buf[35] = dec2bcd(tm_n->tm_hour);
        buf[36] = dec2bcd(tm_n->tm_min);

        index += 1;
    }

    // //时间步长
    // buf[37] = SIGNAL_DRXNN;
    // buf[38] = TIME_INTERVAL_D;

    // //时间步长数据
    // buf[39] = 0x00;
    // buf[40] = dec2bcd(INTERVAL);
    // buf[41] = 00;

    int each_data_len = 0;
    int nb_byte = 0, nb_dec = 0;

    //触发数据
    each_data_len = serialed_sl651_datas(&buf[36 + index + 1], data->sl_signal, &data->val, &nb_byte, &nb_dec);
    buf[36 + index] = nb_byte << 3 | nb_dec;
    index += each_data_len + 1;

    //电压
    buf[36 + index] = SIGNAL_VT;
    index += 1;
    // buf[36 + index] = VT_D;
    // index += 1;
    //电压数据
    each_data_len = serialed_sl651_datas(&buf[36 + index + 1], SIGNAL_VT, &Bat_Cap, &nb_byte, &nb_dec);
    buf[36 + index] = nb_byte << 3 | nb_dec;
    index += each_data_len + 1;

    
    //上下行标识及正文长度
    unsigned short signal_up_len = 0;
    signal_up_len |= (index + 36 - 14);
    buf[11] = (signal_up_len >> 8) & 0xff;
    buf[12] = signal_up_len & 0xff;

    return seriled_sl651_end(buf, 36 + index);
}

//sl651小时报数据帧
int serialed_sl651_hour_frame(unsigned char* buf, sl651_setting_t* setting, struct data_info* data)
{
    int index = 0;
    int tt_set = 0;
    int data_len = 0;

    seriled_sl651_head(buf, setting, HOUR_UPLOAD_CODE_TYPE);

    serial_num += 1;
    serial_num = serial_num > 65535 ? 0 : serial_num;

    //流水号
    buf[14] = serial_num / 256;
    buf[15] = serial_num % 256;

    time_t now;
    now = time(&now);

    struct tm* tm_n;
	tm_n = localtime(&now);

    //时间戳
    buf[16] = dec2bcd((tm_n->tm_year + 1900) % 100);
    buf[17] = dec2bcd(tm_n->tm_mon + 1);
    buf[18] = dec2bcd(tm_n->tm_mday);
    buf[19] = dec2bcd(tm_n->tm_hour);
    buf[20] = dec2bcd(tm_n->tm_min);
    buf[21] = dec2bcd(tm_n->tm_sec);   

    //遥测站地址标识符
    buf[22] = SIGNAL_GUIDE_ST;
    buf[23] = SIGNAL_GUIDE_ST;

    //遥测站地址
    buf[24] = buf[3];
    buf[25] = buf[4];
    buf[26] = buf[5];
    buf[27] = buf[6];
    buf[28] = buf[7];

    //遥测站分类码
    buf[29] = Remote_ST[setting->st];

    //观测时间标识符
    buf[30] = SIGNAL_GUIDE_TT;
    buf[31] = SIGNAL_GUIDE_TT;

    int each_data_len = 0;
    int nb_byte = 0, nb_dec = 0;

    rt_slist_t *node_data;
    rt_slist_for_each(node_data, &(dev->dataslist))
    {
        struct data_info* data = rt_slist_entry(node_data, struct data_info, datalist);
        if(data != NULL)
        {

            if(data->sl_signal == SIGNAL_VT)
                continue;

            if(!tt_set)
            {
                tt_set = 1;
                tm_n = localtime(&data->timestamp);
                //观测时间
                buf[32] = dec2bcd((tm_n->tm_year + 1900) % 100);
                buf[33] = dec2bcd(tm_n->tm_mon + 1);
                buf[34] = dec2bcd(tm_n->tm_mday);
                buf[35] = dec2bcd(tm_n->tm_hour);
                buf[36] = dec2bcd(tm_n->tm_min);

                index += 1;
            }

            //数据定义
            buf[36 + index] = data->sl_signal;
            index += 1;

            for (size_t i = 0; i < 12; i++)
            {
                each_data_len = serialed_sl651_datas(&buf[36 + index + 1], data->sl_signal, data->his_data, &nb_byte, &nb_dec);
                buf[36 + index] = nb_byte << 3 | nb_dec;
                index += each_data_len;
            }
        }
    }

    index += 1;
    //电压
    buf[36 + index] = SIGNAL_VT;
    index += 1;
    // buf[36 + index] = VT_D;
    // index += 1;
    //电压数据
    each_data_len = serialed_sl651_datas(&buf[36 + index + 1], SIGNAL_VT, &Bat_Cap, &nb_byte, &nb_dec);
    buf[36 + index] = nb_byte << 3 | nb_dec;
    index += each_data_len + 1;

    //上下行标识及正文长度
    unsigned short signal_up_len = 0;
    signal_up_len |= (index + 36 - 14);
    buf[11] = (signal_up_len >> 8) & 0xff;
    buf[12] = signal_up_len & 0xff;

    return seriled_sl651_end(buf, 36 + index);
}


//sl651实时数据报数据帧
int serialed_realtime_datas_frame(unsigned char* buf, sl651_setting_t* setting, struct data_info* data)
{
    int index = 0;
    int tt_set = 0;
    int data_len = 0;

    seriled_sl651_head(buf, setting, REALTIME_DATA_CODE_TYPE);

    serial_num += 1;
    serial_num = serial_num > 65535 ? 0 : serial_num;

    //流水号
    buf[14] = serial_num / 256;
    buf[15] = serial_num % 256;

    time_t now;
    now = time(&now);

    struct tm* tm_n;
	tm_n = localtime(&now);

    //时间戳
    buf[16] = dec2bcd((tm_n->tm_year + 1900) % 100);
    buf[17] = dec2bcd(tm_n->tm_mon + 1);
    buf[18] = dec2bcd(tm_n->tm_mday);
    buf[19] = dec2bcd(tm_n->tm_hour);
    buf[20] = dec2bcd(tm_n->tm_min);
    buf[21] = dec2bcd(tm_n->tm_sec);   

    //遥测站地址标识符
    buf[22] = SIGNAL_GUIDE_ST;
    buf[23] = SIGNAL_GUIDE_ST;

    //遥测站地址
    buf[24] = buf[3];
    buf[25] = buf[4];
    buf[26] = buf[5];
    buf[27] = buf[6];
    buf[28] = buf[7];

    //遥测站分类码
    buf[29] = Remote_ST[setting->st];

    //观测时间标识符
    buf[30] = SIGNAL_GUIDE_TT;
    buf[31] = SIGNAL_GUIDE_TT;

    int each_data_len = 0;
    int nb_byte = 0, nb_dec = 0;

    rt_slist_t *node_data;
    rt_slist_for_each(node_data, &(dev->dataslist))
    {
        struct data_info* data = rt_slist_entry(node_data, struct data_info, datalist);
        if(data != NULL)
        {
            if(data->sl_signal == SIGNAL_VT)
                continue;

            if(!tt_set)
            {
                tt_set = 1;
                tm_n = localtime(&data->timestamp);
                //观测时间
                buf[32] = dec2bcd((tm_n->tm_year + 1900) % 100);
                buf[33] = dec2bcd(tm_n->tm_mon + 1);
                buf[34] = dec2bcd(tm_n->tm_mday);
                buf[35] = dec2bcd(tm_n->tm_hour);
                buf[36] = dec2bcd(tm_n->tm_min);

                index += 1;
            }

            //数据定义
            buf[36 + index] = data->sl_signal;
            index += 1;

            each_data_len = serialed_sl651_datas(&buf[36 + index + 1], data->sl_signal, &data->val, &nb_byte, &nb_dec);
            buf[36 + index] = nb_byte << 3 | nb_dec;
            index += each_data_len;
        }
    }

    index += 1;

    //电压
    buf[36 + index] = SIGNAL_VT;
    index += 1;
    // buf[36 + index] = VT_D;
    // index += 1;
    //电压数据
    each_data_len = serialed_sl651_datas(&buf[36 + index + 1], SIGNAL_VT, &Bat_Cap, &nb_byte, &nb_dec);
    buf[36 + index] = nb_byte << 3 | nb_dec;
    index += each_data_len + 1;

    //上下行标识及正文长度
    unsigned short signal_up_len = 0;
    signal_up_len |= (index + 36 - 14);
    buf[11] = (signal_up_len >> 8) & 0xff;
    buf[12] = signal_up_len & 0xff;

    return seriled_sl651_end(buf, 36 + index);
}
