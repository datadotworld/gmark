#include <unistd.h>
#include <fstream>
#include <chrono>
#include <thread>

#include "config.h"
#include "gmark.h"
#include "configparser.h"

int main(int argc, char ** argv) {
	string conf_file = "../use-cases/test.xml";
    string graph_file;
    int c;
    bool print_alias = false;
    bool printNodeProperties = false;
    while ((c = getopt(argc, argv, "c:g:a:p")) != -1) {
        switch(c) {
            case 'c':
                conf_file = optarg;
                break;
            case 'g':
                graph_file = optarg;
                break;
            case 'a':
                print_alias = true;
                break;
            case 'p':
            	printNodeProperties = true;
            	break;
        }
    }
    config::config conf;
    conf.nb_nodes = {0};
    conf.nb_graphs = 0;
    conf.print_alias = print_alias;
    configparser::parse_config(conf_file, conf);
    if (graph_file == "") {
    	graph_file = "ignore/outputGraph";
    }
    for (size_t i=0; i<conf.nb_graphs; i++) {
        ofstream graph_stream;
        string fileName = graph_file + to_string(i) + ".txt";
        graph_stream.open(fileName);
        graph::ntriple_graph_writer writer(graph_stream);
        writer.build_graph(conf, i);
        graph_stream.close();
    }
}