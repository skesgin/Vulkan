/**
 * VKTS - VulKan ToolS.
 *
 * The MIT License (MIT)
 *
 * Copyright (c) since 2014 Norbert Nopper
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef VKTS_FN_JSON_TOKENS_HPP_
#define VKTS_FN_JSON_TOKENS_HPP_

#include <vkts/core/vkts_core.hpp>

namespace vkts
{

// Structural tokens

static const std::string JSON_left_square_bracket = "[";
static const std::string JSON_left_curly_bracket = "{";
static const std::string JSON_right_square_bracket = "]";
static const std::string JSON_right_curly_bracket = "}";
static const std::string JSON_colon = ":";
static const std::string JSON_comma = ",";

// Literal name tokens

static const std::string JSON_true = "true";
static const std::string JSON_false = "false";
static const std::string JSON_null = "null";

//

static const std::string JSON_quotation_mark = "\"";
static const std::string JSON_reverse_solidus = "\\";
static const std::string JSON_solidus = "/";

static const std::string JSON_Decode_backspace = "b";
static const std::string JSON_Decode_form_feed = "f";
static const std::string JSON_Decode_line_feed = "n";
static const std::string JSON_Decode_carriage_return = "r";
static const std::string JSON_Decode_character_tabulation = "t";

static const std::string JSON_Encode_backspace = "\b";
static const std::string JSON_Encode_form_feed = "\f";
static const std::string JSON_Encode_line_feed = "\n";
static const std::string JSON_Encode_carriage_return = "\r";
static const std::string JSON_Encode_character_tabulation = "\t";

//

static const std::string JSON_space = " ";

//

static const std::string JSON_plus = "+";
static const std::string JSON_minus = "-";
static const std::string JSON_point = ".";

static const std::string JSON_A = "A";
static const std::string JSON_B = "B";
static const std::string JSON_C = "C";
static const std::string JSON_D = "D";
static const std::string JSON_E = "E";
static const std::string JSON_F = "F";

static const std::string JSON_a = "a";
static const std::string JSON_b = "b";
static const std::string JSON_c = "c";
static const std::string JSON_d = "d";
static const std::string JSON_e = "e";
static const std::string JSON_f = "f";

static const std::string JSON_0 = "0";
static const std::string JSON_1 = "1";
static const std::string JSON_2 = "2";
static const std::string JSON_3 = "3";
static const std::string JSON_4 = "4";
static const std::string JSON_5 = "5";
static const std::string JSON_6 = "6";
static const std::string JSON_7 = "7";
static const std::string JSON_8 = "8";
static const std::string JSON_9 = "9";

static const std::string JSON_u = "u";

static const int32_t JSON_C0_start = 0x00;
static const int32_t JSON_C0_end = 0x1F;
static const int32_t JSON_C1_start = 0x7F;
static const int32_t JSON_C1_end = 0x9F;

}

#endif /* VKTS_FN_JSON_TOKENS_HPP_ */
