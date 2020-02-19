/*
 * crontab_parser.h
 *
 *  Created on: 18 lut 2020
 *      Author: beku
 */

#ifndef CRONTAB_PARSER_H_
#define CRONTAB_PARSER_H_

#include <cstdio>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

const int MINUTESPERHOUR = 60;
const int HOURESPERDAY = 24;
const int DAYSPERWEEK = 7;
const int MONTHSPERYEAR = 12;
const int DAYSPERMONTH = 31;

std::string get_command_output(std::string cmd);

void replace_string_in_place(std::string* subject, const std::string& search,
                             const std::string& replace);

std::vector<std::string> split(const std::string& value, const char delimiter);
std::string expand(const std::string& expression);
std::vector<std::string> parse(std::string expression, int maximum,
                               int minimum);

#endif /* CRONTAB_PARSER_H_ */
