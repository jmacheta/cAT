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
static char ack_results[256];

static int8_t var1;
static int8_t var2;
static int8_t var3;

static char const *input_text;
static size_t input_index;

static int var2_write_cntr;
static int var3_read_cntr;

static int8_t var_int8;
static int16_t var_int16;
static int32_t var_int32;
static uint8_t var_uint8;
static uint16_t var_uint16;
static uint32_t var_uint32;
static uint8_t var_hex8;
static uint16_t var_hex16;
static uint32_t var_hex32;
static uint8_t var_buf[4];
static char var_string[16];

static int var1_write(const cat_variable *, size_t)
{
        return 0;
}

static int var1_read(const cat_variable *)
{
        return 0;
}

static int var2_write(const cat_variable *, size_t)
{
        var2_write_cntr++;
        return 0;
}

static int var2_read(const cat_variable *)
{
        return 0;
}

static int var3_write(const cat_variable *, size_t)
{
        return 0;
}

static int var3_read(const cat_variable *)
{
        var3_read_cntr++;
        return 0;
}

static cat_return_state print_cmd_list(const cat_command *)
{
        return CAT_RETURN_STATE_PRINT_CMD_LIST_OK;
}

static cat_variable vars_ro[] = {
        { .type = CAT_VAR_INT_DEC, .data = &var2, .data_size = sizeof(var2), .access = CAT_VAR_ACCESS_READ_ONLY, .write = var2_write, .read = var2_read }
};

static cat_variable vars_wo[] = {
        { .type = CAT_VAR_INT_DEC, .data = &var3, .data_size = sizeof(var3), .access = CAT_VAR_ACCESS_WRITE_ONLY, .write = var3_write, .read = var3_read }
};

static cat_variable vars[] = {
        { .type = CAT_VAR_INT_DEC, .data = &var1, .data_size = sizeof(var1), .access = CAT_VAR_ACCESS_READ_WRITE, .write = var1_write, .read = var1_read },
        { .type = CAT_VAR_INT_DEC, .data = &var2, .data_size = sizeof(var2), .access = CAT_VAR_ACCESS_READ_ONLY, .write = var2_write, .read = var2_read },
        { .type = CAT_VAR_INT_DEC, .data = &var3, .data_size = sizeof(var3), .access = CAT_VAR_ACCESS_WRITE_ONLY, .write = var3_write, .read = var3_read }
};

static cat_variable vars_misc_ro[] = {
        { .type = CAT_VAR_INT_DEC, .data = &var1, .data_size = sizeof(var1), .access = CAT_VAR_ACCESS_READ_WRITE },
        { .name = "x", .type = CAT_VAR_INT_DEC, .data = &var_int8, .data_size = sizeof(var_int8), .access = CAT_VAR_ACCESS_READ_ONLY },
        { .name = "y", .type = CAT_VAR_INT_DEC, .data = &var_int16, .data_size = sizeof(var_int16), .access = CAT_VAR_ACCESS_READ_ONLY },
        { .type = CAT_VAR_INT_DEC, .data = &var_int32, .data_size = sizeof(var_int32), .access = CAT_VAR_ACCESS_READ_ONLY },
        { .type = CAT_VAR_UINT_DEC, .data = &var_uint8, .data_size = sizeof(var_uint8), .access = CAT_VAR_ACCESS_READ_ONLY },
        { .type = CAT_VAR_UINT_DEC, .data = &var_uint16, .data_size = sizeof(var_uint16), .access = CAT_VAR_ACCESS_READ_ONLY },
        { .type = CAT_VAR_UINT_DEC, .data = &var_uint32, .data_size = sizeof(var_uint32), .access = CAT_VAR_ACCESS_READ_ONLY },
        { .type = CAT_VAR_NUM_HEX, .data = &var_hex8, .data_size = sizeof(var_hex8), .access = CAT_VAR_ACCESS_READ_ONLY },
        { .type = CAT_VAR_NUM_HEX, .data = &var_hex16, .data_size = sizeof(var_hex16), .access = CAT_VAR_ACCESS_READ_ONLY },
        { .type = CAT_VAR_NUM_HEX, .data = &var_hex32, .data_size = sizeof(var_hex32), .access = CAT_VAR_ACCESS_READ_ONLY },
        { .type = CAT_VAR_BUF_HEX, .data = &var_buf, .data_size = sizeof(var_buf), .access = CAT_VAR_ACCESS_READ_ONLY },
        { .name = "msg", .type = CAT_VAR_BUF_STRING, .data = &var_string, .data_size = sizeof(var_string), .access = CAT_VAR_ACCESS_READ_ONLY }
};

