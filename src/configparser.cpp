#include "configparser.h"
#include "pugixml.hpp"
#include "config.h"

#include <iostream>

namespace configparser {

int parse_config(config::config& conf) {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(conf.input.c_str());

    if (!result) {
        cerr << "XML [" << conf.input << "] parsed with errors, attr value: [" << doc.child("node").attribute("attr").value() << "]" << endl;
        cerr << "Error description: " << result.description() << endl;
        cerr << "Error offset: " << result.offset << endl;   
        return 0;
    }
    
    pugi::xml_node root = doc.child("generator");
   
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

void parse_predicates(pugi::xml_node node, config::config& conf) {
    size_t size = node.child("size").text().as_uint();
    conf.predicates.resize(size);

    conf.predicate_distribution = parse_distribution(node.child("distribution"));
    
    for (pugi::xml_node alias_node : node.children("alias")) {
        size_t id = alias_node.attribute("symbol").as_uint();//
        string name = alias_node.text().get();
        if (id < 0 || id >= size) {
            cerr << "id " << id << " is out of range" << endl;
            continue;
        }
        conf.predicates[id].alias = name;
    }
    
    for (pugi::xml_node proportion_node : node.children("proportion")) {
        size_t id = proportion_node.attribute("symbol").as_uint();
        double proportion = proportion_node.text().as_double();
        if (id < 0 || id >= size) {
            cerr << "id " << id << " is out of range" << endl;
            continue;
        }
        conf.predicates[id].proportion = proportion;
        conf.predicates[id].size = (size_t) (proportion * conf.nb_edges);
    }
}

void parse_types(pugi::xml_node node, config::config& conf) {
    size_t size = node.child("size").text().as_uint();
    conf.types.resize(size);
    
    for (pugi::xml_node alias_node : node.children("alias")) {
        size_t id = alias_node.attribute("type").as_uint();
        string name = alias_node.text().get();
        if (id < 0 || id >= size) {
            cerr << "id " << id << " is out of range" << endl;
            continue;
        }
        conf.types[id].alias = name;
    }
    
    for (pugi::xml_node proportion_node : node.children("proportion")) {
    	size_t id = proportion_node.attribute("type").as_uint();
    	double proportion = proportion_node.text().as_double();
        if (id < 0 || id >= size) {
            cerr << "id " << id << " is out of range" << endl;
            continue;
        }
        conf.types[id].size = (size_t) (proportion * conf.nb_nodes);
        if (proportion * conf.nb_nodes > 0 && conf.types[id].size == 0) {
            conf.types[id].size = 1;
        }
        conf.types[id].proportion = proportion;
    }
    
    for (pugi::xml_node fixed_node : node.children("fixed")) {
        size_t id = fixed_node.attribute("type").as_uint();
        size_t size2 = fixed_node.text().as_ullong();
        if (id < 0 || id >= size) {
            cerr << "id " << id << " is out of range" << endl;
            continue;
        }
        conf.types[id].size = size2;
        conf.types[id].proportion = -1;
    }

    for (pugi::xml_node alias_node : node.children("alias")) {
    	size_t id = alias_node.attribute("type").as_uint();
        if (conf.types[id].size == 0) {
            conf.types[id].proportion = 1.0 / size;
            conf.types[id].size =  conf.types[id].proportion * conf.nb_nodes;
        }
	}
}

void parse_schema(pugi::xml_node node, config::config& conf) {
	for (pugi::xml_node source_node : node.children("source")) {
        size_t source_type = source_node.attribute("type").as_uint();
        for (pugi::xml_node target_node : source_node.children("target")) {
            char multiplicity = '*';
            size_t target_type = target_node.attribute("type").as_uint();
            size_t symbol = target_node.attribute("symbol").as_uint();
            string mult_string = target_node.attribute("multiplicity").value();
            if (mult_string.size() > 0 && (mult_string[0] == '?' || mult_string[0] == '+' || mult_string[0] == '1')) {
                multiplicity = mult_string[0];
            }
            pugi::xml_node outdistribution_node = target_node.child("outdistribution");
            distribution outdistribution = parse_distribution(outdistribution_node);
            pugi::xml_node indistribution_node = target_node.child("indistribution");
            distribution indistribution = parse_distribution(indistribution_node);

            if (multiplicity == '1') {
                outdistribution = distribution(DISTRIBUTION::UNIFORM, 1, 1);
            }
            else if (multiplicity == '?') {
                outdistribution = distribution(DISTRIBUTION::UNIFORM, 0, 1);
            }
            
            if(outdistribution.type == DISTRIBUTION::UNDEFINED) {
                outdistribution = distribution(DISTRIBUTION::ZIPFIAN, 0, 2.5);
            }
            
            conf.schema.add_edge(source_type, symbol, target_type, outdistribution, indistribution);
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