#ifndef GMARK_CONFIG_H
#define GMARK_CONFIG_H

#include "randomgen.h"

namespace config {

using namespace std;

class type {
public:
    string alias;
    vector<size_t> size;
    bool scalable;
    double proportion;
    type () {
        alias = "";
        size.push_back(0);
        scalable = true;
        proportion = 0.0;
    }
    type (const string& alias_, vector<size_t> size_, bool scalable_, double proportion_) {
        alias = alias_;
        size = size_;
        scalable = scalable_;
        proportion = proportion_;
    }
};

class predicate {
public:
   string alias;
   vector<size_t> size;
   double proportion;
   predicate () {
       alias = "";
       size.push_back(0);
       proportion = 0;
    }
   predicate (const string& alias_, vector<size_t> size_, double proportion_) {
        alias = alias_;
        size = size_;
        proportion = proportion_;
    }
};


class edge {
public:
    size_t subject_type;
    size_t predicate;
    size_t object_type;
    distribution outgoing_distrib;
    distribution incoming_distrib;
    edge (size_t s_type, size_t pred, size_t o_type, const distribution& odistrib, const distribution& idistrib);
};

class schem {
public:
    vector<edge> edges;
    void add_edge(size_t subject_type, size_t predicate, size_t object_type, const distribution& outgoing_dist, const distribution& incoming_dist);
    void add_edge(size_t subject_type, size_t predicate, size_t object_type, DISTRIBUTION::type type, double arg1, double arg2);
    void add_edge(size_t subject_type, size_t predicate, size_t object_type, DISTRIBUTION::type outgoing_type, double outgoing_arg1, double outgoing_arg2, DISTRIBUTION::type incoming_type, double incoming_arg1, double incoming_arg2);
};

class config {
public:
	size_t nb_graphs;
    string input;
    vector<size_t> nb_nodes;
    vector<size_t> nb_edges;
    vector<type> types;
    distribution node_distribution;
    vector<predicate> predicates;
    distribution predicate_distribution;
    schem schema;
    bool print_alias;
};

}

#endif