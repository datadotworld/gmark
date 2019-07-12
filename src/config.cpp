#include "config.h"

namespace config {

edge::edge (size_t s_type, size_t pred, size_t o_type, const distribution& odistrib, const distribution& idistrib){
    subject_type = s_type;
    predicate = pred;
    object_type = o_type;
    outgoing_distrib = odistrib;
    incoming_distrib = idistrib;
}

void schem::add_edge(size_t subject_type,  size_t predicate, size_t object_type, const distribution& outgoing_dist, const distribution& incoming_dist) {
	edges.emplace_back(subject_type, predicate, object_type, outgoing_dist, incoming_dist);
}
    
void schem::add_edge(size_t subject_type, size_t predicate, size_t object_type, DISTRIBUTION::type type, double arg1, double arg2) {
    distribution outgoing_dist(type, arg1, arg2);
    distribution incoming_dist(DISTRIBUTION::UNDEFINED, 0, 0);
    add_edge(subject_type, predicate, object_type, outgoing_dist, incoming_dist);
}
    
void schem::add_edge(size_t subject_type, size_t predicate, size_t object_type, DISTRIBUTION::type outgoing_type, double outgoing_arg1, double outgoing_arg2, DISTRIBUTION::type incoming_type, double incoming_arg1, double incoming_arg2) {
    distribution outgoing_dist(outgoing_type, outgoing_arg1, outgoing_arg2);
    distribution incoming_dist(incoming_type, incoming_arg1, incoming_arg2);
    add_edge(subject_type, predicate, object_type, outgoing_dist, incoming_dist);
}
     
}