static cat_variable vars_misc_wo[] = {
        { .type = CAT_VAR_INT_DEC, .data = &var1, .data_size = sizeof(var1), .access = CAT_VAR_ACCESS_READ_WRITE },
        { .name = "x", .type = CAT_VAR_INT_DEC, .data = &var_int8, .data_size = sizeof(var_int8), .access = CAT_VAR_ACCESS_WRITE_ONLY },
        { .name = "y", .type = CAT_VAR_INT_DEC, .data = &var_int16, .data_size = sizeof(var_int16), .access = CAT_VAR_ACCESS_WRITE_ONLY },
        { .type = CAT_VAR_INT_DEC, .data = &var_int32, .data_size = sizeof(var_int32), .access = CAT_VAR_ACCESS_WRITE_ONLY },
        { .type = CAT_VAR_UINT_DEC, .data = &var_uint8, .data_size = sizeof(var_uint8), .access = CAT_VAR_ACCESS_WRITE_ONLY },
        { .type = CAT_VAR_UINT_DEC, .data = &var_uint16, .data_size = sizeof(var_uint16), .access = CAT_VAR_ACCESS_WRITE_ONLY },
        { .type = CAT_VAR_UINT_DEC, .data = &var_uint32, .data_size = sizeof(var_uint32), .access = CAT_VAR_ACCESS_WRITE_ONLY },
        { .type = CAT_VAR_NUM_HEX, .data = &var_hex8, .data_size = sizeof(var_hex8), .access = CAT_VAR_ACCESS_WRITE_ONLY },
        { .type = CAT_VAR_NUM_HEX, .data = &var_hex16, .data_size = sizeof(var_hex16), .access = CAT_VAR_ACCESS_WRITE_ONLY },
        { .type = CAT_VAR_NUM_HEX, .data = &var_hex32, .data_size = sizeof(var_hex32), .access = CAT_VAR_ACCESS_WRITE_ONLY },
        { .type = CAT_VAR_BUF_HEX, .data = &var_buf, .data_size = sizeof(var_buf), .access = CAT_VAR_ACCESS_WRITE_ONLY },
        { .name = "msg", .type = CAT_VAR_BUF_STRING, .data = &var_string, .data_size = sizeof(var_string), .access = CAT_VAR_ACCESS_WRITE_ONLY }
};

static cat_command cmds[] = { { .name = "+VRW", .var = vars, .var_num = sizeof(vars) / sizeof(vars[0]) },
                              { .name = "+VRO", .var = vars_ro, .var_num = sizeof(vars_ro) / sizeof(vars_ro[0]) },
                              { .name = "+VWO", .var = vars_wo, .var_num = sizeof(vars_wo) / sizeof(vars_wo[0]) },
                              { .name = "+MRO", .var = vars_misc_ro, .var_num = sizeof(vars_misc_ro) / sizeof(vars_misc_ro[0]) },
                              { .name = "+MWO", .var = vars_misc_wo, .var_num = sizeof(vars_misc_wo) / sizeof(vars_misc_wo[0]) },
                              {
                                      .name = "#HELP",
                                      .run = print_cmd_list,
                              } };

static uint8_t buf[256];

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

        memset(ack_results, 0, sizeof(ack_results));
}

