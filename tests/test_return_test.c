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
static char cmd_results[256];
static char ack_results[256];

static char const *input_text;
static size_t input_index;

static int var_x;
static cat_object at;

static cat_return_state ret;
static bool ret_error;

static cat_return_state cmd_test(const cat_command *cmd, char *data, size_t *data_size, size_t max_data_size)
{
        (void)max_data_size; // Unused
        strcat(cmd_results, " test:");
        strcat(cmd_results, cmd->name);

        if (cat_is_hold(&at) == CAT_STATUS_HOLD) {
                var_x++;
                if (var_x > 4) {
                        ret = (ret_error == false) ? CAT_RETURN_STATE_HOLD_EXIT_OK : CAT_RETURN_STATE_HOLD_EXIT_ERROR;
                } else {
                        if (var_x == 4) {
                                strcpy(data, "test");
                                *data_size = strlen(data);
                        }
                        ret = CAT_RETURN_STATE_DATA_NEXT;
                }
        } else {
                if ((ret == CAT_RETURN_STATE_DATA_NEXT) || (ret == CAT_RETURN_STATE_NEXT)) {
                        var_x++;
                        if (var_x > 2)
                                ret = CAT_RETURN_STATE_DATA_OK;
                } else if (ret == CAT_RETURN_STATE_HOLD) {
                        cat_trigger_unsolicited_test(&at, cmd);
                }
        }

        return ret;
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

static cat_io_interface iface = { .read = read_char, .write = write_char };

static void prepare_input(const char *text)
{
        input_text = text;
        input_index = 0;
        var_x = 1;

        memset(ack_results, 0, sizeof(ack_results));
        memset(cmd_results, 0, sizeof(cmd_results));
}

static const char test_case_1[] = "\nAT+CMD=?\n";

int main(void)
{
        cat_init(&at, &desc, &iface, NULL);

        ret = CAT_RETURN_STATE_ERROR;
        prepare_input(test_case_1);
        while (cat_service(&at) != 0) {
        };

        assert(strcmp(ack_results, "\nERROR\n") == 0);
        assert(strcmp(cmd_results, " test:+CMD") == 0);

        ret = CAT_RETURN_STATE_DATA_OK;
        prepare_input(test_case_1);
        while (cat_service(&at) != 0) {
        };

        assert(strcmp(ack_results, "\n+CMD=<X:INT32[RW]>\n\nOK\n") == 0);
        assert(strcmp(cmd_results, " test:+CMD") == 0);

        ret = CAT_RETURN_STATE_DATA_NEXT;
        prepare_input(test_case_1);
        while (cat_service(&at) != 0) {
        };

        assert(strcmp(ack_results, "\n+CMD=<X:INT32[RW]>\n\n+CMD=<X:INT32[RW]>\n\nOK\n") == 0);
        assert(strcmp(cmd_results, " test:+CMD test:+CMD") == 0);

        ret = CAT_RETURN_STATE_NEXT;
        prepare_input(test_case_1);
        while (cat_service(&at) != 0) {
        };

        assert(strcmp(ack_results, "\n+CMD=<X:INT32[RW]>\n\nOK\n") == 0);
        assert(strcmp(cmd_results, " test:+CMD test:+CMD") == 0);

        ret = CAT_RETURN_STATE_OK;
        prepare_input(test_case_1);
        while (cat_service(&at) != 0) {
        };

        assert(strcmp(ack_results, "\nOK\n") == 0);
        assert(strcmp(cmd_results, " test:+CMD") == 0);

        ret_error = false;
        ret = CAT_RETURN_STATE_HOLD;
        prepare_input(test_case_1);
        while (cat_service(&at) != 0) {
        };

        assert(strcmp(ack_results, "\n+CMD=<X:INT32[RW]>\n\n+CMD=<X:INT32[RW]>\n\ntest\n\nOK\n") == 0);
        assert(strcmp(cmd_results, " test:+CMD test:+CMD test:+CMD test:+CMD test:+CMD") == 0);

        ret_error = true;
        ret = CAT_RETURN_STATE_HOLD;
        prepare_input(test_case_1);
        while (cat_service(&at) != 0) {
        };

        assert(strcmp(ack_results, "\n+CMD=<X:INT32[RW]>\n\n+CMD=<X:INT32[RW]>\n\ntest\n\nERROR\n") == 0);
        assert(strcmp(cmd_results, " test:+CMD test:+CMD test:+CMD test:+CMD test:+CMD") == 0);

        return 0;
}
