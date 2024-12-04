/**
 * @file shell_cfg.h
 * @author Letter (NevermindZZT@gmail.com)
 * @brief shell config
 * @version 0.1
 * @date 2019-04-11
 *
 * @copyright (c) 2019 Letter
 *
 */

#ifndef __SHELL_CFG_H__
#define __SHELL_CFG_H__

#include "wmsdk_config.h"
#include "wm_osal.h"

/**
 * @brief �Ƿ�ʹ��Ĭ��shell����
 *        ʹ��֮�����ʹ��`shellTask()`����shell���񣬻���ʹ��`shellTask()`������ѯ
 */
#define     SHELL_USING_TASK            0

/**
 * @brief �Ƿ�ʹ��Ĭ��shell����whileѭ����ʹ�ܺ�`SHELL_USING_TASK`��˺�������
 *        ʹ�ܴ˺꣬��`shellTask()`������һֱѭ����ȡ���룬һ��ʹ�ò���ϵͳ����shell
 *        ����ʱʹ�ܴ˺꣬�رմ˺������£�һ���������޲���ϵͳ������ѭ���е���`shellTask()`
 */
#define     SHELL_TASK_WHILE            1

/**
 * @brief �Ƿ�ʹ���������ʽ
 *        ʹ�ܴ˺�󣬿���ʹ��`SHELL_EXPORT_CMD()`����`SHELL_EXPORT_CMD_EX()`
 *        ����shell����رմ˺������£���Ҫʹ��������ķ�ʽ
 */
#define     SHELL_USING_CMD_EXPORT      1

/**
 * @brief �Ƿ�ʹ�ñ�������
 *
 */
#define     SHELL_USING_VAR             0

/**
 * @brief �Ƿ���ʾ������ú�������ֵ
 *        ʹ�ܴ˺꣬��ÿ�ε���shell����֮��������κ�ʮ�����Ƶķ�ʽ��ӡ�����ķ���ֵ
 */
#define     SHELL_DISPLAY_RETURN        0

/**
 * @brief �Ƿ�ʹ��shell�����Զ�����
 *        ʹ�ܴ˺���֧�ֳ���C������ʽ�����shell���Զ�ת������
 *        �رմ˺���֧��main������ʽ�������Ҫ�����ں����д�������
 */
#define     SHELL_AUTO_PRASE            0

/**
 * @brief �Ƿ�ʹ��shell������
 *        ʹ�ܴ˺���֧������ĳ�������Ϣ
 */
#define     SHELL_LONG_HELP             1

/**
 * @brief shell������󳤶�
 *        �����п����������ַ�����
 */
#define     SHELL_COMMAND_MAX_LENGTH    (CONFIG_CLI_LINE_SIZE + 1)

/**
 * @brief shell��������������
 *        �������������ڣ�����8����������ʹ���˲����Զ�ת��������£���Ҫ�޸�Դ��
 */
#define     SHELL_PARAMETER_MAX_NUMBER  (CONFIG_CLI_CMD_PARAS_NUM + 1)

/**
 * @brief ��ʷ�����¼����
 */
#define     SHELL_HISTORY_MAX_NUMBER    CONFIG_CLI_CMD_HISTORY_NUM

/**
 * @brief ˫�����(ms)
 *        ʹ�ܺ�`SHELL_LONG_HELP`��˺���Ч������˫��tab��ȫhelp��ʱ����
 */
#define     SHELL_DOUBLE_CLICK_TIME     200

/**
 * @brief ���������shell����
 */
#define     SHELL_MAX_NUMBER            1//5

/**
 * @brief shell��ʽ������Ļ����С
 *        Ϊ0ʱ��ʹ��shell��ʽ�����
 */
#define     SHELL_PRINT_BUFFER          0//128

/**
 * @brief ��ȡϵͳʱ��(ms)
 *        ����˺�Ϊ��ȡϵͳTick����`HAL_GetTick()`
 * @note �˺겻����ʱ�޷�ʹ��˫��tab��ȫ����help���޷�ʹ��shell��ʱ����
 */
#define     SHELL_GET_TICK()            wm_os_internal_get_time_ms()

/**
 * @brief shellĬ����ʾ��
 */
//#define     SHELL_DEFAULT_COMMAND       "\r\n"CONFIG_PROJECT_NAME": "
#define     SHELL_DEFAULT_COMMAND       "\r\n"CONFIG_CHIP_NAME": "

/**
 * @brief �Ƿ�ʹ�����빦��
 */
#define     SHELL_USING_AUTH             0

/**
 * @brief shell�û�����
 */
#define     SHELL_USER_PASSWORD         "letter"

/**
 * @brief shell�Զ�������ʱ
 *        ʹ��`SHELL_USING_AUTH`���������Ч����ʱ����Զ���������shell
 *        ����Ϊ0ʱ�ر��Զ��������ܣ�ʱ�䵥λΪ`SHELL_GET_TICK()`��λ
 * @note ʹ�ó�ʱ�������뱣֤`SHELL_GET_TICK()`��Ч
 */
#define     SHELL_LOCK_TIMEOUT          5 * 60 * 1000

#define     SHELL_QUOTE_KEEP_RAW        1

#define     SHELL_FILTER_CTRL_KEYS      1

#endif
