/*
 * crontab_parser.cpp
 *
 *  Created on: 18 lut 2020
 *      Author: beku
 */

#include "crontab_parser.h"



std::string get_command_output(std::string cmd) {

   std::string data;
   FILE * stream;
   const int max_buffer = 256;
   char buffer[max_buffer];
   cmd.append(" 2>&1");

   stream = popen(cmd.c_str(), "r");
   if (stream) {
   while (!feof(stream))
   if (fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);
   pclose(stream);
   }
   return data;
}


void replace_string_in_place(std::string* subject, const std::string& search,
                             const std::string& replace) {
  size_t pos = 0;
  while ((pos = subject->find(search, pos)) != std::string::npos) {
    subject->replace(pos, search.length(), replace);
    pos += replace.length();
  }
}

std::vector< std::string > split(const std::string& value, const char delimiter )
{
	std::vector< std::string > result;

	std::string token = "";

	std::stringstream stream( value );

    while( getline( stream, token, delimiter ) )
    {
        result.push_back( token );
    }

    return result;
}

std::string expand( const std::string& expression )
{
	std::string result = "";

    if ( expression.find( "-" ) not_eq std::string::npos )
    {
		std::vector< std::string > rangearray = split( expression, '-' );

		int start = std::stoi( rangearray[ 0 ] );
		int stop = std::stoi( rangearray[ 1 ] );

	    for(int index = start; index <= stop; index++ )
	    {
	        result.append( std::to_string( index ) + "," );
	    }
	}
	else
	{
        result = expression + ",";
	}

    return result;
}

std::vector< std::string > parse(std::string expression, int maximum, int minimum)
{
	std::vector< std::string > subexpressions;

	if ( expression.find( "," ) not_eq std::string::npos )
	{
		subexpressions = split( expression, ',' );
	}
	else
	{
	    subexpressions.push_back(expression);
	}

	std::string rangeitems;

	for (const auto& subexpression : subexpressions)
	{
	    if (subexpression.find("/") not_eq std::string::npos) // handle */N syntax
	    {
	        for (int a = 1; a <= maximum; a++)
	        {
	            if (a % stoi(subexpression.substr(subexpression.find("/")+1)) == 0)
	            {
	                if(a == maximum)
	                {
	                    rangeitems.append(std::to_string(minimum));
	                }
	                else
	                {
	                    rangeitems.append(std::to_string(a));
	                }

	                rangeitems.append(",");
	            }
	        }
	    }
	    else
	    {
	        if(subexpression == "*")
	        {


	        	rangeitems.append(expand(std::to_string(minimum) + "-" + std::to_string(maximum)));
	        }
	        else
	        {
	            rangeitems.append(expand(subexpression));
	        }
	    }
	}

   return split( rangeitems, ',' );
}
