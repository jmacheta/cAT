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

static uint8_t var1, var1b;
static uint16_t var2, var2b;
static uint32_t var3, var3b;

static char const *input_text;
static size_t input_index;

static cat_return_state cmd_write(const cat_command *, const char *data, size_t data_size, size_t)
{
        strcat(write_results, " CMD:");
        strncat(write_results, data, data_size);
        return CAT_RETURN_STATE_OK;
}

static int var1_write(const cat_variable *var, size_t write_size)
{
        assert(write_size == 1);
        var1b = *(uint8_t *)(var->data);
        return 0;
}

static int var2_write(const cat_variable *var, size_t write_size)
{
        assert(write_size == 2);
        var2b = *(uint16_t *)(var->data);
        return 0;
}

static int var3_write(const cat_variable *var, size_t write_size)
{
        assert(write_size == 4);
        var3b = *(uint32_t *)(var->data);
        return 0;
}

static cat_variable vars[] = { { .type = CAT_VAR_NUM_HEX, .data = &var1, .data_size = sizeof(var1), .write = var1_write },
                               { .type = CAT_VAR_NUM_HEX, .data = &var2, .data_size = sizeof(var2), .write = var2_write },
                               { .type = CAT_VAR_NUM_HEX, .data = &var3, .data_size = sizeof(var3), .write = var3_write } };

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

        var1 = 1;
        var2 = 2;
        var3 = 3;
        var1b = 10;
        var2b = 20;
        var3b = 30;

        memset(ack_results, 0, sizeof(ack_results));
        memset(write_results, 0, sizeof(write_results));
}

static const char test_case_1[] = "\nAT+SET=0\nAT+SET=0x0\nAT+SET=0x01\nAT+SET=0x0ff\nAT+SET=0x100\n";
static const char test_case_2[] = "\nAT+SET=0x,0x00\nAT+SET=0x1,0x00\nAT+SET=0x2,0xFFf\nAT+SET=0x3,0xFFFF\nAT+SET=0x4,0xFFFFF\n";
static const char test_case_3[] =
        "\nAT+SET=0x0,0x0,0\nAT+SET=0x0,0x0,0x0000000000000\nAT+SET=0x0,0x0,0x1\nAT+SET=0x0,0x0,0xffffFFFF\nAT+SET=0x10,0x20,0x100000000\n";

TEST(cAT, write_hex_range)
{
        cat_object at;

        cat_init(&at, &desc, &iface, NULL);

        prepare_input(test_case_1);
        while (cat_service(&at) != 0) {
        };

        EXPECT_STREQ(ack_results, "\nERROR\n\nOK\n\nOK\n\nOK\n\nERROR\n");
        EXPECT_STREQ(write_results, " CMD:0x0 CMD:0x01 CMD:0x0ff");

        EXPECT_EQ(var1, 255);
        EXPECT_EQ(var1b, var1);
        EXPECT_EQ(var2, 2);
        EXPECT_EQ(var2b, 20);
        EXPECT_EQ(var3, 3);
        EXPECT_EQ(var3b, 30);

        prepare_input(test_case_2);
        while (cat_service(&at) != 0) {
        };

        EXPECT_STREQ(ack_results, "\nERROR\n\nOK\n\nOK\n\nOK\n\nERROR\n");
        EXPECT_STREQ(write_results, " CMD:0x1,0x00 CMD:0x2,0xFFf CMD:0x3,0xFFFF");

        EXPECT_EQ(var1, 4);
        EXPECT_EQ(var1b, var1);
        EXPECT_EQ(var2, 0xFFFF);
        EXPECT_EQ(var2b, var2);
        EXPECT_EQ(var3, 3);
        EXPECT_EQ(var3b, 30);

        prepare_input(test_case_3);
        while (cat_service(&at) != 0) {
        };

        EXPECT_STREQ(ack_results, "\nERROR\n\nOK\n\nOK\n\nOK\n\nERROR\n");
        EXPECT_STREQ(write_results, " CMD:0x0,0x0,0x0000000000000 CMD:0x0,0x0,0x1 CMD:0x0,0x0,0xffffFFFF");

        EXPECT_EQ(var1, 0x10);
        EXPECT_EQ(var1b, var1);
        EXPECT_EQ(var2, 0x20);
        EXPECT_EQ(var2b, var2);
        EXPECT_EQ(var3, 0xFFFFFFFF);
        EXPECT_EQ(var3b, var3);
}
