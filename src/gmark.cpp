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



#include "gmark.h"
#include "config.h"

#include <cstdlib>
#include <vector>
#include <utility>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <ostream>
#include <iostream>
#include <cmath>
#include <random>
#include <algorithm>
#include <chrono>

namespace graph {

vector<size_t> generate_random_slots(pair<size_t,size_t> range, const distribution & distrib) {
    vector<size_t> vslots;

    random_generator * gen = make_generator(distrib);
    
    for (size_t node = range.first; node <= range.second; node++) {
        size_t nb_slots = gen->next();
        for (size_t i = 0; i < nb_slots; i++) {
            vslots.push_back(node);
        }
    }
    
    delete gen;
    
    return vslots;
}

void abstract_graph_writer::add_random_edges(config::edge & c_edge) {
    if (c_edge.incoming_distrib.type == DISTRIBUTION::UNDEFINED) {
        add_random_edges1(c_edge);
    } else {
        add_random_edges2(c_edge);
    }
}

void abstract_graph_writer::add_random_edges1(config::edge & c_edge) {
    auto subject_node_range = node_ranges_per_type[c_edge.subject_type];
    auto object_node_range = node_ranges_per_type[c_edge.object_type];
    size_t nb_objects = 1 + object_node_range.second - object_node_range.first;
    
    if(c_edge.outgoing_distrib.type == DISTRIBUTION::ZIPFIAN && c_edge.outgoing_distrib.arg1 == 0) {
        c_edge.outgoing_distrib.arg1 = nb_objects;
    }
    
    random_generator * gen = make_generator(c_edge.outgoing_distrib);
    uniform_random_generator uniform_gen(0, nb_objects);
    
    for (size_t subject = subject_node_range.first; subject <= subject_node_range.second; subject++) {
        size_t nb_edges = gen->next();
        for (size_t i = 0; i < nb_edges; i++) {
            size_t rnd = uniform_gen.next();
            //size_t rnd = rand() % nb_objects; // todo
            size_t object = object_node_range.first + rnd;
            //if (! has_edge(subject, c_edge.predicate, object)) {
            add_edge(subject, c_edge.predicate, object);
                    //    break;
                //}    
        }
    }
    
    delete gen;
}

void abstract_graph_writer::add_random_edges2(config::edge & c_edge) {
    auto subject_node_range = node_ranges_per_type[c_edge.subject_type];
    auto object_node_range = node_ranges_per_type[c_edge.object_type];
    size_t nb_subjects = 1 + subject_node_range.second - subject_node_range.first;
    size_t nb_objects = 1 + object_node_range.second - object_node_range.first;
    
    if(c_edge.outgoing_distrib.type == DISTRIBUTION::ZIPFIAN && c_edge.outgoing_distrib.arg1 == 0) {
        c_edge.outgoing_distrib.arg1 = nb_objects;
    }

    if(c_edge.incoming_distrib.type == DISTRIBUTION::ZIPFIAN && c_edge.incoming_distrib.arg1 == 0) {
        c_edge.incoming_distrib.arg1 = nb_subjects;
    }
    
    
    vector<size_t> subject_slots = generate_random_slots(node_ranges_per_type[c_edge.subject_type], c_edge.outgoing_distrib);
    vector<size_t> object_slots = generate_random_slots(node_ranges_per_type[c_edge.object_type], c_edge.incoming_distrib);
    size_t n = subject_slots.size();
    size_t m = object_slots.size();
    
    if (n < m) {
        random_shuffle(object_slots.begin(), object_slots.end());
    } else {
        random_shuffle(subject_slots.begin(), subject_slots.end());
        n = m;
    }
    for (size_t i = 0; i < n; i++) {
        add_edge(subject_slots[i], c_edge.predicate, object_slots[i]);
    }
}


void abstract_graph_writer::add_remaining_edges(size_t predicate, size_t nb_edges) {
    cout << "add_remaining_edges: " << predicate << " " << nb_edges << endl;
}

void abstract_graph_writer::build_graph (config::config & conf) {
    nb_nodes = 0;
    //size_t nb_vertices = conf.nb_vertices;
    //size_t node_index = 0;
       
    size_t type = 0;
    for (auto & typeconfig : conf.types) {
        add_vertices(type, typeconfig.size);
        type++;
    }
    
    created_edges.clear();
    created_edges.resize(conf.predicates.size());
    
    cout << "creating edges" << endl;
    for (config::edge & edge : conf.schema.edges) {
        cout << "add random edges: " << edge.subject_type << " " << edge.predicate << " " << edge.object_type << " " << edge.multiplicity << " " << edge.outgoing_distrib << " " << edge.incoming_distrib <<endl;
        add_random_edges(edge);
    }
    
    for (size_t predicate = 0; predicate < conf.predicates.size(); predicate++) {
        if(created_edges[predicate] < conf.predicates[predicate].size) {
            size_t nb_edges = conf.predicates[predicate].size - created_edges[predicate];
            add_remaining_edges(predicate, nb_edges);
        }
    }
    
    
    
}

void abstract_graph_writer::add_vertices(size_t type, size_t size) {
    if (size == 0) {
        size = 1;
    }
    cout << "add_vertices: " << type << " " << size << " " <<  nb_nodes << " " << nb_nodes+size-1 << endl;
    node_ranges_per_type.push_back(make_pair(nb_nodes, nb_nodes+size-1));
    nb_nodes += size;
}

void abstract_graph_writer::add_edge(size_t subject, size_t predicate, size_t object) {
    created_edges[predicate]++;
    print_edge(subject, predicate, object);
}


ntriple_graph_writer::ntriple_graph_writer (ostream & s) {
    stream = &s;
}

void ntriple_graph_writer::print_edge(size_t subject, size_t predicate, size_t object) {
    *stream << subject << " " << predicate << " " << object << "\n";
}


/*
int main(int argc, char ** argv) {
    config conf;
    conf.nb_vertices = 1000000;
    //conf.ratio_per_type = {{0, 0.3}, {1, 0.7}};
    conf.ratio_per_type = {{0, 1.0}};
    conf.graph.add_edge(0, 0, 0, DISTRIBUTION::UNIFORM, 10, 10); //, DISTRIBUTION::UNIFORM, 5, 5);
    //conf.graph.add_edge(0, 1, 1, DISTRIBUTION::ZIPFIAN, 2.0, 6);
    graph g(conf);
}
*/

}