TEST(cAT, var_access)
{
        cat_object at;

        cat_init(&at, &desc, &iface, NULL);

        prepare_input("\nAT#HELP\n");
        while (cat_service(&at) != 0) {
        };

        EXPECT_STREQ(
                ack_results,
                "\nAT+VRW?\nAT+VRW=\nAT+VRW=?\n\nAT+VRO?\nAT+VRO=?\n\nAT+VWO=\nAT+VWO=?\n\nAT+MRO?\nAT+MRO=\nAT+MRO=?\n\nAT+MWO?\nAT+MWO=\nAT+MWO=?\n\nAT#HELP\n\nOK\n");

        prepare_input("\nAT+VRW=?\n");
        while (cat_service(&at) != 0) {
        };

        EXPECT_STREQ(ack_results, "\n+VRW=<INT8[RW]>,<INT8[RO]>,<INT8[WO]>\n\nOK\n");

        prepare_input("\nAT+VRO=?\n");
        while (cat_service(&at) != 0) {
        };

        EXPECT_STREQ(ack_results, "\n+VRO=<INT8[RO]>\n\nOK\n");

        prepare_input("\nAT+VWO=?\n");
        while (cat_service(&at) != 0) {
        };

        EXPECT_STREQ(ack_results, "\n+VWO=<INT8[WO]>\n\nOK\n");

        var2 = 1;
        prepare_input("\nAT+VRO=1\n");
        while (cat_service(&at) != 0) {
        };

        EXPECT_STREQ(ack_results, "\nERROR\n");
        EXPECT_EQ(var2, 1);

        var3 = 3;
        prepare_input("\nAT+VWO?\n");
        while (cat_service(&at) != 0) {
        };

        EXPECT_STREQ(ack_results, "\nERROR\n");
        EXPECT_EQ(var3, 3);

        var1 = -1;
        var2 = -2;
        var3 = -3;
        var2_write_cntr = 0;
        var3_read_cntr = 0;

        prepare_input("\nAT+VRW?\n");
        while (cat_service(&at) != 0) {
        };

        EXPECT_STREQ(ack_results, "\n+VRW=-1,-2,0\n\nOK\n");
        EXPECT_EQ(var2_write_cntr, 0);
        EXPECT_EQ(var3_read_cntr, 1);

        prepare_input("\nAT+VRW=1,2,3\n");
        while (cat_service(&at) != 0) {
        };

        EXPECT_STREQ(ack_results, "\nOK\n");
        EXPECT_EQ(var2_write_cntr, 1);
        EXPECT_EQ(var3_read_cntr, 1);
        EXPECT_EQ(var1, 1);
        EXPECT_EQ(var2, -2);
        EXPECT_EQ(var3, 3);

        var1 = 100;
        var_int8 = 1;
        var_int16 = 2;
        var_int32 = 3;
        var_uint8 = 4;
        var_uint16 = 5;
        var_uint32 = 6;
        var_hex8 = 7;
        var_hex16 = 8;
        var_hex32 = 9;
        var_buf[0] = 0x10;
        var_buf[1] = 0x11;
        var_buf[2] = 0x12;
        var_buf[3] = 0x13;
        strcpy(var_string, "test_string");

        prepare_input("\nAT+MWO?\n");
        while (cat_service(&at) != 0) {
        };

        EXPECT_STREQ(ack_results, "\n+MWO=100,0,0,0,0,0,0,0x00,0x0000,0x00000000,00000000,\"\"\n\nOK\n");

        prepare_input("\nAT+MWO=1,2,3,4,5,6,7,0x08,0x0009,0x0000000A,01020304,\"abc\"\n");
        while (cat_service(&at) != 0) {
        };

        EXPECT_STREQ(ack_results, "\nOK\n");
        EXPECT_EQ(var1, 1);
        EXPECT_EQ(var_int8, 2);
        EXPECT_EQ(var_int16, 3);
        EXPECT_EQ(var_int32, 4);
        EXPECT_EQ(var_uint8, 5);
        EXPECT_EQ(var_uint16, 6);
        EXPECT_EQ(var_uint32, 7);
        EXPECT_EQ(var_hex8, 8);
        EXPECT_EQ(var_hex16, 9);
        EXPECT_EQ(var_hex32, 10);
        EXPECT_EQ(var_buf[0], 0x01);
        EXPECT_EQ(var_buf[1], 0x02);
        EXPECT_EQ(var_buf[2], 0x03);
        EXPECT_EQ(var_buf[3], 0x04);
        EXPECT_STREQ(var_string, "abc");

        prepare_input("\nAT+MRO?\n");
        while (cat_service(&at) != 0) {
        };

        EXPECT_STREQ(ack_results, "\n+MRO=1,2,3,4,5,6,7,0x08,0x0009,0x0000000A,01020304,\"abc\"\n\nOK\n");

        prepare_input("\nAT+MRO=2,0,0,0,0,0,0,0x00,0x0000,0x00000000,00000000,\"cba\"\n");
        while (cat_service(&at) != 0) {
        };

        EXPECT_STREQ(ack_results, "\nOK\n");
        EXPECT_EQ(var1, 2);
        EXPECT_EQ(var_int8, 2);
        EXPECT_EQ(var_int16, 3);
        EXPECT_EQ(var_int32, 4);
        EXPECT_EQ(var_uint8, 5);
        EXPECT_EQ(var_uint16, 6);
        EXPECT_EQ(var_uint32, 7);
        EXPECT_EQ(var_hex8, 8);
        EXPECT_EQ(var_hex16, 9);
        EXPECT_EQ(var_hex32, 10);
        EXPECT_EQ(var_buf[0], 0x01);
        EXPECT_EQ(var_buf[1], 0x02);
        EXPECT_EQ(var_buf[2], 0x03);
        EXPECT_EQ(var_buf[3], 0x04);
        EXPECT_STREQ(var_string, "abc");
}
