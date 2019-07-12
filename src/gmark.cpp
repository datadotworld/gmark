#include "gmark.h"

namespace graph {

void abstract_graph_writer::add_random_edges(config::edge& c_edge) {
    if (c_edge.incoming_distrib.type == DISTRIBUTION::UNDEFINED) {
        add_random_edges_simple(c_edge);
    } else {
        add_random_edges_complex(c_edge);
    }
}

void abstract_graph_writer::add_random_edges_simple(config::edge& c_edge) {
    auto object_node_range = node_ranges_per_type[c_edge.object_type];
    size_t nb_objects = 1 + object_node_range.second - object_node_range.first;
    if(c_edge.outgoing_distrib.type == DISTRIBUTION::ZIPFIAN && c_edge.outgoing_distrib.arg1 == 0) {
        c_edge.outgoing_distrib.arg1 = nb_objects;
    }
    random_generator* gen = make_generator(c_edge.outgoing_distrib);
    uniform_random_generator uniform_gen(0, nb_objects);
    auto subject_node_range = node_ranges_per_type[c_edge.subject_type];
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

vector<size_t> generate_random_slots(pair<size_t, size_t> range, const distribution& distrib) {
    vector<size_t> vslots;
    random_generator* gen = make_generator(distrib);
    for (size_t node = range.first; node <= range.second; node++) {
        size_t nb_slots = gen->next();
        if (distrib.type == DISTRIBUTION::ZIPFIAN) {
        	nb_slots++;
        }
        for (size_t i = 0; i < nb_slots; i++) {
            vslots.push_back(node);
        }
    }
    delete gen;
    return vslots;
}

void abstract_graph_writer::add_random_edges_complex(config::edge& c_edge) {
    if(c_edge.outgoing_distrib.type == DISTRIBUTION::ZIPFIAN) {
        if(c_edge.outgoing_distrib.arg1 == 0) {
            auto object_node_range = node_ranges_per_type[c_edge.object_type];
            size_t nb_objects = 1 + object_node_range.second - object_node_range.first;
            c_edge.outgoing_distrib.arg1 = nb_objects;
        }
        if(c_edge.incoming_distrib.arg1 == 0) {
            auto subject_node_range = node_ranges_per_type[c_edge.subject_type];
            size_t nb_subjects = 1 + subject_node_range.second - subject_node_range.first;
            c_edge.incoming_distrib.arg1 = nb_subjects;
        }
    }

    vector<size_t> object_slots = generate_random_slots(node_ranges_per_type[c_edge.object_type], c_edge.incoming_distrib);
    size_t m = object_slots.size();
    vector<size_t> subject_slots = generate_random_slots(node_ranges_per_type[c_edge.subject_type], c_edge.outgoing_distrib);
    size_t n = subject_slots.size();

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

void abstract_graph_writer::build_graph (config::config& conf, int graphNumber) {
    nb_nodes = 0;
    this->conf = &conf;
    size_t type = 0;
    for (auto & typeconfig : conf.types) {
        add_vertices(type, typeconfig.size[graphNumber]);
        type++;
    }
    cout << "creating edges" << endl;
    for (config::edge& edge : conf.schema.edges) {
        cout << "add random edges: " << edge.subject_type << " " << edge.predicate << " " << edge.object_type << " " << edge.outgoing_distrib << " " << edge.incoming_distrib <<endl;
        add_random_edges(edge);
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
    print_edge(subject, predicate, object);
}

ntriple_graph_writer::ntriple_graph_writer (ostream& s) {
    stream = &s;
}

void ntriple_graph_writer::print_edge(size_t subject, size_t predicate, size_t object) {
    (*stream).write(reinterpret_cast<char*>(&subject), sizeof(size_t));
    (*stream).write(reinterpret_cast<char*>(&predicate), sizeof(size_t));
    (*stream).write(reinterpret_cast<char*>(&object), sizeof(size_t));
}

}
