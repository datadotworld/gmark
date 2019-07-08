#include "configparser.h"
#include "pugixml.hpp"
#include "config.h"

#include <iostream>

namespace configparser {

int parse_config(const string & filename, config::config & conf) {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(filename.c_str());

    if (! result) {
        cerr << "XML [" << filename << "] parsed with errors, attr value: [" << doc.child("node").attribute("attr").value() << "]" << endl;
        cerr << "Error description: " << result.description() << endl;
        cerr << "Error offset: " << result.offset << endl;   
        return 0;
    }
    
    pugi::xml_node root = doc.child("generator");

    if (conf.nb_graphs == 0) {
    	if (root.child("size")) {
    	    conf.nb_graphs = root.child("size").text().as_uint();
    	} else {
    	    conf.nb_graphs = 1;
    	}
    }
//    pugi::xml_node graph_node = root.child("graph");

    if (conf.nb_nodes[0] == 0) {
    	conf.nb_nodes.resize(conf.nb_graphs);
    	conf.nb_edges.resize(conf.nb_graphs);

    	int i = 0;
		for (pugi::xml_node graph_node : root.children("graph")) {
			if (i >= conf.nb_graphs) {
				cout << "Number of graphs is greater than specified by the size" << endl;
			}
			conf.nb_nodes[i] = graph_node.child("nodes").text().as_ullong();
//			cout << "Graph " << i << ": " << conf.nb_nodes[i] << endl;
			conf.nb_edges[i] = graph_node.child("edges").text().as_ullong();
			i++;
		}
    }

    conf.input = filename;
   
    pugi::xml_node predicates = root.child("predicates");
    if (!predicates.empty()) {
        parse_predicates(predicates, conf);
    }
    
    pugi::xml_node types = root.child("types");
    if (!types.empty()) {
        parse_types(types, conf);
    }
    
    pugi::xml_node schema = root.child("schema");
    if (!schema.empty()) {
        parse_schema(schema, conf);
    }
    
    return 1;
}

void parse_predicates(pugi::xml_node node, config::config & conf) {
    size_t size = node.child("size").text().as_uint();
    //cout << "predicates size: " << size << endl;
    conf.predicates.resize(size);

    conf.predicate_distribution = parse_distribution(node.child("distribution"));
    
    for (pugi::xml_node alias_node : node.children("alias")) {
        size_t id = alias_node.attribute("symbol").as_uint();//
        string name = alias_node.text().get();
        //cout << "alias " << id << ", " << name  << endl;
        if (id < 0 || id >= size) {
            cerr << "id " << id << " is out of range" << endl;
            continue;
        }

        conf.predicates[id].alias = name;
    }
    
    for (pugi::xml_node proportion_node : node.children("proportion")) {
        size_t id = proportion_node.attribute("symbol").as_uint();
        conf.predicates[id].size.resize(conf.nb_graphs);
        double proportion = proportion_node.text().as_double();
        if (id < 0 || id >= size) {
            cerr << "id " << id << " is out of range" << endl;
            continue;
        }
        conf.predicates[id].proportion = proportion;
        for (size_t i=0; i<conf.nb_edges.size(); i++) {
        	conf.predicates[id].size.push_back((size_t) (proportion * conf.nb_edges[i]));
        }
    }
}

void parse_types(pugi::xml_node node, config::config & conf) {
    size_t size = node.child("size").text().as_uint();
    //cout << "type size: " << size << endl;
    conf.types.resize(size);
    
    for (pugi::xml_node alias_node : node.children("alias")) {
        size_t id = alias_node.attribute("type").as_uint();//
        string name = alias_node.text().get();
        //cout << "alias " << id << ", " << name  << endl;
        if (id < 0 || id >= size) {
            cerr << "id " << id << " is out of range" << endl;
            continue;
        }
        conf.types[id].alias = name;
    }
    
    for (pugi::xml_node proportion_node : node.children("proportion")) {
    	size_t id = proportion_node.attribute("type").as_uint();
    	conf.types[id].size.resize(conf.nb_graphs);

    	double proportion = proportion_node.text().as_double();
        //cout << "proportion " << id << ", " << proportion  << endl;
        if (id < 0 || id >= size) {
            cerr << "id " << id << " is out of range" << endl;
            continue;
        }

        for (int i=0; i<conf.nb_graphs; i++) {
        	conf.types[id].size[i] = (size_t) (proportion * conf.nb_nodes[i]);
        	if (proportion * conf.nb_nodes[i] > 0 && conf.types[id].size[i] == 0) {
				conf.types[id].size[i] = 1;
			}
//        	cout << "i: " << i << ". Graphsize: " << conf.nb_nodes[i] << ": ";
//        	cout << "Type " << conf.types[id].alias << ": " << conf.types[id].size[i] << endl;
        }
        conf.types[id].scalable = true;
        conf.types[id].proportion = proportion;
    }
    
    for (pugi::xml_node fixed_node : node.children("fixed")) {
        size_t id = fixed_node.attribute("type").as_uint();
        conf.types[id].size.resize(conf.nb_graphs);

        size_t size2 = fixed_node.text().as_uint();
        //cout << "fixed " << id << ", " << size  << endl;
        if (id < 0 || id >= size) {
            cerr << "id " << id << " is out of range" << endl;
            continue;
        }
        for (int i=0; i<conf.nb_graphs; i++) {
        	conf.types[id].size[i] = size2;
        }
        conf.types[id].scalable = false;
        conf.types[id].proportion = -1;
    }

    for (pugi::xml_node alias_node : node.children("alias")) {
    	size_t id = alias_node.attribute("type").as_uint();
    	conf.types[id].size.resize(conf.nb_graphs);
//    	cout << "ID: " << id << endl;
//    	cout << "Name: " << alias_node.text().get() << endl;
    	for (int i=0; i<conf.nb_graphs; i++) {
//			cout << "Before: " << conf.types[id].size[i] << endl;
    		if (conf.types[id].size[i] == 0) {
				conf.types[id].scalable = true;
				conf.types[id].proportion = 1.0 / size;
				conf.types[id].size[i] =  conf.types[id].proportion * conf.nb_nodes[i];
			}
//    		cout << "After: " << conf.types[id].size[i] << endl;
    	}
	}
}

void parse_schema(pugi::xml_node node, config::config & conf) {
//    int currentEdgeTypeId = 0;
	for (pugi::xml_node source_node : node.children("source")) {
        size_t source_type = source_node.attribute("type").as_uint();
        for (pugi::xml_node target_node : source_node.children("target")) {
            char multiplicity = '*';
            size_t target_type = target_node.attribute("type").as_uint();
            size_t symbol = target_node.attribute("symbol").as_uint();
            string mult_string = target_node.attribute("multiplicity").value();
            size_t edgeTypeId = target_node.attribute("edge_type").as_uint();
//            size_t edgeTypeId = currentEdgeTypeId;
//            currentEdgeTypeId++;
            if (mult_string.size() > 0 && (mult_string[0] == '?' || mult_string[0] == '+' || mult_string[0] == '1')) {
                multiplicity = mult_string[0];
            }
            pugi::xml_node outdistribution_node = target_node.child("outdistribution");
            distribution outdistribution = parse_distribution(outdistribution_node);
            pugi::xml_node indistribution_node = target_node.child("indistribution");
            distribution indistribution = parse_distribution(indistribution_node);
            
            pugi::xml_node sfNode = target_node.child("scalefactor");
            int sf = 0;
            if (!sfNode.empty()) {
            	sf = sfNode.text().as_int();
            	if (sf < 0) {
            		sf = 0;
            		cout << "The scale-factor should be greater than 0" << endl;
            	}
            }

            if (multiplicity == '1') { // && outdistribution.type == DISTRIBUTION::UNDEFINED) {
                outdistribution = distribution(DISTRIBUTION::UNIFORM, 1, 1);
            }
            else if (multiplicity == '?') {// && outdistribution.type == DISTRIBUTION::UNDEFINED) {
                outdistribution = distribution(DISTRIBUTION::UNIFORM, 0, 1);
            }
            
            if(outdistribution.type == DISTRIBUTION::UNDEFINED) {
                outdistribution = distribution(DISTRIBUTION::ZIPFIAN, 0, 2.5);
            }
            
            if(indistribution.type == DISTRIBUTION::UNDEFINED) {
				indistribution = distribution(DISTRIBUTION::NORMAL, 0, 1);
			}
            
            conf.schema.add_edge(source_type, symbol, target_type, multiplicity, edgeTypeId, sf, outdistribution, indistribution);
            //cout << "conf.add_edge "  << source_type << " " << symbol << " " << target_type << " " << multiplicity << " " << outdistribution << " " << indistribution <<endl;
            
        }
    }
}


distribution parse_distribution(pugi::xml_node node) {
    distribution dist;
    if (!node.empty()) {
        string type = node.attribute("type").value();
        if (type == "uniform") {
            size_t min = node.child("min").text().as_ullong();
            size_t max = node.child("max").text().as_ullong();
            dist.type = DISTRIBUTION::UNIFORM;
            dist.arg1 = min;
            dist.arg2 = max;
        } else if (type == "zipfian") {
//            size_t n = node.child("n").text().as_uint();
            double alpha = node.child("alpha").text().as_double();
            dist.type = DISTRIBUTION::ZIPFIAN;
            dist.arg1 = 0;
            dist.arg2 = alpha;
        } else if (type == "normal" or type == "gaussian") {
            double mean = node.child("mu").text().as_double();
            double stddev = node.child("sigma").text().as_double();
            dist.type = DISTRIBUTION::NORMAL;
            dist.arg1 = mean;
            dist.arg2 = stddev;
        }
    }
    return dist;
}

}