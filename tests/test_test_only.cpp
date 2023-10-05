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
static char cmd_results[256];
static char ack_results[256];

static char const *input_text;
static size_t input_index;

static cat_return_state ap_test(const cat_command *cmd, char *data, size_t *data_size, size_t max_data_size)
{
        (void)max_data_size; // Unused
        strcat(cmd_results, " test:");
        strcat(cmd_results, cmd->name);

        strcpy(data, "ap_test");
        *data_size = strlen(data);

        return CAT_RETURN_STATE_DATA_OK;
}

static cat_return_state ap_run(const cat_command *cmd)
{
        strcat(cmd_results, " run:");
        strcat(cmd_results, cmd->name);

        return CAT_RETURN_STATE_DATA_OK;
}

static cat_return_state ap_read(const cat_command *cmd, char *data, size_t *data_size, size_t max_data_size)
{
        (void)data_size; // Unused
        (void)max_data_size; // Unused

        strcat(cmd_results, " read:");
        strcat(cmd_results, cmd->name);

        strcpy(data, "ap_read");
        *data_size = strlen(data);

        return CAT_RETURN_STATE_DATA_OK;
}

static cat_return_state ap_write(const cat_command *cmd, const char *data, size_t data_size, size_t args_num)
{
        (void)data_size; // Unused

        (void)args_num; // Unused

        strcat(cmd_results, " write:");
        strcat(cmd_results, cmd->name);

        assert(strcmp(data, "1") == 0);

        return CAT_RETURN_STATE_DATA_OK;
}

static cat_command cmds[] = {
        { .name = "AP1", .write = ap_write, .read = ap_read, .run = ap_run, .test = ap_test, .only_test = true },
        { .name = "AP2", .write = ap_write, .read = ap_read, .run = ap_run, .test = ap_test, .only_test = false },
};

static uint8_t buf[128];

static cat_command_group cmd_group = {
        .cmd = cmds,
        .cmd_num = sizeof(cmds) / sizeof(cmds[0]),
};

static cat_command_group *cmd_desc[] = { &cmd_group };

static cat_descriptor desc = { .cmd_group = cmd_desc,
                               .cmd_group_num = sizeof(cmd_desc) / sizeof(cmd_desc[0]),

                               .buf = buf,
                               .buf_size = sizeof(buf) };

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

static void prepare_input(const char *text)
{
        input_text = text;
        input_index = 0;

        memset(ack_results, 0, sizeof(ack_results));
        memset(cmd_results, 0, sizeof(cmd_results));
}

static const char test_case_1[] = "\nATAP1=?\n\nATAP1?\n\nATAP1=1\n\nATAP1\n";
static const char test_case_2[] = "\nATAP2=?\n\nATAP2?\n\nATAP2=1\n\nATAP2\n";

TEST(cAT, test_only)
{
        cat_object at;

        cat_init(&at, &desc, &iface, NULL);

        prepare_input(test_case_1);
        while (cat_service(&at) != 0) {
        };

        EXPECT_STREQ(ack_results, "\nap_test\n\nOK\n\nERROR\n\nERROR\n\nERROR\n");
        EXPECT_STREQ(cmd_results, " test:AP1");

        prepare_input(test_case_2);
        while (cat_service(&at) != 0) {
        };

        EXPECT_STREQ(ack_results, "\nap_test\n\nOK\n\nap_read\n\nOK\n\nOK\n\nOK\n");
        EXPECT_STREQ(cmd_results, " test:AP2 read:AP2 write:AP2 run:AP2");
}
