/* gMark
 * Copyright (C) 2015-2016 Guillaume Bagan <guillaume.bagan@liris.cnrs.fr>
 * Copyright (C) 2015-2016 Angela Bonifati <angela.bonifati@univ-lyon1.fr>
 * Copyright (C) 2015-2016 Radu Ciucanu <radu.ciucanu@cs.ox.ac.uk>
 * Copyright (C) 2015-2016 George Fletcher <g.h.l.fletcher@tue.nl>
 * Copyright (C) 2015-2016 Aurélien Lemay <aurelien.lemay@univ-lille3.fr>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H

#include "pugixml.hpp"
#include "config.h"

namespace configparser {

using namespace std;

int parse_config(const string & filename, config::config & conf);
void parse_predicates(pugi::xml_node node, config::config & conf);
void parse_types(pugi::xml_node node, config::config & conf);
void parse_schema(pugi::xml_node node, config::config & conf);
void parse_workloads(pugi::xml_node node, config::config & conf);

distribution parse_distribution(pugi::xml_node node);

}

#endif
