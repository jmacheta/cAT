/*
MIT License

Copyright (c) 2019 Marcin Borowicz

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include <assert.h>

#include <cat/cat.h>
#include <gtest/gtest.h>
static char run_results[256];
static char ack_results[256];

static char const *input_text;
static size_t input_index;

static cat_return_state a_run(const cat_command *cmd)
{
        strcat(run_results, " A:");
        strcat(run_results, cmd->name);
        return CAT_RETURN_STATE_DATA_OK;
}

static cat_return_state ap_run(const cat_command *cmd)
{
        strcat(run_results, " AP:");
        strcat(run_results, cmd->name);
        return CAT_RETURN_STATE_DATA_OK;
}

static cat_return_state test_run(const cat_command *cmd)
{
        strcat(run_results, " +TEST:");
        strcat(run_results, cmd->name);
        return CAT_RETURN_STATE_DATA_OK;
}

static cat_command cmds[] = { { .name = "A", .run = a_run }, { .name = "AP", .run = ap_run }, { .name = "+TEST", .run = test_run } };

static uint8_t buf[128];

static cat_command_group cmd_group = {
        .cmd = cmds,
        .cmd_num = sizeof(cmds) / sizeof(cmds[0]),
};

static cat_command_group *cmd_desc[] = { &cmd_group };

static cat_descriptor desc = {
        .cmd_group = cmd_desc,
        .cmd_group_num = sizeof(cmd_desc) / sizeof(cmd_desc[0]),

        .buf = buf,
        .buf_size = sizeof(buf),
};

static int write_char(char ch)
{
        char str[2];
        str[0] = ch;
        str[1] = 0;
        strcat(ack_results, str);
        return 1;
}

static int read_char(char *ch)
{
        if (input_index >= strlen(input_text))
                return 0;

        *ch = input_text[input_index];
        input_index++;
        return 1;
}

static cat_io_interface iface = { .write = write_char, .read = read_char };

static int mutex_ret_lock;
static int mutex_ret_unlock;

static int mutex_lock(void)
{
        return mutex_ret_lock;
}

static int mutex_unlock(void)
{
        return mutex_ret_unlock;
}

static cat_mutex_interface mutex = { .lock = mutex_lock, .unlock = mutex_unlock };

static void prepare_input(const char *text)
{
        input_text = text;
        input_index = 0;

        mutex_ret_lock = 0;
        mutex_ret_unlock = 0;

        memset(run_results, 0, sizeof(run_results));
        memset(ack_results, 0, sizeof(ack_results));
}

static const char test_case_1[] = "\nAT\nAT+test\n";

TEST(cAT, mutex)
{
        cat_object at;

        cat_init(&at, &desc, &iface, &mutex);

        prepare_input(test_case_1);
        while (cat_service(&at) != 0) {
        };

        EXPECT_STREQ(ack_results, "\nOK\n\nOK\n");
        EXPECT_STREQ(run_results, " +TEST:+TEST");

        mutex_ret_lock = 1;
        mutex_ret_unlock = 0;
        EXPECT_EQ(cat_service(&at), CAT_STATUS_ERROR_MUTEX_LOCK);

        mutex_ret_lock = 0;
        mutex_ret_unlock = 1;
        EXPECT_EQ(cat_service(&at), CAT_STATUS_ERROR_MUTEX_UNLOCK);

        mutex_ret_lock = 1;
        mutex_ret_unlock = 0;
        EXPECT_EQ(cat_is_busy(&at), CAT_STATUS_ERROR_MUTEX_LOCK);

        mutex_ret_lock = 0;
        mutex_ret_unlock = 1;
        EXPECT_EQ(cat_is_busy(&at), CAT_STATUS_ERROR_MUTEX_UNLOCK);

        mutex_ret_lock = 0;
        mutex_ret_unlock = 0;
        EXPECT_EQ(cat_service(&at), CAT_STATUS_OK);
        EXPECT_EQ(cat_is_busy(&at), CAT_STATUS_OK);
}
