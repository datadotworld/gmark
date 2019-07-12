#ifndef GMARK_H
#define GMARK_H

#include <vector>
#include <utility>
#include <ostream>
#include <iostream>

#include "randomgen.h"
#include "config.h"

namespace graph {

using namespace std;

vector<size_t> generate_random_slots(pair<size_t, size_t> range, const distribution& distrib);

class abstract_graph_writer {
private:
    size_t nb_nodes;
    vector<pair<size_t, size_t>> node_ranges_per_type;
    void add_random_edges_simple(config::edge& c_edge);
    void add_random_edges_complex(config::edge& c_edge);
protected:
    config::config* conf;
public:
    void build_graph (config::config& conf);
    void add_vertices(size_t type, size_t size);
    void add_edge(size_t subject, size_t predicate, size_t object);
    virtual void print_edge(size_t subject, size_t predicate, size_t object) = 0;
    void add_random_edges(config::edge& c_edge);
};   

class ntriple_graph_writer : public abstract_graph_writer {
private:
    ostream* stream;
public:  
    ntriple_graph_writer (ostream& s);
    void print_edge(size_t subject, size_t predicate, size_t object);
};

}

#endif
