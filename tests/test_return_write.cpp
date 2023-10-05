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

static int var_x;
static cat_object at;

static cat_return_state ret;
static bool ret_error;

static cat_return_state cmd_read(const cat_command *cmd, char *data, size_t *data_size, size_t max_data_size)
{
        (void)max_data_size; // Unused
        strcat(cmd_results, " read:");
        strcat(cmd_results, cmd->name);

        var_x++;
        if (var_x > 5) {
                ret = (ret_error == false) ? CAT_RETURN_STATE_HOLD_EXIT_OK : CAT_RETURN_STATE_HOLD_EXIT_ERROR;
        } else {
                if (var_x == 5) {
                        strcpy(data, "test");
                        *data_size = strlen(data);
                }
                ret = CAT_RETURN_STATE_DATA_NEXT;
        }

        return ret;
}

static cat_return_state cmd_write(const cat_command *cmd, const char *, size_t, size_t)
{
        strcat(cmd_results, " write:");
        strcat(cmd_results, cmd->name);

        if ((ret == CAT_RETURN_STATE_DATA_NEXT) || (ret == CAT_RETURN_STATE_NEXT)) {
                var_x++;
                if (var_x > 3)
                        ret = CAT_RETURN_STATE_DATA_OK;
        } else if (ret == CAT_RETURN_STATE_HOLD) {
                cat_trigger_unsolicited_read(&at, cmd);
        }

        return ret;
}

static cat_variable vars[] = { { .name = "X", .type = CAT_VAR_INT_DEC, .data = &var_x, .data_size = sizeof(var_x) } };

static cat_command cmds[] = { {
        .name = "+CMD",
        .write = cmd_write,
        .read = cmd_read,
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

        memset(ack_results, 0, sizeof(ack_results));
        memset(cmd_results, 0, sizeof(cmd_results));
}

static const char test_case_1[] = "\nAT+CMD=2\n";

TEST(cAT, return_write)
{
        cat_init(&at, &desc, &iface, NULL);

        ret = CAT_RETURN_STATE_ERROR;
        prepare_input(test_case_1);
        while (cat_service(&at) != 0) {
        };

        EXPECT_STREQ(ack_results, "\nERROR\n");
        EXPECT_STREQ(cmd_results, " write:+CMD");
        EXPECT_EQ(var_x, 2);

        ret = CAT_RETURN_STATE_DATA_OK;
        prepare_input(test_case_1);
        while (cat_service(&at) != 0) {
        };

        EXPECT_STREQ(ack_results, "\nOK\n");
        EXPECT_STREQ(cmd_results, " write:+CMD");
        EXPECT_EQ(var_x, 2);

        ret = CAT_RETURN_STATE_DATA_NEXT;
        prepare_input(test_case_1);
        while (cat_service(&at) != 0) {
        };

        EXPECT_STREQ(ack_results, "\nOK\n");
        EXPECT_STREQ(cmd_results, " write:+CMD write:+CMD");
        EXPECT_EQ(var_x, 4);

        ret = CAT_RETURN_STATE_NEXT;
        prepare_input(test_case_1);
        while (cat_service(&at) != 0) {
        };

        EXPECT_STREQ(ack_results, "\nOK\n");
        EXPECT_STREQ(cmd_results, " write:+CMD write:+CMD");
        EXPECT_EQ(var_x, 4);

        ret = CAT_RETURN_STATE_OK;
        prepare_input(test_case_1);
        while (cat_service(&at) != 0) {
        };

        EXPECT_STREQ(ack_results, "\nOK\n");
        EXPECT_STREQ(cmd_results, " write:+CMD");
        EXPECT_EQ(var_x, 2);

        ret_error = false;
        ret = CAT_RETURN_STATE_HOLD;
        prepare_input(test_case_1);
        while (cat_service(&at) != 0) {
        };

        EXPECT_STREQ(ack_results, "\n+CMD=2\n\n+CMD=3\n\ntest\n\nOK\n");
        EXPECT_STREQ(cmd_results, " write:+CMD read:+CMD read:+CMD read:+CMD read:+CMD");
        EXPECT_EQ(var_x, 6);

        ret_error = true;
        ret = CAT_RETURN_STATE_HOLD;
        prepare_input(test_case_1);
        while (cat_service(&at) != 0) {
        };

        EXPECT_STREQ(ack_results, "\n+CMD=2\n\n+CMD=3\n\ntest\n\nERROR\n");
        EXPECT_STREQ(cmd_results, " write:+CMD read:+CMD read:+CMD read:+CMD read:+CMD");
        EXPECT_EQ(var_x, 6);

        ret = CAT_RETURN_STATE_HOLD_EXIT_ERROR;
        prepare_input(test_case_1);
        while (cat_service(&at) != 0) {
        };

        EXPECT_STREQ(ack_results, "\nERROR\n");
        EXPECT_STREQ(cmd_results, " write:+CMD");
        EXPECT_EQ(var_x, 2);

        ret = CAT_RETURN_STATE_HOLD_EXIT_OK;
        prepare_input(test_case_1);
        while (cat_service(&at) != 0) {
        };

        EXPECT_STREQ(ack_results, "\nERROR\n");
        EXPECT_STREQ(cmd_results, " write:+CMD");
        EXPECT_EQ(var_x, 2);
}
