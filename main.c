#include <stdlib.h>
#include "sl651_hex_data.h"

sl651_setting_t setting_sl;

void init_datas(void)
{
    dev = (struct dev_info*)malloc(sizeof(struct dev_info));
    char tmp[16];
    dev->name = strdup("test_dev");
    dev->station = 1;
    rt_slist_init(&dev->dataslist);

    setting_sl.master_st = 0x02;
    setting_sl.passwd = 0xFFFF;
    setting_sl.st = RIVER_ST;

    sscanf("2208090003", "%lx", &setting_sl.remote_st);
    // setting_sl.remote_st = 2208090003;

    printf("master:%02X  passwd:%04X st:%02X  remote_st:%02X%08X\r\n", setting_sl.master_st, setting_sl.passwd, setting_sl.st, 
    (int)((setting_sl.remote_st)>>32) & 0xFF, (int)(setting_sl.remote_st) & 0xFFFFFFFF);

    for (size_t i = 0; i < 5; i++)
    {
        struct data_info* data  = (struct data_info*) malloc(sizeof(struct data_info));
        memset(tmp, 0, sizeof(tmp));
        sprintf(tmp, "data_%ld", i);
        data->tag = strdup(tmp);

        data->val = (i + 1) * 10;

        data->ratio = 1;
        data->timestamp = time(NULL);
        switch (i)
        {
        case 0:
            data->sl_signal = SIGNAL_Q;
            break;

        case 1:
            data->sl_signal = SIGNAL_Q1;
            break;

        case 2:
            data->sl_signal = SIGNAL_Q2;
            break;

        case 3:
            data->sl_signal = SIGNAL_Q3;
            break;

        case 4:
            data->sl_signal = SIGNAL_Q4;
            break;
        default:
            break;
        }
        for (size_t j = 0; j < 12; j++)
        {
            data->his_data[j] = (j+1) * 10;
        }

        printf("data:%s signal:%02x ts:%ld val:%.3lf   his_val:%.3lf %.3lf %.3lf %.3lf %.3lf %.3lf %.3lf %.3lf %.3lf %.3lf %.3lf %.3lf\r\n",
            data->tag, data->sl_signal, data->timestamp, data->val, data->his_data[0], data->his_data[1],  data->his_data[2], data->his_data[3],
            data->his_data[4], data->his_data[5],  data->his_data[6], data->his_data[7],data->his_data[8], data->his_data[9],  data->his_data[10], data->his_data[11]);
        
        rt_slist_init(&data->datalist);
        rt_slist_append(&dev->dataslist, &data->datalist);
    }

    Bat_Cap = 219.98;
    
}

int main(int argc, char** argvs)
{
    int len = 0;
    unsigned char payload[1024];
    memset(payload, 0, 1024);

    if((argc != 2) || (atoi(argvs[1]) < 0) || (atoi(argvs[1])>6))
    {
        printf("0:keepalive frame 1:test frame 2:equ interval frame 3:timer upload frame 4:triggle frame 5:hour frame 6:realtime datas frame \r\n");
        exit(0);
        return -1;
    }

    init_datas();

    switch (atoi(argvs[1]))
    {
        case 0: //链路报
        {
            len = serialed_sl651_keepalive_frame(payload, &setting_sl);
            printf("keep_alive frame:\r\n");
            break;
        }

        case 1: //测试报
        {
            len = serialed_sl651_test_frame(payload, &setting_sl, RT_NULL);
            printf("test frame:\r\n");
            break;
        }

        case 2: //均匀时段报
        {

            rt_slist_t *node_data;
            rt_slist_for_each(node_data, &(dev->dataslist))
            {
                struct data_info* data = rt_slist_entry(node_data, struct data_info, datalist);
                if(data != NULL)
                {
                    len = serialed_sl651_equ_interval_frame(payload, &setting_sl, data);
                    break;
                }
            }
            printf("equ interval frame:\r\n");
            break;
        }

        case 3: //定时报
        {
            len = serialed_sl651_timer_upload_data_frame(payload, &setting_sl, RT_NULL);
            printf("timer up frame:\r\n");
            break;
        }

        case 4: //加报
        {
            rt_slist_t *node_data;
            rt_slist_for_each(node_data, &(dev->dataslist))
            {
                struct data_info* data = rt_slist_entry(node_data, struct data_info, datalist);
                if(data != NULL)
                {
                    len = serialed_sl651_triggle_frame(payload, &setting_sl, data);
                    break;
                }
            }
            printf("triggle frame:\r\n");
            break;
        }

        case 5: //小时报
        {
            len = serialed_sl651_hour_frame(payload, &setting_sl, RT_NULL);
            printf("hour frame:\r\n");
            break;
        }

        case 6: //实时报
        {
            len = serialed_realtime_datas_frame(payload, &setting_sl, RT_NULL);
            printf("realtime frame:\r\n");
            break;
        }
        
        default:
        {
            break;
        }
    }

        printf("frame len : %d\r\n", len);
        for (size_t i = 0; i < len; i++)
        {
            printf("%02x ", payload[i]);
        }
        printf("\r\n");
        return 0;
}


