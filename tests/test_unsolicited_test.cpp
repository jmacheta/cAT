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
static char read_results[256];
static char ack_results[256];

static char const *input_text;
static size_t input_index;

static int var_x, var_u1, var_u2;
static cat_object at;

static cat_return_state cmd_test(const cat_command *cmd, char *data, size_t *data_size, size_t max_data_size);

static cat_variable u_vars[] = { { .name = "U1", .type = CAT_VAR_INT_DEC, .data = &var_u1, .data_size = sizeof(var_u1) },
                                 { .name = "U2", .type = CAT_VAR_INT_DEC, .data = &var_u2, .data_size = sizeof(var_u2) } };

static cat_command u_cmds[] = { {
                                        .name = "+U1CMD",
                                        .test = cmd_test,
                                        .var = &u_vars[0],
                                        .var_num = 1,
                                },
                                {
                                        .name = "+U2CMD",
                                        .test = cmd_test,
                                        .var = &u_vars[1],
                                        .var_num = 1,
                                } };

static cat_return_state cmd_test(const cat_command *cmd, char *, size_t *, size_t)
{
        cat_status s;

        strcat(read_results, " test:");
        strcat(read_results, cmd->name);

        if (strcmp(cmd->name, "+CMD") == 0) {
                s = cat_trigger_unsolicited_test(&at, &u_cmds[1]);
                assert(s == CAT_STATUS_OK);
        }

        return CAT_RETURN_STATE_DATA_OK;
}

static cat_variable vars[] = { { .name = "X", .type = CAT_VAR_INT_DEC, .data = &var_x, .data_size = sizeof(var_x) } };

static cat_command cmds[] = { {
        .name = "+CMD",
        .test = cmd_test,
        .var = vars,
        .var_num = sizeof(vars) / sizeof(vars[0]),
} };

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

static void prepare_input(const char *text)
{
        input_text = text;
        input_index = 0;

        var_x = 1;
        var_u1 = 2;
        var_u2 = 3;

        memset(ack_results, 0, sizeof(ack_results));
        memset(read_results, 0, sizeof(read_results));
}

static const char test_case_1[] = "\nAT+CMD=?\n";

TEST(cAT, unsilicited_test)
{
        cat_init(&at, &desc, &iface, NULL);

        prepare_input(test_case_1);

        EXPECT_EQ(cat_is_unsolicited_buffer_full(&at), CAT_STATUS_OK);
        EXPECT_EQ(cat_trigger_unsolicited_event(&at, &u_cmds[0], CAT_CMD_TYPE_TEST), CAT_STATUS_OK);

        EXPECT_EQ(cat_is_unsolicited_buffer_full(&at), CAT_STATUS_ERROR_BUFFER_FULL);
        EXPECT_EQ(cat_trigger_unsolicited_test(&at, &u_cmds[1]), CAT_STATUS_ERROR_BUFFER_FULL);

        while (cat_service(&at) != 0) {
        };

        EXPECT_STREQ(ack_results, "\n+U1CMD=<U1:INT32[RW]>\n\n+CMD=<X:INT32[RW]>\n\n+U2CMD=<U2:INT32[RW]>\n\nOK\n");
        EXPECT_STREQ(read_results, " test:+U1CMD test:+CMD test:+U2CMD");
}
