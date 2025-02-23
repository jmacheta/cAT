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
static char write_results[256];
static char ack_results[256];

static char var[8];
static size_t var_write_size[4];
static int var_write_size_index;

static char const *input_text;
static size_t input_index;

static cat_return_state cmd_write(const cat_command *, const char *data, size_t data_size, size_t )
{
        strcat(write_results, " CMD:");
        strncat(write_results, data, data_size);
        return CAT_RETURN_STATE_DATA_OK;
}

static int var_write(const cat_variable *, size_t write_size)
{
        var_write_size[var_write_size_index++] = write_size;
        return 0;
}

static cat_variable vars[] = { { .type = CAT_VAR_BUF_STRING, .data = var, .data_size = sizeof(var), .write = var_write } };

static cat_command cmds[] = { { .name = "+SET",
                                .write = cmd_write,

                                .var = vars,
                                .var_num = sizeof(vars) / sizeof(vars[0]) } };

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

        memset(var, 0, sizeof(var));
        memset(var_write_size, 0, sizeof(var_write_size));
        var_write_size_index = 0;

        memset(ack_results, 0, sizeof(ack_results));
        memset(write_results, 0, sizeof(write_results));
}

static const char test_case_1[] = "\nAT+SET=0\nAT+SET=\"\\\"abcd\\\"\"\nAT+SET=\"\"a\nAT+SET=\"1122334\"\nAT+SET=\"t\"\r\n";
static const char test_case_2[] = "\nAT+SET=\"12345678\"\nAT+SET=\"\"\nAT+SET=\"\\\\\\\\\"\nAT+SET=\"r1\\nr2\\n\"\n";

TEST(cAT, write_string_buffer)
{
        cat_object at;

        cat_init(&at, &desc, &iface, NULL);

        prepare_input(test_case_1);
        while (cat_service(&at) != 0) {
        };

        EXPECT_STREQ(ack_results, "\nERROR\n\nOK\n\nERROR\n\nOK\n\r\nOK\r\n");
        EXPECT_STREQ(write_results, " CMD:\"\\\"abcd\\\"\" CMD:\"1122334\" CMD:\"t\"");

        EXPECT_STREQ(var, "t");

        EXPECT_EQ(var_write_size[0], 6);
        EXPECT_EQ(var_write_size[1], 7);
        EXPECT_EQ(var_write_size[2], 1);
        EXPECT_EQ(var_write_size[3], 0);

        prepare_input(test_case_2);
        while (cat_service(&at) != 0) {
        };

        EXPECT_STREQ(ack_results, "\nERROR\n\nOK\n\nOK\n\nOK\n");
        EXPECT_STREQ(write_results, " CMD:\"\" CMD:\"\\\\\\\\\" CMD:\"r1\\nr2\\n\"");

        EXPECT_STREQ(var, "r1\nr2\n");

        EXPECT_EQ(var_write_size[0], 0);
        EXPECT_EQ(var_write_size[1], 2);
        EXPECT_EQ(var_write_size[2], 6);
        EXPECT_EQ(var_write_size[3], 0);
}
